#ifndef GMAST_H
#define GMAST_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>

#include "baseblock.h"
#include "gmlexprcontext.h"


enum class GmlPattern 
{
    Invalid,

    If,
    IfElse,
    While,
    Repeat,
    Break,
    Continue,
    DoUntil,
    LinearBlock,
    BinaryOp,
    UnaryOp,
    Number,
    String,
    Assignment,
    CompoundAssignment,
    FunctionCall,
    Return,
    Exit,
    Nop,
    Variable,
    VarScope,
    DroppedExpr,
    Prefix,
    Postfix,
    ArrayElement,
    ArrayElement2,
    Switch,
    SwitchCase,
    SwitchDefault,
    With,
};

const char* GmlPattern2String(GmlPattern p);
bool GmlNeedsPadding(GmlPattern p);

extern const std::map<std::string, int> BinaryOpPriority;
extern const std::map<std::string, int> UnaryOpPriority;

class GmAST
{
public:
    typedef std::unique_ptr<GmAST> ptr_t;

    template<class... Ts>
    static ptr_t make(Ts&&... arg)
    {
        return std::make_unique<GmAST>(std::forward<Ts>(arg)...);
    }

    GmAST();
    GmAST(GmlPattern t);
    GmAST(GmlPattern t, const std::string& data);
    GmAST(GmlPattern t, int64_t data);
    GmAST(GmlPattern t, double data);
    GmAST(GmlPattern t, std::vector<ptr_t>&& l);

    GmlPattern pattern() const;
    int leavesCount() const;
    bool isNumber() const;
    bool isNumber(int x) const;
    bool isNop() const;
    const std::string& dataString() const;
    int64_t dataInt() const;
    double dataReal() const;
    ptr_t deepcopy() const;
    const GmAST* leftLeaf() const;
    const GmAST* rightLeaf() const;
    const GmAST* leaf(int i = 0) const;
    bool deepEquals(const GmAST& other) const;

    void dataString(const std::string& s);
    void dataInt(int64_t x);
    void dataReal(double x);
    void addLeaf(ptr_t l);
    void addLeaf(ptr_t l, size_t p);
    GmAST* leftLeaf();
    GmAST* rightLeaf();
    GmAST* leaf(int i = 0);

    bool operator== (const GmAST& other) const;
    bool operator!= (const GmAST& other) const;
    friend std::ostream& operator<< (std::ostream& out, const GmAST& ast);
    friend class GraphmlWriter;
    friend class GmlWriter;
    friend class AstTransformer;

private:
    ExprContext::Type context_ = ExprContext::Unknown;
    GmlPattern pat_ = GmlPattern::Invalid;
    GmAST* uplink_ = nullptr;
    int64_t val_int_ = -1;
    double val_double_ = -1;
    std::string val_string_;
    std::vector<ptr_t> links_;
};

#endif // GMAST_H
