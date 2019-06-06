#include "controltree.h"

#include <sstream>
#include <string>


ControlTree::ControlTree(const BaseBlock& bb_)
    : type_(Type::Terminal)
    , bb_(bb_)
    , root_()
    , leaves_()
{}

ControlTree::ControlTree(ControlTree::Type t)
    : type_(t)
    , bb_()
    , root_()
    , leaves_()
{}

int ControlTree::leavesCount() const
{
    return leaves_.size();
}

int ControlTree::addr() const
{
    if (type_ == Type::Terminal) 
	{
        if (!bb_.valid()) 
		{
            return -1;
        }
        return bb_.addr();
    }
    return leaves_.front()->addr();
}

int ControlTree::pastTheEndAddr() const
{
    if (type_ == Type::Terminal) 
	{
        if (!bb_.valid()) 
		{
            return -1;
        }
        return bb_.pastTheEndAddr();
    }
    return leaves_.back()->pastTheEndAddr();
}

bool ControlTree::isNumber(int x) const
{
    return type_ == Type::Terminal
           && bb_.isNumber(x);
}

bool ControlTree::isNumber() const
{
    return type_ == Type::Terminal
           && bb_.isNumber();
}

bool ControlTree::isSwitchHeader() const
{
    if (type_ == Type::Terminal) 
	{
        return bb_.endsAsSwitchHeader();
    }
    if (leaves_.size() > 1) 
	{
        return leaves_.back()->isSwitchCase();
    }
    return leaves_.back()->isSwitchHeader();
}

bool ControlTree::isSwitchCase() const
{
    if (type_ == Type::Terminal) 
	{
        return bb_.endsAsSwitchCase();
    }
    if (leaves_.empty()) 
	{
        return false;
    }
    return leaves_.back()->isSwitchCase();
}

bool ControlTree::isSwitchFinalizer() const
{
    if (type_ == Type::Terminal) 
	{
        return bb_.startsWithPop() && !bb_.endsWithExit();
    }
    return leaves_.front()->isSwitchFinalizer();
}

bool ControlTree::isNop() const
{
    return type_ == Type::Terminal
           && bb_.isNop();
}

void ControlTree::addLeaf(ptr_t l)
{
    leaves_.push_back(std::move(l));
}

BaseBlock& ControlTree::baseblock()
{
    return bb_;
}

ControlTree* ControlTree::leftLeaf()
{
    return leaves_.front().get();
}

ControlTree* ControlTree::rightLeaf()
{
    return leaves_.back().get();
}

ControlTree* ControlTree::leaf(int index)
{
    return leaves_.at(index).get();
}

ControlTree::Type ControlTree::type() const
{
    return type_;
}

const char* ControlTreeTypeToString(ControlTree::Type t)
{
    switch (t) 
	{
		CASE_RETURN_SCOPED(ControlTree::Type, Terminal)
		CASE_RETURN_SCOPED(ControlTree::Type, LinearBlock)
		CASE_RETURN_SCOPED(ControlTree::Type, NaturalLoop)
		CASE_RETURN_SCOPED(ControlTree::Type, LoopWithHeader)
		CASE_RETURN_SCOPED(ControlTree::Type, RepeatLoop)
		CASE_RETURN_SCOPED(ControlTree::Type, If)
		CASE_RETURN_SCOPED(ControlTree::Type, IfElse)
		CASE_RETURN_SCOPED(ControlTree::Type, And)
		CASE_RETURN_SCOPED(ControlTree::Type, Or)
		CASE_RETURN_SCOPED(ControlTree::Type, Switch)
		CASE_RETURN_SCOPED(ControlTree::Type, SwitchCaseBreak)
		CASE_RETURN_SCOPED(ControlTree::Type, SwitchCaseFallthrough)
		CASE_RETURN_SCOPED(ControlTree::Type, SwitchDefaultAction)
		CASE_RETURN_SCOPED(ControlTree::Type, SwitchDefaultEmpty)
		CASE_RETURN_SCOPED(ControlTree::Type, SwitchFinalizer)
		CASE_RETURN_SCOPED(ControlTree::Type, Break)
		CASE_RETURN_SCOPED(ControlTree::Type, Continue)
    }
    return "<Type>";
}

std::ostream& operator<< (std::ostream& out, const ControlTree& t)
{
    static int depth = 0;

    std::string pad;
    for (int i = 0; i < depth; ++i) 
	{
        pad += "    ";
    }

    out << pad << ControlTreeTypeToString(t.type_) << " {" << std::endl;

    ++depth;
    for (const auto& ptr : t.leaves_) 
	{
        out << *ptr << std::endl;
    }
    if (t.type_ == ControlTree::Type::Terminal) 
	{
        std::string pad2(pad);
        pad2 += "    ";
        std::string bs;
        {
            std::ostringstream os;
            for (auto& cmd : t.bb_) 
			{
                os << pad2 << cmd.toString() << std::endl;
            }
            bs = os.str();
        }
        out << bs;
    }
    --depth;

    return out << pad << "}";
}
