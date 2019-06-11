#include "gmheader.h"
#include "binaryreader.h"


GmHeader::GmHeader(BinaryReader& br)
    : nameCode(br.read<uint32_t>())
    , size(br.read<uint32_t>())
    , nameString({
        char(nameCode & 0xff),
        char((nameCode >> 8) & 0xff),
        char((nameCode >> 16) & 0xff),
        char(nameCode >> 24)
    })
{}
