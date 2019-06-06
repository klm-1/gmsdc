#include "utils.h"

void string_replace_char(std::string& s, char c, const std::string& rep)
{
    for (size_t i = 0; i < s.size(); ++i) 
	{
        if (s[i] == c) 
		{ 
			s.replace(i, 1, rep); 
		}
    }
}
