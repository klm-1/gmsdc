#include "decompiler.h"

#include <fstream>
#include <cassert>

#include "gmform.h"
#include "gmchunk.h"
#include "flowgraph.h"
#include "fsmanager.h"
#include "gmast.h"
#include "asmcommand.h"
#include "utils.h"
#include "graphmlwriter.h"
#include "gmlwriter.h"
#include "asttransformer.h"

const std::map<Operation, std::string> Decompiler::AsmOpToBinary{
    { Operation::Add, "+" },
    { Operation::Sub, "-" },
    { Operation::Mul, "*" },
    { Operation::Div, "/" },
    { Operation::Mod, "%" },
    { Operation::Rem, "div" },
    { Operation::And, "&" },
    { Operation::Xor, "^" },
    { Operation::Or, "|" },
};

const std::map<Operation, std::string> Decompiler::AsmOpToUnary{
    { Operation::Neg, "-" },
    { Operation::Not, "~" },
};

const std::map<Operation, std::string> Decompiler::AsmOpToRelative{
    { Operation::Add, "+=" },
    { Operation::Sub, "-=" },
    { Operation::Mul, "*=" },
    { Operation::Div, "/=" },
    { Operation::Mod, "%=" },
    { Operation::And, "&=" },
    { Operation::Xor, "^=" },
    { Operation::Or, "|=" },
};

const std::map<Comparison, std::string> Decompiler::ComparisonToOperator{
    { Comparison::LT, "<" },
    { Comparison::GT, ">" },
    { Comparison::LE, "<=" },
    { Comparison::GE, ">=" },
    { Comparison::EQ, "==" },
    { Comparison::NE, "!=" },
};


Decompiler::Options Decompiler::Options::Debug()
{
    Options ret;
    ret.outputDir = "out";
    ret.gmlSubDir = ".";
    ret.logSubDir = "log";
    ret.logFlowgraph = true;
    ret.logTree = true;
    ret.logAssembly = true;
    ret.eachScriptInSeparateDir = true;
    ret.decompileAll = true;
    return ret;
}

Decompiler::Options Decompiler::Options::Release()
{
    Options ret;
    ret.outputDir = "out";
    ret.gmlSubDir = ".";
    ret.logSubDir = "log";
    ret.logFlowgraph = false;
    ret.logTree = false;
    ret.logAssembly = false;
    ret.decompileAll = true;
    return ret;
}


Decompiler::Decompiler(GmForm& f)
    : form_(&f)
    , stack_()
    , addr_(nullptr)
    , index_(nullptr)
    , ret_expr_(nullptr)
{}

void Decompiler::decompile(const Options& opt)
{
    std::string gmlsub = opt.outputDir + "/" + opt.gmlSubDir;
    std::string logsub = opt.outputDir + "/" + opt.logSubDir;
    FsManager::directoryCreate(std::wstring(opt.outputDir.begin(), opt.outputDir.end()));
    FsManager::directoryCreate(std::wstring(gmlsub.begin(), gmlsub.end()));
    FsManager::directoryCreate(std::wstring(logsub.begin(), logsub.end()));

    for (ScriptEntry const& src : form_->code())
	{
        auto tgt_it = opt.targets.find(src.name);
        auto ign_it = opt.ignore.find(src.name);
        if ((!opt.decompileAll && tgt_it == opt.targets.end()) || ign_it != opt.ignore.end())
		{
            std::cout << "Skipped: " << src.name << std::endl;
            continue;
        }

        std::string fname = opt.outputDir + "/" + opt.gmlSubDir + "/" + src.name + ".gml";

        std::ofstream out(fname);
        if (!out.good())
		{
            std::cout << "Failed to open " << fname << std::endl;
            continue;
        }

        std::cout << "Processing: " << src.name << std::endl;
        try
        {
            GmAST::ptr_t ptree = decompileScript(src, opt);
            AstTransformer::transform(ptree);
            GmlWriter(out, *form_).print(*ptree);
        }
        catch (std::runtime_error& e)
        {
            std::cout << "  ERROR: " << e.what() << std::endl;
        }
    }
}

GmAST::ptr_t Decompiler::decompileScript(ScriptEntry const& src, const Options& opt)
{
    std::string logPrefix = opt.outputDir + "/" + opt.logSubDir + "/" + src.name;
    if (opt.eachScriptInSeparateDir && (opt.logAssembly || opt.logFlowgraph || opt.logTree))
	{
        FsManager::directoryCreate(std::wstring(logPrefix.begin(), logPrefix.end()));
        logPrefix += "/";
        logPrefix += src.name;
    }

    if (opt.logAssembly)
	{
        std::ofstream(logPrefix + "_dump.gmasm") << src << std::endl;
    }

    FlowGraph g(src.code);

    if (opt.logFlowgraph)
	{
        std::ofstream tmp(logPrefix + "_raw.gml");
        GraphmlWriter(tmp).print(g);
    }

    FlowGraph::Options fgOpt = FlowGraph::Options::Debug();
    fgOpt.stepLogPrefix = logPrefix + "_fold_step_";
    fgOpt.logSteps = opt.logFlowgraph;
    g.analyze(fgOpt);

    if (opt.logFlowgraph)
	{
        std::ofstream tmp(logPrefix + "_format.gml");
        GraphmlWriter(tmp).print(g);
    }

    ControlTree* ct = g.controlTree();

    if (opt.logTree)
	{
        std::ofstream tout(logPrefix + "_ctree.gml");
        GraphmlWriter(tout).print(*ct);
    }

    GmAST::ptr_t ret = analyzeControlTree(ct);

    if (opt.logTree)
	{
        std::ofstream tout(logPrefix + "_ast.gml");
        GraphmlWriter(tout).print(*ret);
    }

    return std::move(ret);
}

GmAST::ptr_t Decompiler::analyzeControlTree(ControlTree* ct)
{
    stack_.clear();
    pushFrame();
    visit(ct, true);
    return std::move(frame().stat_list.front());
}

void Decompiler::visit(ControlTree* ct, bool as_block, bool push_into)
{
    if (as_block)
	{
        pushFrame(push_into ? pop_back(frame().expr_stack) : nullptr);
    }

    switch (ct->type())
	{
        case (ControlTree::Type::Terminal):
            decompileBaseBlock(ct->baseblock());
            break;

        case (ControlTree::Type::And):
            visit(ct->leftLeaf());
            visit(ct->rightLeaf());
            applyBinaryOp("&&");
            break;

        case (ControlTree::Type::Or):
            visit(ct->leftLeaf());
            visit(ct->rightLeaf());
            applyBinaryOp("||");
            break;

        case (ControlTree::Type::If):
            visit(ct->leftLeaf());
            visit(ct->rightLeaf(), true);
            applyTree(GmlPattern::If, 1, 1);
            break;

        case (ControlTree::Type::IfElse):
            visit(ct->leftLeaf());
            visit(ct->leaf(1), true);
            visit(ct->rightLeaf(), true);
            applyTree(GmlPattern::IfElse, 2, 1);
            break;

        case (ControlTree::Type::Break):
            applyTree(GmlPattern::Break);
            break;

        case (ControlTree::Type::Continue):
            applyTree(GmlPattern::Continue);
            break;


        case (ControlTree::Type::LinearBlock):
            visit(ct->leftLeaf());
            visit(ct->rightLeaf());
            break;

        case (ControlTree::Type::LoopWithHeader):
            visit(ct->leftLeaf());
            visit(ct->rightLeaf(), true);
            applyTree(GmlPattern::While, 1, 1);
            break;

        case (ControlTree::Type::NaturalLoop):
            visit(ct->leaf(), true);
            applyTree(GmlPattern::DoUntil, 1, 1);
            break;

        case (ControlTree::Type::RepeatLoop):
			{
                visit(ct->leftLeaf());
                GmAST::ptr_t lim = pop_back(frame().expr_stack);
                visit(ct->leaf(1), true, true);
                frame().expr_stack.pop_back();
                frame().expr_stack.push_back(lim->rightLeaf()->deepcopy());
                applyTree(GmlPattern::Repeat, 1, 1);
                visit(ct->rightLeaf());
            }
            break;

        case (ControlTree::Type::Switch):
			{
                visit(ct->leftLeaf());
                int no_default = 0;
                GmAST::ptr_t var = (frame().expr_stack.back())->deepcopy();
                for (int i = 1; i < ct->leavesCount() - 1; ++i)
				{
                    visit(ct->leaf(i));
                    no_default |= ct->leaf(i)->type() == ControlTree::Type::SwitchDefaultEmpty;
                }
                frame().expr_stack.push_back(std::move(var));
                applyTree(GmlPattern::Switch, ct->leavesCount() - no_default - 1, 1);
                visit(ct->rightLeaf());
            }
            break;

        case (ControlTree::Type::SwitchCaseBreak):
        case (ControlTree::Type::SwitchCaseFallthrough):
			{
                visit(ct->leftLeaf());
                GmAST::ptr_t cst = frame().expr_stack.back()->leftLeaf()->deepcopy();
                frame().expr_stack.pop_back();
                if (ct->leavesCount() > 1)
				{
                    visit(ct->rightLeaf(), true);
                }
				else
				{
                    applyTree(GmlPattern::LinearBlock);
                }
                if (ct->type() == ControlTree::Type::SwitchCaseBreak)
				{
                    applyTree(GmlPattern::Break);
                    applyTree(GmlPattern::LinearBlock, 2);
                }
                frame().expr_stack.push_back(std::move(cst));
                applyTree(GmlPattern::SwitchCase, 1, 1);
            }
            break;

        case (ControlTree::Type::SwitchDefaultAction):
            visit(ct->rightLeaf(), true);
            applyTree(GmlPattern::SwitchDefault, 1);
            break;

        case (ControlTree::Type::SwitchDefaultEmpty):
            break;

        case (ControlTree::Type::SwitchFinalizer):
            visit(ct->leaf());
            break;
    }

    if (as_block)
	{
        popFrame();
    }
}

void Decompiler::decompileBaseBlock(const BaseBlock& bb)
{
    for (const AsmCommand& cmd : bb)
	{
        applyCommand(cmd);
    }
}

void Decompiler::applyCommand(const AsmCommand& cmd)
{
    switch (cmd.operation())
	{
        case (Operation::Add):
        case (Operation::Sub):
        case (Operation::Mul):
        case (Operation::Div):
        case (Operation::Mod):
        case (Operation::And):
        case (Operation::Shl):
        case (Operation::Shr):
        case (Operation::Xor):
        case (Operation::Rem):
        case (Operation::Or):
            applyBinaryOp(cmd.operation());
            break;

        case (Operation::Neg):
        case (Operation::Not):
            applyUnaryOp(cmd);
            break;

        case (Operation::Call):
            applyCall(cmd);
            break;

        case (Operation::Cmp):
            applyCompare(cmd.cmpType());
            break;

        case (Operation::Exit):
            applyTree(GmlPattern::Exit);
            break;

        case (Operation::Pop):
            if (!ret_expr_)
			{
                if (!frame().expr_stack.empty())
				{
                    GmAST::ptr_t t = GmAST::make(GmlPattern::DroppedExpr);
                    t->addLeaf(pop_back(frame().expr_stack));
                    frame().stat_list.push_back(std::move(t));
                }
            }
            break;

        case (Operation::PushCst):
        case (Operation::PushGlb):
        case (Operation::PushI16):
        case (Operation::PushLoc):
        case (Operation::PushVar):
            applyPush(cmd);
            break;

        case (Operation::Ret):
            applyReturn();
            break;

        case (Operation::Set):
            applyAssignment(cmd);
            break;

        case (Operation::Save):
            applySaveAddr(cmd);
            break;

        case (Operation::Dup):
            applyDuplicate(cmd);
            break;

        case (Operation::PushEnv):
            frame().env_stack.push_back(frame().stat_list.size());
            break;

        case (Operation::PopEnv):
            if (!ret_expr_)
			{
                if (!frame().env_stack.empty())
				{
                    applyTree(GmlPattern::LinearBlock, frame().stat_list.size() - frame().env_stack.back());
                    frame().env_stack.pop_back();
                    applyTree(GmlPattern::With, 1, 1);
                }
            }
            break;

        case (Operation::Break):
        case (Operation::Conv):
        case (Operation::Jmp):
        case (Operation::JNZ):
        case (Operation::JZ):
            /* Do nothing */
            break;

        case (Operation::Nop):
            throw std::runtime_error("Unknown opcode found");
            break;
    }
}

void Decompiler::applyBinaryOp(std::string op)
{
    GmAST::ptr_t t = GmAST::make(GmlPattern::BinaryOp, op);
    t->addLeaf(pop_back(frame().expr_stack));
    t->addLeaf(pop_back(frame().expr_stack));
    frame().expr_stack.push_back(std::move(t));
}

void Decompiler::applyBinaryOp(Operation op)
{
    applyBinaryOp(lookup(AsmOpToBinary, op));
}

void Decompiler::applyUnaryOp(const AsmCommand& cmd)
{
    std::string op = lookup(AsmOpToUnary, cmd.operation());
    if (cmd.operation() == Operation::Not && cmd.dataType() == DataType::Bool)
	{
        op = "!";
    }
    GmAST::ptr_t t = GmAST::make(GmlPattern::UnaryOp, op);
    t->addLeaf(pop_back(frame().expr_stack));
    frame().expr_stack.push_back(std::move(t));
}

void Decompiler::applyCompare(Comparison cmp)
{
    GmAST::ptr_t t = GmAST::make(GmlPattern::BinaryOp, lookup(ComparisonToOperator, cmp));
    t->addLeaf(pop_back(frame().expr_stack));
    t->addLeaf(pop_back(frame().expr_stack));
    frame().expr_stack.push_back(std::move(t));
}

void Decompiler::applyTree(GmlPattern pat, int statc, int exprc)
{
    assert(static_cast<int>(frame().expr_stack.size()) >= exprc);
    assert(static_cast<int>(frame().stat_list.size()) >= statc);

    GmAST::ptr_t t = GmAST::make(pat);
    for (int i = 0; i < statc; ++i)
	{
        t->addLeaf(pop_back(frame().stat_list));
    }
    for (int i = 0; i < exprc; ++i)
	{
        t->addLeaf(pop_back(frame().expr_stack));
    }
    frame().stat_list.push_back(std::move(t));
}

void Decompiler::applyCall(const AsmCommand& cmd)
{
    GmAST::ptr_t t = GmAST::make(GmlPattern::FunctionCall, cmd.symbol);
    for (int i = 0; i < cmd.dataInt16(); ++i)
	{
        t->addLeaf(pop_back(frame().expr_stack));
    }
    frame().expr_stack.push_back(std::move(t));
}

void Decompiler::applyPush(const AsmCommand& cmd)
{
    switch (cmd.dataType())
	{
        case (DataType::Int16):
            frame().expr_stack.push_back(GmAST::make(GmlPattern::Number, static_cast<int64_t>(cmd.dataInt16())));
            break;

        case (DataType::Int32):
            frame().expr_stack.push_back(GmAST::make(GmlPattern::Number, static_cast<int64_t>(cmd.dataInt32())));
            break;

        case (DataType::Int64):
            frame().expr_stack.push_back(GmAST::make(GmlPattern::Number, cmd.dataInt64()));
            break;

        case (DataType::Float):
            frame().expr_stack.push_back(GmAST::make(GmlPattern::Number, static_cast<double>(cmd.dataFloat())));
            break;

        case (DataType::Double):
            frame().expr_stack.push_back(GmAST::make(GmlPattern::Number, cmd.dataDouble()));
            break;

        case (DataType::String):
            frame().expr_stack.push_back(GmAST::make(GmlPattern::String, form_->strings().get(cmd.dataInt32()).data()));
            break;

        case (DataType::Variable):
            frame().expr_stack.push_back(popVariable(cmd));
            break;

        case (DataType::Bool):
        case (DataType::Instance):
            // Seems this never happens
            frame().expr_stack.push_back(GmAST::make(GmlPattern::Invalid, "@push " + DataType2PrettyString(cmd.dataType())));
            break;
    }
}

void Decompiler::applySaveAddr(const AsmCommand& cmd)
{
    assert(frame().expr_stack.size() > 2);
    assert(!addr_);

    switch (static_cast<SaveState>(cmd.dataInt16()))
	{
        case (SaveState::Addr):
			{
                auto it = frame().expr_stack.rbegin() + 2;
                addr_ = std::move(*it);
                frame().expr_stack.erase(--it.base());
            }
            break;

        case (SaveState::Index):
			{
                assert(frame().expr_stack.size() > 3);
                assert(!index_);
                auto ii = frame().expr_stack.rbegin() + 2;
                auto ia = frame().expr_stack.rbegin() + 3;
                index_ = std::move(*ii);
                addr_ = std::move(*ia);
                frame().expr_stack.erase(--ii.base());
                frame().expr_stack.erase(--ia.base());
            }
            break;
    }
}

void Decompiler::applyDuplicate(const AsmCommand& cmd)
{
    assert(!frame().expr_stack.empty());
    GmAST::ptr_t val = frame().expr_stack.back()->deepcopy();

    if (cmd.dataInt16() || cmd.dataType() == DataType::Int64)
	{
        assert(frame().expr_stack.size() > 1);
        frame().expr_stack.push_back((*++frame().expr_stack.rbegin())->deepcopy());
    }

    frame().expr_stack.push_back(std::move(val));
}

bool Decompiler::matchIncrement()
{
    if (frame().expr_stack.empty() || frame().stat_list.empty())
	{
        return false;
    }

    GmAST* asn = frame().stat_list.back().get();
    GmAST* expr = frame().expr_stack.back().get();
    if (asn->pattern() != GmlPattern::Assignment ||
        asn->rightLeaf()->pattern() != GmlPattern::BinaryOp)
	{
        return false;
    }

    GmAST* rvalue = asn->rightLeaf();
    GmAST* lhs = rvalue->rightLeaf();
    GmAST* rhs = rvalue->leftLeaf();

    if (!(rvalue->dataString() == "+" || rvalue->dataString() == "-") ||
        !rhs->isNumber(1))
	{
        return false;
    }

    if (rvalue->deepEquals(*expr))
	{
        GmAST::ptr_t pref = GmAST::make(GmlPattern::Prefix, rvalue->dataString() + rvalue->dataString());
        pref->addLeaf(asn->leftLeaf()->deepcopy());
        frame().expr_stack.pop_back();
        frame().stat_list.pop_back();
        frame().expr_stack.push_back(std::move(pref));
        return true;
    }

    if (lhs->deepEquals(*expr))
	{
        GmAST::ptr_t post = GmAST::make(GmlPattern::Postfix, rvalue->dataString() + rvalue->dataString());
        post->addLeaf(expr->deepcopy());
        frame().stat_list.pop_back();
        frame().expr_stack.pop_back();
        frame().expr_stack.push_back(std::move(post));
        return true;
    }

    return false;
}

bool Decompiler::matchSwap()
{
    GmAST* asn = frame().stat_list.back().get();
    if (asn->pattern() != GmlPattern::Assignment ||
        asn->leftLeaf()->dataString() != "$$$$temp$$$$")
	{
        return false;
    }
    ret_expr_ = asn->rightLeaf()->deepcopy();
    frame().stat_list.pop_back();
    return true;
}

void Decompiler::applyAssignment(const AsmCommand& cmd)
{
    GmAST::ptr_t lvalue = nullptr;
    GmAST::ptr_t rvalue = nullptr;
    if (cmd.dataType() == DataType::Variable)
	{
        lvalue = popVariable(cmd);
        rvalue = pop_back(frame().expr_stack);
    }
	else
	{
        rvalue = pop_back(frame().expr_stack);
        lvalue = popVariable(cmd);
    }

    GmAST::ptr_t ret = GmAST::make(GmlPattern::Assignment, cmd.symbol);
    ret->addLeaf(std::move(lvalue));
    ret->addLeaf(std::move(rvalue));
    frame().stat_list.push_back(std::move(ret));

    matchSwap() || matchIncrement();
}

void Decompiler::applyReturn()
{
    GmAST::ptr_t var = pop_back(frame().expr_stack);

    if (ret_expr_)
	{
        var = std::move(ret_expr_);
    }

    GmAST::ptr_t ret = GmAST::make(GmlPattern::Return);
    ret->addLeaf(std::move(var));
    frame().stat_list.push_back(std::move(ret));
}

GmAST::ptr_t Decompiler::popVariable(const AsmCommand& cmd)
{
    ScopedVariable var = cmd.variable();
    GmlPattern pat = var.type == VariableType::Array ? GmlPattern::ArrayElement : GmlPattern::Variable;

    GmAST::ptr_t arrIndex = nullptr;
    if (var.type == VariableType::Array)
	{
        if (index_)
		{
            arrIndex = std::move(index_);
        }
		else
		{
            arrIndex = pop_back(frame().expr_stack);
        }
    }

    GmAST::ptr_t varTree = GmAST::make(pat, cmd.symbol);
    GmAST::ptr_t varScope = GmAST::make(GmlPattern::VarScope, static_cast<int64_t>(var.scope));

    if (arrIndex)
	{
        varTree->addLeaf(std::move(arrIndex));
    }

    if (var.scope == InstanceType::StackTopOrGlobal && var.type != VariableType::Normal)
	{
        if (addr_)
		{
            varScope->addLeaf(std::move(addr_));
        }
		else
		{
            varScope->addLeaf(pop_back(frame().expr_stack));
        }
    }

    if (var.type == VariableType::RoomScope)
	{
        varScope->dataInt(varScope->dataInt() + 100000);
    }

    varTree->addLeaf(std::move(varScope));
    return std::move(varTree);
}

Decompiler::Frame& Decompiler::frame()
{
    return stack_.back();
}

void Decompiler::pushFrame(GmAST::ptr_t in)
{
    stack_.emplace_back();
    if (in)
	{
        frame().expr_stack.push_back(std::move(in));
    }
}

void Decompiler::popFrame()
{
    ASSERT(!stack_.empty());
    ASSERT(frame().env_stack.empty());

    reverse(frame().stat_list);
    GmAST::ptr_t t = GmAST::make(GmlPattern::LinearBlock, std::move(frame().stat_list));
    std::move(frame().expr_stack, std::back_inserter((*++stack_.rbegin()).expr_stack));
    stack_.pop_back();
    frame().stat_list.push_back(std::move(t));
}
