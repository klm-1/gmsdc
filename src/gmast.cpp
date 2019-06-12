#include "gmast.h"

#include <stdexcept>

#include "utils.h"


const std::map<std::string, int> BinaryOpPriority{
    { "^", 0 },
    { "^^", 0 },
    { "||", 0 },
    { "&&", 1 },
    { "<", 2 },
    { ">", 2 },
    { "<=", 2 },
    { ">=", 2 },
    { "==", 2 },
    { "!=", 2 },
    { "+", 3 },
    { "-", 3 },
    { "*", 4 },
    { "/", 4 },
    { "%", 4 },
    { "div", 4 },
    { "&", 5 },
    { "|", 5 },
};

const std::map<std::string, int> UnaryOpPriority{
    { "-", 6 },
    { "~", 6 },
    { "!", 6 },
};


GmAST::GmAST()
    : GmAST(GmlPattern::Invalid)
{}

GmAST::GmAST(GmlPattern t)
    : pat_(t)
    , val_string_()
    , links_()
{}

GmAST::GmAST(GmlPattern t, const std::string& data)
    : pat_(t)
    , val_string_(data)
    , links_()
{}

GmAST::GmAST(GmlPattern t, int64_t data)
    : pat_(t)
    , val_int_(data)
    , val_string_(to_string(data))
    , links_()
{}

GmAST::GmAST(GmlPattern t, double data)
    : pat_(t)
    , val_double_(data)
    , val_string_(to_string(data))
    , links_()
{}

GmAST::GmAST(GmlPattern t, std::vector<ptr_t>&& lv)
    : pat_(t)
    , val_string_()
    , links_(std::move(lv))
{
    for (ptr_t& l : links_)
	{
        l->uplink_ = this;
    }
}

std::ostream& operator<< (std::ostream& out, const GmAST& ast)
{
    return out << GmlPattern2String(ast.pattern()) << "(" << ast.dataString() << ")";
}

int GmAST::leavesCount() const
{
    return links_.size();
}

GmAST* GmAST::leftLeaf()
{
    return links_.at(0).get();
}

GmAST* GmAST::rightLeaf()
{
    return links_.at(links_.size() - 1).get();
}

GmAST* GmAST::leaf(int i)
{
    return links_.at(i).get();
}

const GmAST* GmAST::leftLeaf() const
{
    return links_.at(0).get();
}

const GmAST* GmAST::rightLeaf() const
{
    return links_.at(links_.size() - 1).get();
}

const GmAST* GmAST::leaf(int i) const
{
    return links_.at(i).get();
}

GmlPattern GmAST::pattern() const
{
    return pat_;
}

void GmAST::addLeaf(ptr_t l)
{
    addLeaf(std::move(l), links_.size());
}

void GmAST::addLeaf(ptr_t l, size_t p)
{
    if (!l)
	{
        links_.insert(links_.begin() + p, std::move(l));
        return;
    }

    if (l->uplink_)
	{
        throw std::runtime_error("Node can have at most one uplink");
    }

    l->uplink_ = this;
    links_.insert(links_.begin() + p, std::move(l));
}

const std::string& GmAST::dataString() const
{
    return val_string_;
}

const char* GmlPattern2String(GmlPattern p)
{
    switch (p)
	{
		CASE_RETURN_SCOPED(GmlPattern, Invalid)
		CASE_RETURN_SCOPED(GmlPattern, If)
		CASE_RETURN_SCOPED(GmlPattern, IfElse)
		CASE_RETURN_SCOPED(GmlPattern, While)
		CASE_RETURN_SCOPED(GmlPattern, Repeat)
		CASE_RETURN_SCOPED(GmlPattern, Break)
		CASE_RETURN_SCOPED(GmlPattern, Continue)
		CASE_RETURN_SCOPED(GmlPattern, DoUntil)
		CASE_RETURN_SCOPED(GmlPattern, LinearBlock)
		CASE_RETURN_SCOPED(GmlPattern, BinaryOp)
		CASE_RETURN_SCOPED(GmlPattern, UnaryOp)
		CASE_RETURN_SCOPED(GmlPattern, Number)
		CASE_RETURN_SCOPED(GmlPattern, String)
		CASE_RETURN_SCOPED(GmlPattern, Assignment)
		CASE_RETURN_SCOPED(GmlPattern, CompoundAssignment)
		CASE_RETURN_SCOPED(GmlPattern, FunctionCall)
		CASE_RETURN_SCOPED(GmlPattern, Return)
		CASE_RETURN_SCOPED(GmlPattern, Exit)
		CASE_RETURN_SCOPED(GmlPattern, Nop)
		CASE_RETURN_SCOPED(GmlPattern, Variable)
		CASE_RETURN_SCOPED(GmlPattern, VarScope)
		CASE_RETURN_SCOPED(GmlPattern, DroppedExpr)
		CASE_RETURN_SCOPED(GmlPattern, Prefix)
		CASE_RETURN_SCOPED(GmlPattern, Postfix)
		CASE_RETURN_SCOPED(GmlPattern, ArrayElement)
		CASE_RETURN_SCOPED(GmlPattern, ArrayElement2)
		CASE_RETURN_SCOPED(GmlPattern, Switch)
		CASE_RETURN_SCOPED(GmlPattern, SwitchCase)
		CASE_RETURN_SCOPED(GmlPattern, SwitchDefault)
		CASE_RETURN_SCOPED(GmlPattern, With)
    }
    return "???";
}

bool GmlNeedsPadding(GmlPattern p)
{
    switch (p)
	{
        case (GmlPattern::If):
        case (GmlPattern::IfElse):
        case (GmlPattern::While):
        case (GmlPattern::Repeat):
        case (GmlPattern::DoUntil):
        case (GmlPattern::Switch):
        case (GmlPattern::Return):
        case (GmlPattern::With):
            return true;

        case (GmlPattern::BinaryOp):
        case (GmlPattern::UnaryOp):
        case (GmlPattern::Number):
        case (GmlPattern::String):
        case (GmlPattern::FunctionCall):
        case (GmlPattern::Variable):
        case (GmlPattern::ArrayElement):
        case (GmlPattern::ArrayElement2):
        case (GmlPattern::Invalid):
        case (GmlPattern::LinearBlock):
        case (GmlPattern::Nop):
        case (GmlPattern::VarScope):
        case (GmlPattern::SwitchCase):
        case (GmlPattern::SwitchDefault):
        case (GmlPattern::Break):
        case (GmlPattern::Continue):
        case (GmlPattern::Postfix):
        case (GmlPattern::Prefix):
        case (GmlPattern::DroppedExpr):
        case (GmlPattern::Assignment):
        case (GmlPattern::CompoundAssignment):
        case (GmlPattern::Exit):
            return false;
    }

    return false;
}

int64_t GmAST::dataInt() const
{
    return val_int_;
}

double GmAST::dataReal() const
{
    return val_double_;
}

bool GmAST::isNumber() const
{
    if (pat_ == GmlPattern::Number)
	{
        return true;
    }

    if (pat_ == GmlPattern::LinearBlock &&
        links_.size() == 1)
	{
        return links_.front()->isNumber();
    }

    return false;
}

bool GmAST::isNumber(int x) const
{
    if (pat_ == GmlPattern::Number && dataInt() == x)
	{
        return true;
    }

    if (pat_ == GmlPattern::LinearBlock &&
        links_.size() == 1)
	{
        return links_.front()->isNumber(x);
    }

    return false;
}

bool GmAST::isNop() const
{
    if (pat_ == GmlPattern::Nop)
	{
        return true;
    }

    if (leavesCount() == 1)
	{
        const auto& l = links_.front();
        return l ? l->isNop() : false;
    }

    return false;
}

/* Duplicates node and its subtree EXCEPT pointer to uplink */
// NOTE: ret->uplink_ == nullptr
GmAST::ptr_t GmAST::deepcopy() const
{
    ptr_t ret        = make();
    ret->pat_        = pat_;
    ret->val_double_ = val_double_;
    ret->val_int_    = val_int_;
    ret->val_string_ = val_string_;
    ret->uplink_     = nullptr;

    ret->links_.reserve(links_.size());

    for (const ptr_t& l : links_)
	{
        ret->addLeaf(l->deepcopy());
    }

    return std::move(ret);
}

void GmAST::dataString(const std::string& s)
{
    val_string_ = s;
}

void GmAST::dataInt(int64_t x)
{
    val_int_ = x;
    val_string_ = to_string(x);
}

void GmAST::dataReal(double x)
{
    val_double_ = x;
    val_string_ = to_string(x);
}

bool GmAST::deepEquals(const GmAST& other) const
{
    if (*this != other)
	{
        return false;
    }

    if (leavesCount() != other.leavesCount())
	{
        return false;
    }

    for (size_t i = 0; i < links_.size(); ++i)
	{
        if (!links_[i]->deepEquals(*other.links_[i]))
		{
            return false;
        }
    }

    return true;
}

bool GmAST::operator== (const GmAST& other) const
{
    return
        pat_ == other.pat_ &&
        val_double_ == other.val_double_ &&
        val_int_ == other.val_int_ &&
        val_string_ == other.val_string_;
}

bool GmAST::operator!= (const GmAST& other) const
{
    return !(*this == other);
}
