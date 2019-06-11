#include "binaryreader.h"

#include <iostream>
#include <algorithm>

BinaryReader::BinaryReader(std::istream& is)
    : buf_()
    , ptr_(0)
{
    buf_.reserve(64 * 1024);
    std::copy(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>(), std::back_inserter(buf_));
}

void BinaryReader::skip(int n)
{
    ptr_ += n;
}

void BinaryReader::seek(int p)
{
    ptr_ = p;
}

int BinaryReader::read(char* o, int n)
{
    int i = 0;
    while (n--)
    {
        *o++ = read<char>();
        ++i;
    }
    return i;
}

std::string BinaryReader::readStringPtr()
{
    uint32_t off = read<uint32_t>();
    std::string ret(&buf_.at(off));
    return std::move(ret);
}

int BinaryReader::tell()
{
    return ptr_;
}
