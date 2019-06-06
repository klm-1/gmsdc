#ifndef GMHEADER_H
#define GMHEADER_H

#include <cstdint>
#include <string>

#include "utils.h"


enum class SectionHeader : uint32_t 
{
    Form        = 0x4D524F46, // FORM
    General     = 0x384E4547, // GEN8
    Options     = 0x4E54504F, // OPTN
    Language    = 0x474E414C, // LANG
    Extensions  = 0x4E545845, // EXTN
    Sounds      = 0x444E4F53, // SOND
    AudioGroup  = 0x50524741, // AGRP
    Sprites     = 0x54525053, // SPRT
    Backgrounds = 0x444E4742, // BGND
    Paths       = 0x48544150, // PATH
    Scripts     = 0x54504353, // SCPT
    Globals     = 0x424f4c47, // GLOB
    Shaders     = 0x52444853, // SHDR
    Fonts       = 0x544E4F46, // FONT
    Timelines   = 0x4E4C4D54, // TMLN
    Objects     = 0x544A424F, // OBJT
    Rooms       = 0x4D4F4F52, // ROOM
    DataFiles   = 0x4C464144, // DAFL
    TexturePage = 0x47415054, // TPAG
    Code        = 0x45444F43, // CODE
    Variables   = 0x49524156, // VARI
    Functions   = 0x434E5546, // FUNC
    Strings     = 0x47525453, // STRG
    Textures    = 0x52545854, // TXTR
    Audio       = 0x4F445541, // AUDO
};


struct GmHeader
{
    uint32_t name;
    uint32_t size, offset;

    GmHeader();
    GmHeader(const byte* raw, int off);

    std::string nameString() const;
};

#endif // GMHEADER_H
