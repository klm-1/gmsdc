#include "flowgraph.h"

#include <iomanip>
#include <fstream>
#include <sstream>
#include <cassert>
#include <stack>
#include <set>

#include "algext.h"
#include "graphmlwriter.h"


FlowGraph::Options FlowGraph::Options::Debug()
{
    Options ret;
    ret.logSteps = true;
    ret.stepLogPrefix = "step_";
    return ret;
}

FlowGraph::Options FlowGraph::Options::Release()
{
    Options ret;
    ret.logSteps = false;
    return ret;
}

FlowGraph::FlowGraph()
    : entry_(nullptr)
    , nodes_()
    , step_(0)
{}

FlowGraph::FlowGraph(const program_t& p)
    : FlowGraph()
{
    /* Find split points */
    std::vector<int> leaders;
    int last_addr = 0;
    for (const AsmCommand& cmd : p)
	{
        if (cmd.operation() == Operation::Jmp ||
            cmd.operation() == Operation::JZ ||
            cmd.operation() == Operation::JNZ)
		{
            leaders.push_back(cmd.addr + cmd.size);
            leaders.push_back(cmd.jumpAddr());
        }

        last_addr = cmd.addr + cmd.size;
    }

    leaders.push_back(last_addr);
    std::sort(leaders);
    leaders.erase(std::unique(leaders.begin(), leaders.end()), leaders.end());

    auto ldr = leaders.begin();
    program_t currentBB;
    std::map<int, Node*> addr_map;

    /* Create nodes */
    if (!p.empty() && p.front().addr == *ldr)
	{
        ++ldr;
    }
    for (const AsmCommand& cmd : p)
	{
        currentBB.push_back(cmd);

        if (cmd.addr + cmd.size == *ldr)
		{
            Node* n = createTerminal(currentBB);
            addr_map[n->addr] = n;
            currentBB.clear();
            ++ldr;
        }
    }
    Node* nop = createEmptyTerminal(last_addr);
    addr_map[last_addr] = nop;

    /* Link nodes */
    for (auto& ptr : nodes_)
	{
        auto& bb = ptr->tree->baseblock();

        if (bb.valid())
		{
            if (bb.hasJumpIfTrue())
			{
                addLink(ptr.get(), addr_map.find(bb.jumpTargetAddr())->second);
                addLink(ptr.get(), addr_map.find(bb.pastTheEndAddr())->second);

            }
			else if (bb.hasJumpIfFalse())
			{
                addLink(ptr.get(), addr_map.find(bb.pastTheEndAddr())->second);
                addLink(ptr.get(), addr_map.find(bb.jumpTargetAddr())->second);

            }
			else if (bb.hasJumpAlways())
			{
                addLink(ptr.get(), addr_map.find(bb.jumpTargetAddr())->second);

            }
			else
			{
                addLink(ptr.get(), addr_map.find(bb.pastTheEndAddr())->second);
            }
        }
    }
}

void FlowGraph::cleanupNops()
{
    std::vector<Node*> to_erase;
    for (auto& n : nodes_)
	{
        if (n.get() != entry_ && n->isNop() && n->inputsCount() == 0)
		{
            to_erase.push_back(n.get());
        }
    }
    for (Node* n : to_erase)
	{
        eraseNode(n);
    }
}

void FlowGraph::rerouteBreakContinue()
{
    std::vector<Node*> loop_headers;
    for (auto& n : nodes_)
	{
        for (Node* to : n->outputs)
		{
            if (to->addr <= n->addr)
			{
                loop_headers.push_back(to);
            }
        }
    }
    std::sort(loop_headers, [](auto a, auto b)
	{
        return a->addr > b->addr;
    });
    loop_headers.erase(std::unique(loop_headers), loop_headers.end());

    for (Node* hdr : loop_headers) {
        Node* bottom = *std::max_element(hdr->inputs, [](auto a, auto b)
		{
            return a->addr < b->addr;
        });
        Node* pastTheEnd = bottom->outputsCount() == 2 ? bottom->firstOutput() : hdr->lastOutput();

        depthFirstWalk(hdr, [&](Node * node) mutable
		{
            if (node == pastTheEnd)
            {
                return VisitResult::Continue;
            }
            if (node != hdr && node != bottom)
            {
                if (removeLink(node, hdr))
				{
                    Node* to = fallthroughNode(node);
                    Node* ncontinue = mergeNodes(ControlTree::Type::Continue, { createEmptyTerminal() });
                    addLink(node, ncontinue);
                    addLink(ncontinue, to);
                }
                if (removeLink(node, pastTheEnd))
				{
                    Node* to = fallthroughNode(node);
                    Node* term = createEmptyTerminal();
                    Node* nbreak = mergeNodes(ControlTree::Type::Break, { term });
                    addLink(node, nbreak);
                    addLink(nbreak, to);
                }
            }
            return VisitResult::None;
        });
    }
}

void FlowGraph::analyze(const Options& opt)
{
    /* If empty -> exit */
    if (nodes_.empty())
	{
        return;
    }

    /* Erase NOP entries */
    cleanupNops();

    /* Detect loops */
    /* For each loop: detect break/continue */
    rerouteBreakContinue();

    logSelf(opt);

    /* While changed: try match */
    analyzeImpl(opt);

    /* Assert number of nodes == 1 */
    if (nodes_.size() > 1)
	{
        std::cout << "  Failed to simplify graph: " << std::setw(4) << nodes_.size() << " nodes left!\n";
    }
}

void FlowGraph::analyzeImpl(const Options& opt)
{
    for (bool changed = true; changed;)
	{
        /* Walk graph in reverse-depth-first order */
        std::vector<Node*> dftree;
        depthFirstWalk(entry_, [&dftree](Node * node) mutable
		{
            dftree.push_back(node);
            return VisitResult::None;
        });
        std::reverse(dftree);

        /* Match patterns */
        changed = false;
        for (Node* node : dftree)
		{
            if (matchedBlock(node) ||
                matchedAnd(node) ||
                matchedOr(node) ||
                matchedSwitch(node) ||
                matchedIf(node) ||
                matchedIfElse(node) ||
                matchedRepeat(node) ||
                matchedLoop(node) ||
                matchedNaturalLoop(node))
			{
                logSelf(opt);
                changed = true;
                break;
            }
        }
    }
}

FlowGraph::Node::Node(int addr)
    : inputs()
    , outputs()
    , tree(std::make_unique<ControlTree>())
    , addr(addr)
    , pastTheEndAddr(-1)
{}

FlowGraph::Node::Node(ControlTree::ptr_t ct)
    : inputs()
    , outputs()
    , tree(std::move(ct))
    , addr(tree->addr())
    , pastTheEndAddr(tree->pastTheEndAddr())
{}

int FlowGraph::Node::outputsCount() const
{
    return outputs.size();
}

int FlowGraph::Node::inputsCount() const
{
    return inputs.size();
}

bool FlowGraph::Node::isNumber(int v) const
{
    return tree->isNumber(v);
}

bool FlowGraph::Node::isExpression() const
{
    return tree->isNumber();
}

bool FlowGraph::Node::isSwitchHeader() const
{
    return outputs.size() == 2
           && tree->isSwitchHeader();
}

bool FlowGraph::Node::isSwitchCase() const
{
    return outputs.size() == 2
           && tree->isSwitchCase();
}

bool FlowGraph::Node::isSwitchFinalizer() const
{
    return outputs.size() <= 1
           && tree->isSwitchFinalizer();
}

bool FlowGraph::Node::isNop() const
{
    return tree->isNop();
}

FlowGraph::Node* FlowGraph::Node::output(int i)
{
    return outputs.at(i);
}

FlowGraph::Node* FlowGraph::Node::firstOutput()
{
    return outputs.front();
}

FlowGraph::Node* FlowGraph::Node::lastOutput()
{
    return outputs.back();
}


ControlTree* FlowGraph::controlTree()
{
    return entry_->tree.get();
}

bool FlowGraph::matchedBlock(Node* n)
{
    if (n->outputsCount() != 1 ||
        n->output()->inputsCount() != 1 ||
        linkIsUp(n, n->output()))
	{
        return false;
    }
    Node* s = n->output();
    if (s->isSwitchFinalizer())
	{
        return false;
    }
    mergeNodes(ControlTree::Type::LinearBlock, { n, s });
    return true;
}

bool FlowGraph::matchedLoop(Node* n)
{
    if (n->outputsCount() != 2) {
        return false;
    }
    Node* body = n->firstOutput();
    if (body->outputsCount() != 1 ||
        body->output() != n ||
        body->inputsCount() != 1)
	{
        return false;
    }
    removeLink(body, n);
    mergeNodes(ControlTree::Type::LoopWithHeader, { n, body });
    return true;
}

bool FlowGraph::matchedNaturalLoop(Node* n)
{
    if (n->outputsCount() != 2 ||
        n->lastOutput() != n)
	{
        return false;
    }
    removeLink(n, n);
    mergeNodes(ControlTree::Type::NaturalLoop, { n });
    return true;
}

bool FlowGraph::matchedRepeat(Node* n)
{
    if (n->outputsCount() != 2)
	{
        return false;
    }

    Node* body = n->lastOutput();
    if (body->outputsCount() != 2 ||
        body->firstOutput() != body ||
        body->lastOutput() != n->firstOutput())
	{
        return false;
    }

    removeLink(body, body);
    mergeNodes(ControlTree::Type::RepeatLoop, { n, body, body->output() });

    return true;
}

bool FlowGraph::matchedIf(Node* n)
{
    if (n->outputsCount() != 2)
	{
        return false;
    }
    Node* brTrue = n->firstOutput();
    Node* pastTheEnd = n->lastOutput();
    if (n->isSwitchCase() ||
        brTrue->isExpression() ||
        brTrue->outputsCount() != 1 ||
        brTrue->inputsCount() != 1 ||
        brTrue->output() != pastTheEnd)
	{
        return false;
    }
    mergeNodes(ControlTree::Type::If, { n, brTrue });
    return true;
}

bool FlowGraph::matchedIfElse(Node* n)
{
    if (n->outputsCount() != 2)
	{
        return false;
    }
    Node* brTrue = n->firstOutput();
    Node* brFalse = n->lastOutput();
    if (n->isSwitchCase() ||
        brTrue->isExpression() ||
        brFalse->isExpression() ||
        brTrue->outputsCount() != 1 ||
        brTrue->inputsCount() != 1 ||
        brFalse->outputsCount() != 1 ||
        brFalse->inputsCount() != 1 ||
        brTrue->output() != brFalse->output())
	{
        return false;
    }
    mergeNodes(ControlTree::Type::IfElse, { n, brTrue, brFalse });
    return true;
}

bool FlowGraph::matchedAnd(Node* n)
{
    if (n->outputsCount() != 2)
	{
        return false;
    }

    Node* br_b = n->firstOutput();
    Node* br_zero = n->lastOutput();

    if (!br_zero->isNumber(0) ||
        br_zero->outputsCount() != 1 ||
        br_b->inputsCount() != 1 ||
        br_b->outputsCount() != 1 ||
        br_zero->output() != br_b->output())
	{
        return false;
    }

    removeLink(n, br_zero);
    if (br_zero->inputsCount() == 0)
	{
        eraseNode(br_zero);
    }

    mergeNodes(ControlTree::Type::And, { n, br_b });

    return true;
}

bool FlowGraph::matchedOr(Node* n)
{
    if (n->outputsCount() != 2)
	{
        return false;
    }

    Node* br_one = n->firstOutput();
    Node* br_b = n->lastOutput();

    if (!br_one->isNumber(1) ||
        br_one->outputsCount() != 1 ||
        br_b->inputsCount() != 1 ||
        br_b->outputsCount() != 1 ||
        br_one->output() != br_b->output())
	{
        return false;
    }

    removeLink(n, br_one);
    if (br_one->inputsCount() == 0)
	{
        eraseNode(br_one);
    }

    mergeNodes(ControlTree::Type::Or, { n, br_b });

    return true;
}

bool FlowGraph::matchedSwitch(Node* n)
{
    /* 1. Check header */
    if (!n->isSwitchHeader())
	{
        return false;
    }

    /* 2. Find cases */
    std::vector<Node*> checks;
    Node* check = n;
    for (; check->isSwitchCase(); check = check->lastOutput())
	{
        checks.push_back(check);
    }

    /* 3. Check default case */
    Node* def_check = check;
    if (def_check->outputsCount() != 1 ||
        def_check->inputsCount() != 1)
	{
        return false;
    }

    /* 4. Check finalizer */
    Node* def_action = def_check->output();
    Node* finalizer;
    if (def_action->isSwitchFinalizer())
	{
        finalizer = def_action;
        def_action = nullptr;
    }
	else
	{
        if (def_action->outputsCount() != 1)
		{
            return false;
        }
        finalizer = def_action->output();
    }
    if (!finalizer->isSwitchFinalizer())
	{
        return false;
    }

    /* 5. Find actions */
    std::vector<Node*> actions(checks.size(), nullptr);
    std::vector<char> has_break(checks.size(), 0);
    for (long i = checks.size() - 1; i >= 0; --i)
	{
        Node* next_action = i < static_cast<long>(checks.size()) - 1 ? actions[i + 1] : def_action;
        actions[i] = checks[i]->firstOutput();
        if (actions[i] == finalizer)
		{
            actions[i] = nullptr;
            has_break[i] = !!next_action;
        }
		else if (actions[i] != next_action)
		{
            has_break[i] = actions[i]->output() == finalizer && next_action;
        }
    }
    for (long i = 0; i < static_cast<long>(checks.size()) - 1; ++i)
	{
        if (actions[i] == actions[i + 1])
		{
            actions[i] = nullptr;
        }
    }

    /* 6. Merge nodes */
    std::vector<Node*> cases;
    cases.push_back(mergeNodes(ControlTree::Type::SwitchFinalizer, { finalizer }));
    if (def_action)
	{
        cases.push_back(mergeNodes(ControlTree::Type::SwitchDefaultAction, { def_action }));
        if (def_check != def_action)
		{
            eraseNode(def_check);
        }
    }
	else
	{
        cases.push_back(mergeNodes(ControlTree::Type::SwitchDefaultEmpty, { def_check }));
    }
    for (long i = checks.size() - 1; i >= 0; --i)
	{
        if (has_break[i])
		{
            cases.push_back(mergeNodes(ControlTree::Type::SwitchCaseBreak, { checks[i], actions[i] }));
        }
		else if (actions[i])
		{
            cases.push_back(mergeNodes(ControlTree::Type::SwitchCaseFallthrough, { checks[i], actions[i] }));
        }
		else
		{
            cases.push_back(mergeNodes(ControlTree::Type::SwitchCaseFallthrough, { checks[i] }));
        }
    }
    std::reverse(cases);
    mergeNodes(ControlTree::Type::Switch, cases);
    return true;
}

bool FlowGraph::addLink(Node* a, Node* b)
{
    if (!hasLink(a, b))
	{
        a->outputs.push_back(b);
        b->inputs.push_back(a);
        return true;
    }
    return false;
}

FlowGraph::Node* FlowGraph::fallthroughNode(Node* n)
{
    auto it = std::find_if(nodes_, [n](auto & ptr)
	{
        return ptr->addr == n->pastTheEndAddr;
    });
    if (it == nodes_.end())
	{
        return nullptr;
    }
    return it->get();
}

FlowGraph::Node* FlowGraph::createTerminal(program_t& p)
{
    BaseBlock bb(p);
    auto ct = std::make_unique<ControlTree>(bb);
    auto pn = std::make_unique<Node>(std::move(ct));
    if (!entry_)
	{
        entry_ = pn.get();
    }
    nodes_.push_back(std::move(pn));
    return nodes_.back().get();
}

FlowGraph::Node* FlowGraph::createEmptyTerminal(int addr)
{
    nodes_.push_back(std::make_unique<Node>(addr));
    Node* ret = nodes_.back().get();
    if (!entry_)
	{
        entry_ = ret;
    }
    return ret;
}

bool FlowGraph::hasLink(Node* a, Node* b)
{
    return std::find(a->outputs, b) != a->outputs.end() &&
           std::find(b->inputs, a) != b->inputs.end();
}

bool FlowGraph::linkIsUp(Node* a, Node* b)
{
    if (a->addr == -1 || b->addr == -1)
	{
        return false;
    }
    return b->addr < a->addr;
}

bool FlowGraph::removeLink(Node* a, Node* b)
{
    if (!hasLink(a, b))
	{
        return false;
    }
    a->outputs.erase(std::find(a->outputs, b));
    b->inputs.erase(std::find(b->inputs, a));
    return true;
}

void replace_with_vector(
    std::vector<FlowGraph::Node*>& dest,
    FlowGraph::Node* val,
    std::vector<FlowGraph::Node*>& src)
{
    auto it = std::find_if(dest, [val](auto ptr)
	{
        return ptr == val;
    });

    if (it == dest.end())
	{
        return;
    }

    size_t pos = std::distance(dest.begin(), it);
    dest.erase(it);

    for (auto* n : src)
	{
        if (std::find_if(dest, [n](auto ptr) { return ptr == n; }) == dest.end())
		{
            dest.insert(dest.begin() + pos++, n);
        }
    }
}

void FlowGraph::eraseNode(Node* n)
{
    if (n == entry_)
	{
        assert(entry_->outputsCount() == 1);
        entry_ = entry_->output();
    }

    auto equals = [n](const std::unique_ptr<Node>& ptr)
	{
        return ptr.get() == n;
    };
    auto it = std::find_if(nodes_, equals);
    assert(it != nodes_.end());

    std::vector<Node*> inputs(n->inputs);
    std::vector<Node*> outputs(n->outputs);

    for (Node* in : inputs)
	{
        replace_with_vector(in->outputs, n, outputs);
    }

    for (Node* out : outputs)
	{
        replace_with_vector(out->inputs, n, inputs);
    }

    n->inputs.clear();
    n->outputs.clear();

    nodes_.erase(it);
}

void FlowGraph::insertBefore(Node* pos, Node* val)
{
    assert(val->inputsCount() == 0 && val->outputsCount() == 0);

    if (pos == entry_)
	{
        entry_ = val;
    }

    for (Node* in : pos->inputs)
	{
        std::replace(in->outputs, pos, val);
        val->inputs.push_back(in);
    }

    pos->inputs.clear();

    addLink(val, pos);
}

void FlowGraph::logSelf(const Options& opt)
{
    if (opt.logSteps)
	{
        std::ofstream tmp(opt.stepLogPrefix + std::to_string(step_++) + ".gml");
        GraphmlWriter(tmp).print(*this);
    }
}

FlowGraph::Node* FlowGraph::mergeNodes(ControlTree::Type t, std::initializer_list<Node*> l)
{
    return mergeNodes_(t, l.begin(), l.end());
}
