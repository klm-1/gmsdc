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
    using ptr_t = std::unique_ptr<GmForm>;

    static ptr_t Read(BinaryReader& br);

    virtual ~GmForm() {};
    virtual int bytecodeVersion() const = 0;

    virtual GmCodeChunk const& code() const = 0;
    virtual GmStrgChunk const& strings() const = 0;
    virtual GmSprtChunk const& sprites() const = 0;
    virtual GmSondChunk const& sounds() const = 0;
    virtual GmBgndChunk const& backgrounds() const = 0;
    virtual GmPathChunk const& paths() const = 0;
    virtual GmScptChunk const& scripts() const = 0;
    virtual GmShdrChunk const& shaders() const = 0;
    virtual GmFontChunk const& fonts() const = 0;
    virtual GmTmlnChunk const& timelines() const = 0;
    virtual GmObjtChunk const& objects() const = 0;
    virtual GmRoomChunk const& rooms() const = 0;

    virtual GmCodeChunk& code() = 0;
};

#endif // GMFORM_H
