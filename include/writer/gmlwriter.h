#ifndef GMLWRITER_H
#define GMLWRITER_H

#include <iosfwd>
#include <set>
#include <string>

#include "indentablewriter.h"
#include "gmlexprcontext.h"
#include "gmform.h"

class GmAST;


class GmlWriter : public IndentableWriter
{
public:
    GmlWriter(std::ostream& os, GmForm& f);
    void print(const GmAST& ast);

private:
    GmForm& form_;
    bool padAllowed_ = true;
    std::set<std::string> locals_;

    void writeLocalVariables(const GmAST& ast);
    void writeCode(const GmAST& ast, bool ind = true);
    void writeExpression(const GmAST& ast);
    void writeExpression(const GmAST& ast, ExprContext::Type ctx);
    void writeFunctionCall(const GmAST& ast);
    void writeBinaryOp(const GmAST& ast);
    void writeScope(const GmAST& ast);
    void writeDatetime();
    void writeHexadecimal(unsigned x, unsigned prec = 0);
    void writePaddingLine();
    void writeLine(const char* str);
    void beginLine(const char* str = "");
    void endLine(const char* str = "");

    void collectLocalVariables(const GmAST& ast);
    const char* queryForm(int n, ExprContext::Type ctx);

    template<class Chunk>
    const char* queryChunk(const Chunk& ch, int index)
    {
        if (index < 0 || index >= ch.size) { return nullptr; }
        return ch[index].name.c_str();
    }
};

#endif // GMLWRITER_H
