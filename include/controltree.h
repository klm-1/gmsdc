#ifndef CONTROLTREE_H
#define CONTROLTREE_H

#include <memory>
#include <iostream>

#include "baseblock.h"


class ControlTree
{
public:
    using ptr_t = std::unique_ptr<ControlTree>;

    enum class Type 
	{
        Terminal,
        LinearBlock,
        NaturalLoop,
        LoopWithHeader,
        RepeatLoop,
        If,
        IfElse,
        And,
        Or,
        Switch,
        SwitchCaseBreak,
        SwitchCaseFallthrough,
        SwitchDefaultAction,
        SwitchDefaultEmpty,
        SwitchFinalizer,
        Break,
        Continue,
    };

    ControlTree() = default;
    ControlTree(const BaseBlock& bb_);
    ControlTree(Type t);

    bool isNumber(int x) const;
    bool isNumber() const;
    bool isSwitchHeader() const;
    bool isSwitchCase() const;
    bool isSwitchFinalizer() const;
    bool isNop() const;
    int leavesCount() const;
    int addr() const;
    int pastTheEndAddr() const;
    Type type() const;

    void addLeaf(ptr_t l);
    BaseBlock& baseblock();
    ControlTree* leftLeaf();
    ControlTree* rightLeaf();
    ControlTree* leaf(int index = 0);

    friend std::ostream& operator<< (std::ostream& out, const ControlTree& t);
    friend class GraphmlWriter;

private:
    Type type_;
    BaseBlock bb_;
    ControlTree* root_;
    std::vector<ptr_t> leaves_;
};

const char* ControlTreeTypeToString(ControlTree::Type t);

#endif // CONTROLTREE_H
