#ifndef FLOWGRAPH_H
#define FLOWGRAPH_H

#include <vector>
#include <list>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <iostream>
#include <memory>

#include "asmcommand.h"
#include "controltree.h"


class FlowGraph
{
    using program_t = std::vector<AsmCommand>;

public:
    enum class VisitResult
	{
        None, Break, Continue
    };

    struct Options
	{
        std::string stepLogPrefix;
        bool logSteps;

        static Options Debug();
        static Options Release();
    };

    struct Node
	{
        typedef std::unique_ptr<Node> ptr_t;

        std::vector<Node*> inputs, outputs;
        ControlTree::ptr_t tree;
        const int addr;
        const int pastTheEndAddr;

        Node() = default;
        Node(int addr);
        Node(ControlTree::ptr_t ct);

        Node* output(int i = 0);
        Node* firstOutput();
        Node* lastOutput();
        int outputsCount() const;
        int inputsCount() const;
        bool isNumber(int v) const;
        bool isExpression() const;
        bool isSwitchHeader() const;
        bool isSwitchCase() const;
        bool isSwitchFinalizer() const;
        bool isNop() const;
    };

    Options options;

    FlowGraph();
    FlowGraph(const program_t& cv);

    void analyze();
    ControlTree* controlTree();

    friend class GraphmlWriter;

private:
    Node* entry_;
    std::vector<Node::ptr_t> nodes_;

    int step_;
    void logSelf();

    bool matchedBlock(Node* n);
    bool matchedLoop(Node* n);
    bool matchedNaturalLoop(Node* n);
    bool matchedRepeat(Node* n);
    bool matchedIf(Node* n);
    bool matchedIfElse(Node* n);
    bool matchedAnd(Node* n);
    bool matchedOr(Node* n);
    bool matchedSwitch(Node* n);

    void cleanupNops();
    void rerouteBreakContinue();
    void analyzeImpl();
    void eraseNode(Node* n);
    void insertBefore(Node* pos, Node* val);
    Node* fallthroughNode(Node* n);
    Node* createEmptyTerminal(int addr = -1);
    Node* createTerminal(program_t& p);

    static bool addLink(Node* a, Node* b);
    static bool hasLink(Node* a, Node* b);
    static bool linkIsUp(Node* a, Node* b);
    static bool removeLink(Node* a, Node* b);

    template< class Fun >
    void depthFirstWalk(Node* entry, Fun visitor)
    {
        std::stack<Node*> store;
        std::set<Node*> visited;
        store.push(entry);
        visited.insert(entry);

        while (!store.empty())
		{
            Node* node = store.top();
            store.pop();

            VisitResult result = visitor(node);
            if (result == VisitResult::Continue) { continue; }
            if (result == VisitResult::Break) { break; }

            for (Node* l : node->outputs)
			{
                if (visited.insert(l).second)
				{
                    store.push(l);
                }
            }
        }
    }

    Node* mergeNodes(ControlTree::Type t, std::initializer_list<Node*> l);

    template< class V >
    Node* mergeNodes(ControlTree::Type t, V&& v)
    {
        return mergeNodes_(t, std::begin(v), std::end(v));
    }

    template< class It >
    Node* mergeNodes_(ControlTree::Type t, It first, It last)
    {
        auto ct = std::make_unique<ControlTree>(t);
        for (It it = first; it != last; ++it)
		{
            ct->addLeaf(std::move((*it)->tree));
        }

        auto newHdr = std::make_unique<Node>(std::move(ct));
        insertBefore(*first, newHdr.get());

        for (It it = first; it != last; ++it)
		{
            eraseNode(*it);
        }

        nodes_.push_back(std::move(newHdr));
        return nodes_.back().get();
    }
};

#endif // FLOWGRAPH_H
