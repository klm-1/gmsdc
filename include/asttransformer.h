#ifndef ASTTRANSFORMER_H
#define ASTTRANSFORMER_H

#include "gmast.h"


class AstTransformer
{
public:
    static void transform(GmAST::ptr_t& ast);

private:
    static void matchArray2d(GmAST::ptr_t& ast);
    static void matchCompoundAssignment(GmAST::ptr_t& ast);
};

#endif // ASTTRANSFORMER_H
