#include "gmform.h"

#include <fstream>
#include <cstdio>


std::istream& operator>> (std::istream& in, GmForm& f)
{
    /* Header size */
    int off = HEADER_SIZE;

    /* Read form header */
    f.raw_.resize(off);
    for (byte& b : f.raw_) 
	{
        b = in.get();
    }
    f.header = GmHeader(f.raw_.data(), 0);

    /* Read raw bytes */
    f.raw_.resize(f.header.size + off);
    for (size_t i = 0; i < f.header.size; ++i) 
	{
        f.raw_[off + i] = in.get();
    }

    /* Map chunks */
    const byte* data = f.raw_.data();
    for (size_t p = off; p < f.raw_.size();) 
	{
        GmChunk tmp(data, p);

        switch (SectionHeader(tmp.header.name)) 
		{
            case (SectionHeader::Form):
                break;

            case (SectionHeader::General):
                f.general = GmGen8Chunk(data, p);
                if (f.general.bytecodeVersion < 0x0f) 
				{
                    char tmp[64];
                    sprintf(tmp, "Bytecode version 0x%02X not supported", f.general.bytecodeVersion);
                    throw std::runtime_error(tmp);
                }
                break;

            case (SectionHeader::Options):
                f.options = GmOptnChunk(data, p);
                break;

            case (SectionHeader::Language):
                f.language = GmLangChunk(data, p);
                break;

            case (SectionHeader::Extensions):
                f.extensions = GmExtnChunk(data, p);
                break;

            case (SectionHeader::Sounds):
                f.sounds = GmSondChunk(data, p);
                break;

            case (SectionHeader::AudioGroup):
                f.audioGroup = GmAgrpChunk(data, p);
                break;

            case (SectionHeader::Sprites):
                f.sprites = GmSprtChunk(data, p);
                break;

            case (SectionHeader::Backgrounds):
                f.backgrounds = GmBgndChunk(data, p);
                break;

            case (SectionHeader::Paths):
                f.paths = GmPathChunk(data, p);
                break;

            case (SectionHeader::Scripts):
                f.scripts = GmScptChunk(data, p);
                break;

            case (SectionHeader::Globals):
                f.globals = GmGlobChunk(data, p);
                break;

            case (SectionHeader::Shaders):
                f.shaders = GmShdrChunk(data, p);
                break;

            case (SectionHeader::Fonts):
                f.fonts = GmFontChunk(data, p);
                break;

            case (SectionHeader::Timelines):
                f.timelines = GmTmlnChunk(data, p);
                break;

            case (SectionHeader::Objects):
                f.objects = GmObjtChunk(data, p);
                break;

            case (SectionHeader::Rooms):
                f.rooms = GmRoomChunk(data, p);
                break;

            case (SectionHeader::DataFiles):
                f.datafiles = GmDaflChunk(data, p);
                break;

            case (SectionHeader::TexturePage):
                f.texturePages = GmTpagChunk(data, p);
                break;

            case (SectionHeader::Code):
                f.code = GmCodeChunk(data, p);
                break;

            case (SectionHeader::Variables):
                f.variables = GmVariChunk(data, p);
                break;

            case (SectionHeader::Functions):
                f.functions = GmFuncChunk(data, p);
                break;

            case (SectionHeader::Strings):
                f.strings = GmStrgChunk(data, p);
                break;

            case (SectionHeader::Textures):
                f.textures = GmTxtrChunk(data, p);
                break;

            case (SectionHeader::Audio):
                f.audio = GmAudoChunk(data, p);
                break;

            default:
                std::cout << "Invalid section " << tmp.name() << std::endl;
        }
        p += tmp.size;

        std::cout << tmp.name() << " : " << tmp.size << std::endl;
    }

    /* Post-process */
    f.functions.postInit(f);
    f.variables.postInit(f);
    f.code.postInit(f);

    return in;
}

const char* GmForm::nameByAddr(int cmd_addr) const
{
    const char* ret;

    if ((ret = functions.funcName(cmd_addr))) 
	{
        return ret;
    }

    if ((ret = variables.varName(cmd_addr))) 
	{
        return ret;
    }

    return "[[unnamed]]";
}
