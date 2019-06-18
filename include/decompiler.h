#ifndef DECOMPILER_H
#define DECOMPILER_H

#include <string>
#include <set>
#include <map>

#include "controltree.h"
#include "gmast.h"


class GmForm;
class ScriptEntry;
class GmxProject;


class Decompiler
{
public:
    struct Options
	{
        std::string outputDir;
        std::set<std::string> targets;
        std::set<std::string> ignore;
        bool logFlowgraph;
        bool logTree;
        bool logAssembly;
        bool decompileAll;

        static Options Debug();
        static Options Release();

        inline bool logAnything() const { return logAssembly || logFlowgraph || logTree; }
    };

    static const std::map<Operation, std::string> AsmOpToBinary;
    static const std::map<Operation, std::string> AsmOpToUnary;
    static const std::map<Operation, std::string> AsmOpToRelative;
    static const std::map<Comparison, std::string> ComparisonToOperator;

    Options options = Options::Release();

    Decompiler(GmForm& f);

    void decompile(GmxProject& proj);

private:
    struct Frame
	{
        std::vector<size_t> env_stack;
        std::vector<GmAST::ptr_t> expr_stack;
        std::vector<GmAST::ptr_t> stat_list;
    };

    GmForm* form_;
    std::vector<Frame> stack_;
    GmAST::ptr_t addr_;
    GmAST::ptr_t index_;
    GmAST::ptr_t ret_expr_;

    GmAST::ptr_t decompileScript(ScriptEntry const& src);
    GmAST::ptr_t analyzeControlTree(ControlTree* ct);
    void visit(ControlTree* ct, bool as_block = false, bool push_into = false);
    void decompileBaseBlock(const BaseBlock& bb);
    void applyCommand(const AsmCommand& cmd);
    void applyPush(const AsmCommand& cmd);
    void applySaveAddr(const AsmCommand& cmd);
    void applyDuplicate(const AsmCommand& cmd);
    void applyBinaryOp(Operation op);
    void applyBinaryOp(std::string op);
    void applyUnaryOp(const AsmCommand& cmd);
    void applyCompare(Comparison cmp);
    void applyTree(GmlPattern pat, int statc = 0, int exprc = 0);
    void applyCall(const AsmCommand& cmd);
    void applyAssignment(const AsmCommand& cmd);
    void applyReturn();
    GmAST::ptr_t popVariable(const AsmCommand& cmd);
    bool matchIncrement();
    bool matchRelative();
    bool matchSwap();
    Frame& frame();
    void pushFrame(GmAST::ptr_t in = nullptr);
    void popFrame();
};

#endif // DECOMPILER_H
