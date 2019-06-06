#include "asttransformer.h"


void AstTransformer::transform(GmAST::ptr_t& ast)
{
    if (!ast) { return; }

    for (GmAST::ptr_t& l : ast->links_) 
	{
        transform(l);
    }
    auto& v = ast->links_;
    v.erase(std::remove(v.begin(), v.end(), nullptr), v.end());

    matchArray2d(ast);
    matchCompoundAssignment(ast);
}

void AstTransformer::matchArray2d(GmAST::ptr_t& ast)
{
    if (!ast || ast->pattern() != GmlPattern::ArrayElement) 
	{
        return;
    }

    GmAST* index = ast->leftLeaf();
    if (index->pattern() != GmlPattern::BinaryOp || index->dataString() != "+") 
	{
        return;
    }

    GmAST* mul = index->rightLeaf();
    if (mul->pattern() != GmlPattern::BinaryOp || mul->dataString() != "*") 
	{
        return;
    }

    GmAST* k32 = mul->leftLeaf();
    if (!k32->isNumber(32000)) 
	{
        return;
    }

    GmAST::ptr_t i = mul->rightLeaf()->deepcopy();
    GmAST::ptr_t j = index->leftLeaf()->deepcopy();

    ast->links_.erase(ast->links_.begin());
    ast->addLeaf(std::move(i), 0);
    ast->addLeaf(std::move(j), 1);
    ast->pat_ = GmlPattern::ArrayElement2;
}

void AstTransformer::matchCompoundAssignment(GmAST::ptr_t& ast)
{
    if (!ast || ast->pattern() != GmlPattern::Assignment) 
	{
        return;
    }

    GmAST* lvalue = ast->leftLeaf();
    GmAST* rvalue = ast->rightLeaf();
    if (rvalue->pattern() != GmlPattern::BinaryOp ||
		rvalue->dataString() == "div") 
	{
        return;
    }

    GmAST* rhs = rvalue->leftLeaf();
    GmAST* lhs = rvalue->rightLeaf();
    if (!lhs->deepEquals(*lvalue)) 
	{
        return;
    }

    ast->dataString(rvalue->dataString() + "=");
    ast->pat_ = GmlPattern::CompoundAssignment;

    auto inc = rhs->deepcopy();

    ast->links_.pop_back();
    ast->addLeaf(std::move(inc));
}
