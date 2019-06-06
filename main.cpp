#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "gmform.h"
#include "utils.h"
#include "decompiler.h"
#include "fsmanager.h"
#include "algext.h"


struct Options 
{
    std::string dataWin = "data.win";
    std::string outputDir = "out";
    std::string logFile = "log.txt";
    std::vector<std::string> targets;
    std::vector<std::string> ignore;
    bool verboseLog = false;
};


void printUsage()
{
    std::cout <<
              "Usage: gmsdc [OPTIONS]\n"
              " -t \"<script1>[,script2...]\" - Target scripts (default all)\n"
              " -e \"<script1>[,script2...]\" - Exclude scripts\n"
              " -f <file>   - Your 'data.win' file. (default './data.win')\n"
              " -o <dir>    - Output folder. (default './out')\n"
              " -v          - Verbose log.\n"
              ;
}

std::vector<std::string> strsplit(const char* str)
{
    std::vector<std::string> ret;
    ret.emplace_back();

    for (const char* pc = str; *pc; ++pc) 
	{
        if (*pc == ',') 
		{
            ret.emplace_back();
        } 
		else if (*pc != ' ') 
		{
            ret.back().push_back(*pc);
        }
    }

    return std::move(ret);
}

Options parse_commandline(int argc, char** argv)
{
    Options ret;

    for (int i = 1; i < argc;) 
	{
        if (!strcmp(argv[i], "-f")) 
		{
            if (i == argc - 1) 
			{
                printUsage();
                break;
            }
            ret.dataWin = argv[i + 1];
            i += 2;

        } 
		else if (!strcmp(argv[i], "-o")) 
		{
            if (i == argc - 1) 
			{
                printUsage();
                break;
            }
            ret.outputDir = argv[i + 1];
            i += 2;

        } 
		else if (!strcmp(argv[i], "-v")) 
		{
            ret.verboseLog = true;
            ++i;

        } 
		else if (!strcmp(argv[i], "-t")) 
		{
            if (i == argc - 1) 
			{
                printUsage();
                break;
            }
            ret.targets = strsplit(argv[i + 1]);
            i += 2;

        } 
		else if (!strcmp(argv[i], "-e")) 
		{
            if (i == argc - 1) 
			{
                printUsage();
                break;
            }
            ret.ignore = strsplit(argv[i + 1]);
            i += 2;

        } 
		else 
		{
            printUsage();
            break;
        }
    }

    return ret;
}


int main(int argc, char** argv)
{
    Options opt = parse_commandline(argc, argv);

    std::ifstream dump(opt.dataWin, std::ios::binary);
    FsManager::directoryDelete(std::wstring(opt.outputDir.begin(), opt.outputDir.end()));

    GmForm f;
    dump >> f;

    Decompiler::Options dcOptn = Decompiler::Options::Debug();
    dcOptn.outputDir = opt.outputDir;
    dcOptn.logFlowgraph = dcOptn.logTree = dcOptn.logAssembly = opt.verboseLog;
    dcOptn.decompileAll = opt.targets.empty();
    std::copy(opt.targets, std::inserter(dcOptn.targets, dcOptn.targets.end()));
    std::copy(opt.ignore, std::inserter(dcOptn.ignore, dcOptn.ignore.end()));

    Decompiler dc(f);
    dc.decompile(dcOptn);

    return 0;
}
