#ifndef BINARYREADER_H
#define BINARYREADER_H

#include <vector>
#include <ios>

#include "utils.h"


class BinaryReader
{
public:
    BinaryReader(std::istream& is);

    template<class T>
    typename std::enable_if<std::is_arithmetic<T>::value || std::is_enum<T>::value,
    typename std::decay<T>::type>
    ::type read()
    {
        using PT = typename std::decay<T>::type;

        ASSERT(ptr_ + sizeof(T) <= buf_.size());
        PT ret;
        char* ptr = reinterpret_cast<char*>(&ret);
        for (unsigned i = 0; i < sizeof(T); ++i) // little-endian
        {
            ptr[i] = buf_[ptr_++];
        }
        return ret;
    }

    template<class T>
    void read(typename std::remove_const<T>::type* o, int n)
    {
        while (n--)
        {
            *o++ = read<typename std::remove_const<T>::type>();
        }
    }

    int read(char* o, int n = -1);
    std::string readStringPtr();
    void skip(int n);
    void seek(int p);
    int tell();

private:
    std::vector<char> buf_;
    size_t ptr_;
};


#endif // BINARYREADER_H
