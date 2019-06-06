#include "gmlwriter.h"

#include <string>

using namespace std;


const char* ExprContext::ValueInContext(int val, Type ctx)
{
    static string flags;

    const auto itab = contextVal_.find(ctx);
    if (itab != contextVal_.end()) 
	{
        const auto& tab = itab->second;
        const auto it = tab.find(val);

        if (it != tab.end()) 
		{
            return it->second.c_str();
        }
    }

    const auto itab2 = flagVal_.find(ctx);
    if (itab2 != flagVal_.end()) 
	{
        flags.clear();
        int hits = 0;

        for (const auto& it : itab2->second) 
		{
            if (val & it.first) 
			{
                if (hits > 0) 
				{
                    flags += " | ";
                }
                flags += it.second;
                val -= it.first;
            }
        }

        if (val) 
		{
            return nullptr;
        }

        if (hits > 1) 
		{
            flags.insert(0, "(");
            flags += ")";
        }

        return flags.c_str();
    }

    return nullptr;
}

