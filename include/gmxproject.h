#ifndef GMXPROJECT_H
#define GMXPROJECT_H

#include <string>
#include <vector>

#include "gmast.h"

class GmForm;


class GmxProject
{
public:
    struct Options
    {
        bool separateScripts = true;
        std::string codeDir = ".";
        std::string scriptsDir = "scripts";
    };

    struct GmlScript
    {
        std::string fullName;
        std::vector<ExprContext> argCtx;
        GmAST::ptr_t ast;
    };

    Options options;

    GmxProject();

    void analyzeContexts();
    void exportGmx(GmForm&, std::string const& dir);

    void addCode(std::string const& full_name, GmAST::ptr_t ast);

private:
    std::map<std::string, GmlScript> scripts_;
    std::map<std::string, GmAST::ptr_t> codes_;
};

#endif // GMXPROJECT_H
