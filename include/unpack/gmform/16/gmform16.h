#ifndef GMFORM16_H
#define GMFORM16_H

#include "gmform.h"


class GmForm16 : public GmForm
{
public:
    GmForm16(BinaryReader& br);
    virtual ~GmForm16();

    virtual int bytecodeVersion() const override { return 16; }

    virtual GmStrgChunk const& strings()     const override { return strings_; }
    virtual GmCodeChunk const& code()        const override { return code_; }
    virtual GmSprtChunk const& sprites()     const override { return sprites_; }
    virtual GmSondChunk const& sounds()      const override { return sounds_; }
    virtual GmBgndChunk const& backgrounds() const override { return backgrounds_; }
    virtual GmPathChunk const& paths()       const override { return paths_; }
    virtual GmScptChunk const& scripts()     const override { return scripts_; }
    virtual GmShdrChunk const& shaders()     const override { return shaders_; }
    virtual GmFontChunk const& fonts()       const override { return fonts_; }
    virtual GmTmlnChunk const& timelines()   const override { return timelines_; }
    virtual GmObjtChunk const& objects()     const override { return objects_; }
    virtual GmRoomChunk const& rooms()       const override { return rooms_; }

    virtual GmCodeChunk& code() override { return code_; }

private:
    GmHeader header_;

    GmGen8Chunk general_;
    GmOptnChunk options_;
    GmLangChunk language_;
    GmExtnChunk extensions_;
    GmSondChunk sounds_;
    GmAgrpChunk audioGroup_;
    GmSprtChunk sprites_;
    GmBgndChunk backgrounds_;
    GmPathChunk paths_;
    GmScptChunk scripts_;
    GmGlobChunk globals_;
    GmShdrChunk shaders_;
    GmFontChunk fonts_;
    GmTmlnChunk timelines_;
    GmObjtChunk objects_;
    GmRoomChunk rooms_;
    GmDaflChunk datafiles_;
    GmTpagChunk texturePages_;
    GmCodeChunk code_;
    GmVariChunk variables_;
    GmFuncChunk functions_;
    GmStrgChunk strings_;
    GmTxtrChunk textures_;
    GmAudoChunk audio_;
};

#endif // GMFORM16_H
