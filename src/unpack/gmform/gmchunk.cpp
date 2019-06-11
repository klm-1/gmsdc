#include "gmchunk.h"

#include "gmform.h"

#define UNREFERENCED_PARAMETER(_x) (void)_x;
#define TO_CSTR(_ptr) ((const char*)(_ptr))


std::string const& GmChunk::name() const
{
    return header.nameString;
}

int GmChunk::pastTheEndAddr()
{
    return start + size;
}

GmChunk::GmChunk(BinaryReader& br, SectionHeader hdr)
    : start(br.tell())
    , header(br)
    , size(header.size + HEADER_SIZE)
{
    ASSERT(header.nameCode == static_cast<uint32_t>(hdr));
    std::clog << header.nameString << " " << size << "\n";
}


GmGen8Chunk::GmGen8Chunk(BinaryReader& br)
    : GmChunk(br, SectionHeader::General)
{
    debug             = br.read<byte>();
    bytecodeVersion   = br.read<byte>();
    br.skip(2);
    filename          = br.readStringPtr();
    confname          = br.readStringPtr();
    br.skip(8);
    gameID            = br.read<uint32_t>();
    br.skip(16);
    name              = br.readStringPtr();
    major             = br.read<uint32_t>();
    minor             = br.read<uint32_t>();
    release           = br.read<uint32_t>();
    build             = br.read<uint32_t>();
    defaultWindowWidth = br.read<uint32_t>();
    defaultWindowHeight = br.read<uint32_t>();
    infoFlags         = br.read<uint32_t>();
    br.skip(20);
    timestamp         = br.read<uint64_t>();
    dispname          = br.readStringPtr();
    activeTargets     = br.read<uint32_t>();
    br.skip(16);
    steamAppID        = br.read<uint32_t>();

    br.seek(pastTheEndAddr());
}

GmOptnChunk::GmOptnChunk(BinaryReader& br)
    : GmChunk(br, SectionHeader::Options)
{
    br.skip(8);
    infoFlags = br.read<uint32_t>();
    br.skip(48);
    constants = GmPointerList<ConstEntry>(br);

    br.seek(pastTheEndAddr());
}

GmLangChunk::GmLangChunk(BinaryReader& br)
    : GmChunk(br, SectionHeader::Language)
{
    br.seek(pastTheEndAddr());
}

GmExtnChunk::GmExtnChunk(BinaryReader& br)
    : GmChunk(br, SectionHeader::Extensions)
{
    br.seek(pastTheEndAddr());
}

GmSondChunk::GmSondChunk(BinaryReader& br)
    : GmListChunk<SoundEntry>(br, SectionHeader::Sounds)
{
    br.seek(pastTheEndAddr());
}

GmAgrpChunk::GmAgrpChunk(BinaryReader& br)
    : GmChunk(br, SectionHeader::AudioGroup)
{
    br.seek(pastTheEndAddr());
}

GmSprtChunk::GmSprtChunk(BinaryReader& br)
    : GmListChunk<SpriteEntry>(br, SectionHeader::Sprites)
{
    br.seek(pastTheEndAddr());
}

GmBgndChunk::GmBgndChunk(BinaryReader& br)
    : GmListChunk<BackgroundEntry>(br, SectionHeader::Backgrounds)
{
    br.seek(pastTheEndAddr());
}

GmPathChunk::GmPathChunk(BinaryReader& br)
    : GmListChunk<PathEntry>(br, SectionHeader::Paths)
{
    br.seek(pastTheEndAddr());
}

GmScptChunk::GmScptChunk(BinaryReader& br)
    : GmListChunk<ScriptDefEntry>(br, SectionHeader::Scripts)
{
    br.seek(pastTheEndAddr());
}

GmGlobChunk::GmGlobChunk(BinaryReader& br)
    : GmListChunk<GlobalvarEntry>(br, SectionHeader::Globals)
{
    br.seek(pastTheEndAddr());
}

GmShdrChunk::GmShdrChunk(BinaryReader& br)
    : GmListChunk<ShaderEntry>(br, SectionHeader::Shaders)
{
    br.seek(pastTheEndAddr());
}

GmFontChunk::GmFontChunk(BinaryReader& br)
    : GmListChunk<FontEntry>(br, SectionHeader::Fonts)
{
    br.seek(pastTheEndAddr());
}

GmTmlnChunk::GmTmlnChunk(BinaryReader& br)
    : GmListChunk<TimelineEntry>(br, SectionHeader::Timelines)
{
    br.seek(pastTheEndAddr());
}

GmObjtChunk::GmObjtChunk(BinaryReader& br)
    : GmListChunk<ObjectEntry>(br, SectionHeader::Objects)
{
    br.seek(pastTheEndAddr());
}

GmRoomChunk::GmRoomChunk(BinaryReader& br)
    : GmListChunk<RoomEntry>(br, SectionHeader::Rooms)
{
    br.seek(pastTheEndAddr());
}

GmDaflChunk::GmDaflChunk(BinaryReader& br)
    : GmChunk(br, SectionHeader::DataFiles)
{
    br.seek(pastTheEndAddr());
}

GmTpagChunk::GmTpagChunk(BinaryReader& br)
    : GmListChunk<TextureEntry>(br, SectionHeader::TexturePage)
{
    br.seek(pastTheEndAddr());
}

GmCodeChunk::GmCodeChunk(BinaryReader& br)
    : GmListChunk<ScriptEntry>(br, SectionHeader::Code)
{
    for (ScriptEntry& scr : content)
	{
        for (AsmCommand& cmd : scr.code)
		{
            cmd.addr += scr.codeOffset;
        }
    }

    br.seek(pastTheEndAddr());
}

AsmCommand* GmCodeChunk::at(int off)
{
    for (ScriptEntry& scr : content)
	{
	    if (scr.codeOffset + scr.codeSize < off) { continue; }
        auto it = std::find_if(scr.begin(), scr.end(), [off](auto & cmd)
		{
            return cmd.addr == off;
        });
        if (it != scr.end())
		{
            return &*it;
        }
    }
    return nullptr;
}

void GmCodeChunk::postInit(GmForm& f)
{
    for (ScriptEntry& scr : content)
	{
        for (AsmCommand& cmd : scr)
		{
            cmd.initText(&f);
        }
    }
}

GmVariChunk::GmVariChunk(BinaryReader& br)
    : GmChunk(br, SectionHeader::Variables)
{
    int unknown = 12;
    br.skip(unknown);

    int count = (size - HEADER_SIZE - unknown) / VariableDefEntry::static_size;

    refVar.reserve(count);
    for (int i = 0; i < count; ++i)
	{
        refVar.push_back(VariableDefEntry(br));
    }

    br.seek(pastTheEndAddr());
}

GmFuncChunk::GmFuncChunk(BinaryReader& br)
    : GmChunk(br, SectionHeader::Functions)
{
    /* Function definitions */
    uint32_t funcCount = br.read<uint32_t>();

    refFunc.reserve(funcCount);
    for (size_t i = 0; i < funcCount; ++i)
	{
        refFunc.push_back(FunctionDefEntry(br));
    }

    /* Scripts and locals */
    uint32_t scptCount = br.read<uint32_t>();

    refScript.reserve(scptCount);
    for (size_t i = 0; i < scptCount; ++i)
	{
        refScript.push_back(LocalScriptDefEntry(br));
    }

    br.seek(pastTheEndAddr());
}

void GmFuncChunk::postInit(GmForm& f)
{
    for (size_t i = 0; i < refFunc.size(); ++i)
	{
        FunctionDefEntry& rf = refFunc[i];
        int entry = rf.firstOccurrence;

        for (size_t ec = 0; ec < rf.occurrenceCount; ++ec)
		{
            AsmCommand* cmd = f.code().at(entry);

            if (!cmd || cmd->operation() != Operation::Call)
			{
                break;
            }

            cmd->symbol = rf.name;
            int shift = cmd->dataInt32();
            entry += shift;
            if (!shift)
			{
                break;
            }
        }
    }
}

void GmVariChunk::postInit(GmForm& f)
{
    for (size_t i = 0; i < refVar.size(); ++i)
	{
        VariableDefEntry& rv = refVar[i];
        int entry = rv.firstOccurrence;

        for (size_t ec = 0; ec < rv.occurrenceCount; ++ec)
		{
            AsmCommand* cmd = f.code().at(entry);

            ASSERT(cmd && (OperationIsPush(cmd->operation()) || cmd->operation() == Operation::Set));
            ASSERT(cmd->symbol.empty());

            cmd->symbol = rv.name;
            int shift = cmd->variable().nameIndex;
            entry += shift;

            if (!shift)
			{
			    ASSERT(ec + 1 == rv.occurrenceCount);
                break;
            }
        }
    }
}

GmStrgChunk::GmStrgChunk(BinaryReader& br)
    : GmListChunk<StringEntry>(br, SectionHeader::Strings)
{
    br.seek(pastTheEndAddr());
}

std::string const& GmStrgChunk::get(int index) const
{
    return (*this)[index].data;
}

GmTxtrChunk::GmTxtrChunk(BinaryReader& br)
    : GmChunk(br, SectionHeader::Textures)
{
    br.seek(pastTheEndAddr());
}

GmAudoChunk::GmAudoChunk(BinaryReader& br)
    : GmChunk(br, SectionHeader::Audio)
{
    br.seek(pastTheEndAddr());
}

PathPoint::PathPoint(BinaryReader& br)
    : x(br.read<float>())
    , y(br.read<float>())
    , speed(br.read<float>())
{}

SpriteEntry::SpriteEntry(BinaryReader& br)
{
    name           = br.readStringPtr();
    width          = br.read<uint32_t>();
    height         = br.read<uint32_t>();
    maskLeft       = br.read<uint32_t>();
    maskRight      = br.read<uint32_t>();
    maskBottom     = br.read<uint32_t>();
    maskTop        = br.read<uint32_t>();
    br.skip(12);
    bboxMode       = br.read<uint32_t>();
    separateMasks  = br.read<uint32_t>();
    originX        = br.read<uint32_t>();
    originY        = br.read<uint32_t>();
    textureCount   = br.read<uint32_t>();
    textureOffset  = br.read<uint32_t>();
}

BackgroundEntry::BackgroundEntry(BinaryReader& br)
{
    name           = br.readStringPtr();
    br.skip(12);
    textureOffset  = br.read<uint32_t>();
}

PathEntry::PathEntry(BinaryReader& br)
{
    name      = br.readStringPtr();
    isSmooth  = br.read<uint32_t>();
    isClosed  = br.read<uint32_t>();
    precision = br.read<uint32_t>();

    int32_t n_pts = br.read<int32_t>();;

    points.reserve(n_pts);
    for (int i = 0; i < n_pts; ++i)
    {
        points.push_back(PathPoint(br));
    }
}

ScriptDefEntry::ScriptDefEntry(BinaryReader& br)
{
    name   = br.readStringPtr();
    codeID = br.read<uint32_t>();
}

ScriptEntry::ScriptEntry(BinaryReader& br)
{
    name           = br.readStringPtr();
    codeSize       = br.read<int32_t>();
    br.skip(4);
    int32_t shift  = br.read<int32_t>();
    codeOffset     = br.tell() + shift - 4;

    br.seek(codeOffset);
    std::vector<byte> bytecode(codeSize);
    br.read<byte>(bytecode.data(), codeSize);

    /* Disasm */
    code = Disassemble(bytecode);
}

std::ostream& operator<< (std::ostream& out, const ScriptEntry& s)
{
    for (const AsmCommand& cmd : s.code)
	{
        out << cmd.toString() << std::endl;
    }
    return out;
}

GlobalvarEntry::GlobalvarEntry(BinaryReader& br)
{}

ShaderEntry::ShaderEntry(BinaryReader& br)
{
    name = br.readStringPtr();
}

ObjectEntry::ObjectEntry(BinaryReader& br)
{
    name = br.readStringPtr();
}

RoomEntry::RoomEntry(BinaryReader& br)
{
    name = br.readStringPtr();
}

TextureEntry::TextureEntry(BinaryReader& br)
{}

StringEntry::StringEntry(BinaryReader& br)
{
    uint32_t length = br.read<uint32_t>();

    data.resize(length);
    br.read(&data[0], length);
}

ConstEntry::ConstEntry(BinaryReader& br)
{}

SoundEntry::SoundEntry(BinaryReader& br)
{
    name = br.readStringPtr();
    flags = br.read<uint32_t>();
    typeOffset = br.read<uint32_t>();
    fileOffset = br.read<uint32_t>();
    br.skip(4);
    volume = br.read<float>();
    pitch = br.read<float>();
    groupID = br.read<uint32_t>();
    audioID = br.read<uint32_t>();
}

TimelineEntry::TimelineEntry(BinaryReader& br)
{
    name = br.readStringPtr();
}

FunctionDefEntry::FunctionDefEntry(BinaryReader& br)
{
    name = br.readStringPtr();
    occurrenceCount = br.read<uint32_t>();
    firstOccurrence = br.read<uint32_t>();
}

FontEntry::FontEntry(BinaryReader& br)
{
    name = br.readStringPtr();
}

VariableDefEntry::VariableDefEntry(BinaryReader& br)
{
    name = br.readStringPtr();
    br.skip(8);
    occurrenceCount = br.read<int32_t>();
    firstOccurrence = br.read<int32_t>();
}

LocalVariableDefEntry::LocalVariableDefEntry(BinaryReader& br)
{
    type = br.read<VariableType>();
    name = br.readStringPtr();
}

LocalScriptDefEntry::LocalScriptDefEntry(BinaryReader& br)
{
    int localsCount = br.read<uint32_t>();
    name = br.readStringPtr();

    refVar.reserve(localsCount);
    for (int i = 0; i < localsCount; ++i)
	{
        refVar.push_back(LocalVariableDefEntry(br));
    }
}
