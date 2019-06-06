#include "gmchunk.h"

#include "gmform.h"

#define UNREFERENCED_PARAMETER(_x) (void)_x;
#define TO_CSTR(_ptr) ((const char*)(_ptr))


std::string GmChunk::name() const
{
    return header.nameString();
}

GmChunk::GmChunk()
    : header()
    , size(0)
    , offset(-1)
{}

GmChunk::GmChunk(const byte* raw, int off)
    : header(raw, off)
    , size(header.size + HEADER_SIZE)
    , offset(off)
{}


GmGen8Chunk::GmGen8Chunk(const byte* raw, int off)
    : GmChunk(raw, off)
{
    off += HEADER_SIZE;
    PTR_LOOP_INIT();

    PTR_LOOP_READ(byte, debug);
    PTR_LOOP_READ(byte, bytecodeVersion);
    PTR_LOOP_READ_ARRAY(byte, pad0, 2);
    PTR_LOOP_READ(uint32_t, filenameOffset);
    PTR_LOOP_READ(uint32_t, configOffset);
    PTR_LOOP_READ(uint32_t, pad1);
    PTR_LOOP_READ(uint32_t, pad2);
    PTR_LOOP_READ(uint32_t, gameID);
    PTR_LOOP_READ_ARRAY(uint32_t, pad3, 4)
    PTR_LOOP_READ(uint32_t, nameOffset);
    PTR_LOOP_READ(uint32_t, major);
    PTR_LOOP_READ(uint32_t, minor);
    PTR_LOOP_READ(uint32_t, release);
    PTR_LOOP_READ(uint32_t, build);
    PTR_LOOP_READ(uint32_t, defaultWindowWidth);
    PTR_LOOP_READ(uint32_t, defaultWindowHeight);
    PTR_LOOP_READ(uint32_t, infoFlags);
    PTR_LOOP_READ_ARRAY(byte, licenseMd5, 16);
    PTR_LOOP_READ(uint32_t, licenseCRC);
    PTR_LOOP_READ(uint64_t, timestamp);
    PTR_LOOP_READ(uint32_t, displayNameOffset);
    PTR_LOOP_READ(uint32_t, activeTargets);
    PTR_LOOP_READ_ARRAY(uint32_t, pad4, 4);
    PTR_LOOP_READ(uint32_t, steamAppID);
    PTR_LOOP_READ(uint32_t, pad5);
}

GmOptnChunk::GmOptnChunk(const byte* raw, int off)
    : GmChunk(raw, off)
{
    PTR_LOOP_INIT();

    PTR_LOOP_READ_ARRAY(uint32_t, pad0, 2);
    PTR_LOOP_READ(uint32_t, infoFlags);
    PTR_LOOP_READ_ARRAY(uint32_t, pad1, 12);

    constants = GmList<ConstEntry>(raw, PTR_LOOP_OFFSET());
}

GmLangChunk::GmLangChunk(const byte* raw, int off)
    : GmChunk(raw, off)
{}

GmExtnChunk::GmExtnChunk(const byte* raw, int off)
    : GmChunk(raw, off)
{}

GmSondChunk::GmSondChunk(const byte* raw, int off)
    : GmListChunk<SoundEntry>(raw, off)
{}

GmAgrpChunk::GmAgrpChunk(const byte* raw, int off)
    : GmChunk(raw, off)
{}

GmSprtChunk::GmSprtChunk(const byte* raw, int off)
    : GmListChunk<SpriteEntry>(raw, off)
{}

GmBgndChunk::GmBgndChunk(const byte* raw, int off)
    : GmListChunk<BackgroundEntry>(raw, off)
{}

GmPathChunk::GmPathChunk(const byte* raw, int off)
    : GmListChunk<PathEntry>(raw, off)
{}

GmScptChunk::GmScptChunk(const byte* raw, int off)
    : GmListChunk<ScriptDefEntry>(raw, off)
{}

GmGlobChunk::GmGlobChunk(const byte* raw, int off)
    : GmListChunk<GlobalvarEntry>(raw, off)
{}

GmShdrChunk::GmShdrChunk(const byte* raw, int off)
    : GmListChunk<ShaderEntry>(raw, off)
{}

GmFontChunk::GmFontChunk(const byte* raw, int off)
    : GmListChunk<FontEntry>(raw, off)
{}

GmTmlnChunk::GmTmlnChunk(const byte* raw, int off)
    : GmListChunk<TimelineEntry>(raw, off)
{}

GmObjtChunk::GmObjtChunk(const byte* raw, int off)
    : GmListChunk<ObjectEntry>(raw, off)
{}

GmRoomChunk::GmRoomChunk(const byte* raw, int off)
    : GmListChunk<RoomEntry>(raw, off)
{}

GmDaflChunk::GmDaflChunk(const byte* raw, int off)
    : GmChunk(raw, off)
{}

GmTpagChunk::GmTpagChunk(const byte* raw, int off)
    : GmListChunk<TextureEntry>(raw, off)
{}

GmCodeChunk::GmCodeChunk(const byte* raw, int off)
    : GmListChunk<ScriptEntry>(raw, off)
{
    for (ScriptEntry& scr : content) 
	{
        for (AsmCommand& cmd : scr.code) 
		{
            cmd.addr += scr.codeOffset + 0x0C; // Magic number. Probably HEADER_SIZE + 4
        }
    }
}

AsmCommand* GmCodeChunk::at(int off)
{
    for (ScriptEntry& scr : content) 
	{
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

GmVariChunk::GmVariChunk(const byte* raw, int off)
    : GmChunk(raw, off)
{
    PTR_LOOP_INIT();
    PTR_LOOP_SHIFT(HEADER_SIZE);

    int unknown = 12;
    PTR_LOOP_SHIFT(unknown);
    off = PTR_LOOP_OFFSET();

    int count = (size - unknown) / VariableDefEntry::static_size;
    refVar.reserve(count);

    for (int i = 0; i < count; ++i) 
	{
        refVar.push_back(VariableDefEntry(raw, off));
        off += VariableDefEntry::static_size;
    }
}

GmFuncChunk::GmFuncChunk(const byte* raw, int off)
    : GmChunk(raw, off)
{
    PTR_LOOP_INIT();
    PTR_LOOP_SHIFT(HEADER_SIZE);

    /* Function definitions */
    uint32_t funcCount;
    PTR_LOOP_READ(uint32_t, funcCount);

    off = PTR_LOOP_OFFSET();
    refFunc.reserve(funcCount);

    for (size_t i = 0; i < funcCount; ++i) 
	{
        refFunc.push_back(FunctionDefEntry(raw, off));
        off += FunctionDefEntry::static_size;
    }

    PTR_LOOP_SHIFT(funcCount * FunctionDefEntry::static_size);

    /* Scripts and locals */
    uint32_t scptCount;
    PTR_LOOP_READ(uint32_t, scptCount);

    off = PTR_LOOP_OFFSET();
    refScript.reserve(scptCount);

    for (size_t i = 0; i < scptCount; ++i) 
	{
        refScript.push_back(LocalScriptDefEntry(raw, off));
        off += refScript.back().size;
    }
}

void GmFuncChunk::postInit(GmForm& f)
{
    for (size_t i = 0; i < refFunc.size(); ++i) 
	{
        FunctionDefEntry& rf = refFunc[i];
        int entry = rf.firstOccurrence;

        for (size_t ec = 0; ec < rf.occurrenceCount; ++ec) 
		{
            addressToIndex[entry] = i;
            AsmCommand* cmd = f.code.at(entry);

            if (!cmd || cmd->operation() != Operation::Call) 
			{
                break;
            }

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
            addressToIndex[entry] = i;
            AsmCommand* cmd = f.code.at(entry);

            if (!cmd || !(OperationIsPush(cmd->operation()) || cmd->operation() == Operation::Set)) 
			{
                break;
            }

            int shift = cmd->variable().nameIndex;
            entry += shift;

            if (!shift) 
			{
                break;
            }
        }
    }
}

GmStrgChunk::GmStrgChunk(const byte* raw, int off)
    : GmListChunk<StringEntry>(raw, off)
{}

std::string GmStrgChunk::get(int index) const
{
    return (*this)[index].data;
}

GmTxtrChunk::GmTxtrChunk(const byte* raw, int off)
    : GmChunk(raw, off)
{}

GmAudoChunk::GmAudoChunk(const byte* raw, int off)
    : GmChunk(raw, off)
{}

SpriteEntry::SpriteEntry(const byte* raw, int off)
    : SpriteEntry()
{
    PTR_LOOP_INIT();

    PTR_LOOP_READ(uint32_t, nameOffset)
    PTR_LOOP_READ(uint32_t, width);
    PTR_LOOP_READ(uint32_t, height);
    PTR_LOOP_READ(uint32_t, left);
    PTR_LOOP_READ(uint32_t, right);
    PTR_LOOP_READ(uint32_t, bottom);
    PTR_LOOP_READ(uint32_t, top);
    PTR_LOOP_READ_ARRAY(uint32_t, pad0, 3);
    PTR_LOOP_READ(uint32_t, bboxMode);
    PTR_LOOP_READ(uint32_t, separateMasks);
    PTR_LOOP_READ(uint32_t, originX);
    PTR_LOOP_READ(uint32_t, originY);
    PTR_LOOP_READ(uint32_t, textureCount);
    PTR_LOOP_READ(uint32_t, textureOffset);

    name = (const char*)(raw + nameOffset);
}

BackgroundEntry::BackgroundEntry(const byte* raw, int off)
    : BackgroundEntry()
{
    PTR_LOOP_INIT();

    PTR_LOOP_READ(uint32_t, nameOffset);
    PTR_LOOP_READ_ARRAY(uint32_t, pad0, 3);
    PTR_LOOP_READ(uint32_t, textureOffset);

    name = (const char*)(raw + nameOffset);
}

PathEntry::PathEntry(const byte* raw, int off)
    : PathEntry()
{
    PTR_LOOP_INIT();

    PTR_LOOP_READ(uint32_t, nameOffset);
    PTR_LOOP_READ(uint32_t, isSmooth);
    PTR_LOOP_READ(uint32_t, isClosed);
    PTR_LOOP_READ(uint32_t, precision);

    uint32_t pts;
    PTR_LOOP_READ(uint32_t, pts);
    points.resize(pts);
    PTR_LOOP_READ_ARRAY(PathPoint, points, pts);

    name = (const char*)(raw + nameOffset);
}

ScriptDefEntry::ScriptDefEntry(const byte* raw, int off)
    : ScriptDefEntry()
{
    PTR_LOOP_INIT();

    PTR_LOOP_READ(uint32_t, nameOffset);
    PTR_LOOP_READ(uint32_t, codeID);

    name = (const char*)(raw + nameOffset);
}

ScriptEntry::ScriptEntry(const byte* raw, int off)
    : ScriptEntry()
{
    offset = off;
    int32_t dl, shift, nameOffset;
    PTR_LOOP_INIT();

    PTR_LOOP_READ(uint32_t, nameOffset);
    PTR_LOOP_READ(int32_t, dl);
    PTR_LOOP_READ(int32_t, shift); // Waste this value
    PTR_LOOP_READ(int32_t, shift);
    PTR_LOOP_SHIFT(shift);
    PTR_LOOP_SHIFT(-4);
    codeOffset = offset + shift;

    std::vector<byte> bytecode(dl);
    PTR_LOOP_READ_ARRAY(byte, bytecode, dl);

    name = (const char*)(raw + nameOffset);

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

GlobalvarEntry::GlobalvarEntry(const byte*, int)
    : GlobalvarEntry()
{}

ShaderEntry::ShaderEntry(const byte* raw, int off)
    : ShaderEntry()
{
    PTR_LOOP_INIT();
    PTR_LOOP_READ(uint32_t, nameOffset);
    name = (const char*)(raw + nameOffset);
}

ObjectEntry::ObjectEntry(const byte* raw, int off)
    : ObjectEntry()
{
    PTR_LOOP_INIT();
    PTR_LOOP_READ(uint32_t, nameOffset);
    name = (const char*)(raw + nameOffset);
}

RoomEntry::RoomEntry(const byte* raw, int off)
    : RoomEntry()
{
    PTR_LOOP_INIT();
    PTR_LOOP_READ(uint32_t, nameOffset);
    name = (const char*)(raw + nameOffset);
}

TextureEntry::TextureEntry(const byte*, int)
    : TextureEntry()
{}

StringEntry::StringEntry(const byte* raw, int off)
    : StringEntry()
{
    PTR_LOOP_INIT();

    uint32_t length;
    PTR_LOOP_READ(uint32_t, length);

    data.resize(length);
    PTR_LOOP_READ_ARRAY(char, data, length);
}

ConstEntry::ConstEntry(const byte*, int)
    : ConstEntry()
{}

SoundEntry::SoundEntry(const byte* raw, int off)
{
    PTR_LOOP_INIT();

    PTR_LOOP_READ(uint32_t, nameOffset);
    PTR_LOOP_READ(uint32_t, flags);
    PTR_LOOP_READ(uint32_t, typeOffset);
    PTR_LOOP_READ(uint32_t, fileOffset);
    PTR_LOOP_READ(uint32_t, pad0);
    PTR_LOOP_READ(float, volume);
    PTR_LOOP_READ(float, pitch);
    PTR_LOOP_READ(int32_t, groupID);
    PTR_LOOP_READ(int32_t, audioID);

    name = (const char*)(raw + nameOffset);
}

TimelineEntry::TimelineEntry(const byte* raw, int off)
    : TimelineEntry()
{
    PTR_LOOP_INIT();
    PTR_LOOP_READ(uint32_t, nameOffset);
    name = (const char*)(raw + nameOffset);
}

FunctionDefEntry::FunctionDefEntry(const byte* raw, int off)
{
    PTR_LOOP_INIT();
    PTR_LOOP_READ(uint32_t, nameOffset);
    PTR_LOOP_READ(uint32_t, occurrenceCount);
    PTR_LOOP_READ(uint32_t, firstOccurrence);

    name = (const char*)(raw + nameOffset);
}

FontEntry::FontEntry(const byte* raw, int off)
    : FontEntry()
{
    PTR_LOOP_INIT();
    PTR_LOOP_READ(uint32_t, nameOffset);

    name = (const char*)(raw + nameOffset);
}

VariableDefEntry::VariableDefEntry(const byte* raw, int off)
{
    PTR_LOOP_INIT();

    uint32_t nameOffset;
    int32_t unknown;

    PTR_LOOP_READ(uint32_t, nameOffset);
    PTR_LOOP_READ(int32_t, unknown);
    PTR_LOOP_READ(int32_t, unknown);
    PTR_LOOP_READ(uint32_t, occurrenceCount);
    PTR_LOOP_READ(uint32_t, firstOccurrence);

    UNREFERENCED_PARAMETER(unknown);

    name = (const char*)(raw + nameOffset);
}

LocalVariableDefEntry::LocalVariableDefEntry(const byte* raw, int off)
{
    PTR_LOOP_INIT();

    int tmp, nameOffset;
    PTR_LOOP_READ(int32_t, tmp);
    PTR_LOOP_READ(int32_t, nameOffset);

    name = (const char*)(raw + nameOffset);
    type = static_cast<VariableType>(tmp);
}

LocalScriptDefEntry::LocalScriptDefEntry(const byte* raw, int off)
{
    PTR_LOOP_INIT();

    int localsCount, nameOffset;
    PTR_LOOP_READ(int32_t, localsCount);
    PTR_LOOP_READ(int32_t, nameOffset);

    refVar.reserve(localsCount);
    int inner_off = PTR_LOOP_OFFSET();

    for (int i = 0; i < localsCount; ++i) 
	{
        refVar.push_back(LocalVariableDefEntry(raw, inner_off));
        inner_off += LocalVariableDefEntry::static_size;
    }

    size = inner_off - off;
    name = (const char*)(raw + nameOffset);
}

const char* GmVariChunk::varName(int cmd_addr) const
{
    auto kv = addressToIndex.find(cmd_addr);
    return kv == addressToIndex.end() ? nullptr : refVar[kv->second].name;
}

const char* GmFuncChunk::funcName(int cmd_addr) const
{
    auto kv = addressToIndex.find(cmd_addr);
    return kv == addressToIndex.end() ? nullptr : refFunc[kv->second].name;
}
