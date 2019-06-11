#include "gmform.h"

#include <fstream>
#include <cstdio>

#include "gmform/16/gmform16.h"


GmForm::ptr_t GmForm::Read(BinaryReader& br)
{
    GmHeader header(br);
    ASSERT(header.nameCode == static_cast<uint32_t>(SectionHeader::Form));

    br.seek(0x11);
    int version = br.read<byte>();
    br.seek(0);

    switch (version)
    {
    case 16:
        {
            return ptr_t(new GmForm16(br));
        }
        break;

    case 14:
    case 15:
    default:
        {
            char tmp[64];
            sprintf(tmp, "Bytecode version 0x%02X not supported", version);
            throw std::runtime_error(tmp);
        }
    }
}
