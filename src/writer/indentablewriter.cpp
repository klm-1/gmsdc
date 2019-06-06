#include "indentablewriter.h"

#include <iostream>
#include <cstring>


IndentableWriter::IndentableWriter(std::ostream& os)
    : out_(os)
{}

const char* IndentableWriter::indent(int t)
{
    if(t > prev_) 
	{
        memset(spaces_, ' ', t * 4);
    }
    spaces_[t * 4] = '\0';
    prev_ = t;
    return spaces_;
}

const char* IndentableWriter::indent()
{
    return indent(depth);
}

std::ostream& IndentableWriter::out()
{
    return out_;
}

void IndentableWriter::stepIn()
{
    ++depth;
}

void IndentableWriter::stepOut()
{
    --depth;
}
