#ifndef GMFORM_H
#define GMFORM_H

#include <iostream>
#include <vector>

#include "gmheader.h"
#include "gmchunk.h"
#include "utils.h"


class GmForm
{
public:
    GmHeader header;

    GmGen8Chunk general;
    GmOptnChunk options;
    GmLangChunk language;
    GmExtnChunk extensions;
    GmSondChunk sounds;
    GmAgrpChunk audioGroup;
    GmSprtChunk sprites;
    GmBgndChunk backgrounds;
    GmPathChunk paths;
    GmScptChunk scripts;
    GmGlobChunk globals;
    GmShdrChunk shaders;
    GmFontChunk fonts;
    GmTmlnChunk timelines;
    GmObjtChunk objects;
    GmRoomChunk rooms;
    GmDaflChunk datafiles;
    GmTpagChunk texturePages;
    GmCodeChunk code;
    GmVariChunk variables;
    GmFuncChunk functions;
    GmStrgChunk strings;
    GmTxtrChunk textures;
    GmAudoChunk audio;

    friend std::istream& operator>> (std::istream& in, GmForm& f);

    const char* nameByAddr(int cmd_addr) const;

private:
    std::vector<byte> raw_;
};

#endif // GMFORM_H
