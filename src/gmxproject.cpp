#include "gmxproject.h"

#include <fstream>

#include "fsmanager.h"
#include "utils.h"
#include "gmlwriter.h"

GmxProject::GmxProject()
{}

void GmxProject::analyzeContexts()
{

}

void GmxProject::exportGmx(GmForm& f, std::string const& dir)
{
    std::clog << "Writing code...\n";

    std::wstring wdir = wide(dir);

    FsManager::directoryCreate(wdir);
    FsManager::directoryCreate(wdir + L"/" + wide(options.codeDir));
    if (options.separateScripts)
    {
        FsManager::directoryCreate(wdir + L"/" + wide(options.scriptsDir));
    }

    std::string codePrefix = dir + "/" + options.codeDir + "/";
    std::string scriptsPrefix = options.separateScripts
        ? (dir + "/" + options.scriptsDir + "/")
        : codePrefix;

    for (auto& kv : scripts_)
    {
        std::ofstream tmp(scriptsPrefix + kv.first + ".gml");
        GmlWriter(tmp, f).print(*kv.second.ast);
    }
    for (auto& kv : codes_)
    {
        std::ofstream tmp(codePrefix + kv.first + ".gml");
        GmlWriter(tmp, f).print(*kv.second);
    }
}

void GmxProject::addCode(std::string const& full_name, GmAST::ptr_t ast)
{
    static const char prefix[] = "gml_Script_";

    for (size_t i = 0; i < full_name.size(); ++i)
    {
        // Code is script
        if (prefix[i] == '\0')
        {
            GmlScript scr{
                full_name,
                {},
                std::move(ast)
            };

            std::string short_name(&full_name[i]);

            scripts_[short_name] =  std::move(scr);
            return;
        }

        if (prefix[i] != full_name[i]) { break; }
    }

    codes_[full_name] = std::move(ast);
}
