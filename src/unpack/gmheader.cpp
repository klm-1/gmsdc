#include "gmheader.h"


std::string GmHeader::nameString() const
{
    std::string ret;

    ret += name & 0xff;
    ret += (name >> 8) & 0xff;
    ret += (name >> 16) & 0xff;
    ret += (name >> 24) & 0xff;

    return ret;
}

GmHeader::GmHeader()
    : name(0)
    , size(0)
    , offset(-1)
{}

GmHeader::GmHeader(const byte* raw, int off)
    : name(*(uint32_t*)(raw + off))
    , size(*((uint32_t*)(raw + off) + 1))
    , offset(off)
{}
