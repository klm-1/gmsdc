#ifndef GMCHUNK_H
#define GMCHUNK_H

#include <string>
#include <cstdint>
#include <vector>
#include <map>

#include "gmheader.h"
#include "utils.h"
#include "asmcommand.h"
#include "binaryreader.h"


class GmForm;
class BinaryReader;

/* ****** *
 * Common *
 * ****** */
struct GmChunk
{
    int start;
    GmHeader header;
    int size;

    GmChunk(BinaryReader& br, SectionHeader hdr);

    std::string const& name() const;
    int pastTheEndAddr();
};


template<class T>
struct GmPointerList
{
    std::vector<T> content;
    int size;

    GmPointerList()
        : content()
        , size(0)
    {}

    explicit GmPointerList(BinaryReader& br)
        : content()
        , size(0)
    {
        uint32_t n = br.read<uint32_t>();

        uint32_t pointers[n];
        br.read<uint32_t>(pointers, n);

        content.reserve(n);

        for (uint32_t i = 0; i < n; ++i)
		{
		    br.seek(pointers[i]);
            content.push_back(T(br));
        }

        size = (n + 1) * sizeof(uint32_t) + n * sizeof(T);
    }

    T&       operator[](int i)       { return content[i]; }
    T const& operator[](int i) const { return content[i]; }
    auto begin()       { return content.begin(); }
    auto begin() const { return content.begin(); }
    auto end()         { return content.end(); }
	auto end()   const { return content.end(); }
    int count()  const { return content.size(); }
};


template<class T>
struct GmListChunk : GmChunk
{
    GmPointerList<T> content;

    GmListChunk(BinaryReader& br, SectionHeader hdr)
        : GmChunk(br, hdr)
        , content(br)
    {}

    T&       operator[](int i)       { return content[i]; }
    T const& operator[](int i) const { return content[i]; }
	auto begin()       { return content.begin(); }
    auto begin() const { return content.begin(); }
    auto end()         { return content.end(); }
    auto end()   const { return content.end(); }
    int count()  const { return content.count(); }
};


/* ************ *
 * List entries *
 * ************ */
struct PathPoint
{
    float x, y, speed;

    PathPoint(BinaryReader& br);
};

struct SpriteEntry
{
    std::string name;
    uint32_t width, height;
    uint32_t maskLeft, maskRight, maskBottom, maskTop;
    uint32_t bboxMode;
    uint32_t separateMasks;
    uint32_t originX, originY;
    uint32_t textureCount;
    uint32_t textureOffset;

    explicit SpriteEntry(BinaryReader& br);
};

struct BackgroundEntry
{
    std::string name;
    uint32_t textureOffset;

    explicit BackgroundEntry(BinaryReader& br);
};

struct PathEntry
{
    std::string name;
    uint32_t isSmooth, isClosed;
    uint32_t precision;
    std::vector<PathPoint> points;

    PathEntry(BinaryReader& br);
};

struct ScriptDefEntry
{
    std::string name;
    uint32_t codeID;

    explicit ScriptDefEntry(BinaryReader& br);
};

struct ScriptEntry
{
    using v_code_t = std::vector<AsmCommand>;

    std::string name;
    v_code_t code;
    uint32_t offset, codeOffset, codeSize;

    explicit ScriptEntry(BinaryReader& br);

    auto begin() { return code.begin(); }
    auto end()   { return code.end(); }

    friend std::ostream& operator<< (std::ostream&, ScriptEntry const&);
};

struct GlobalvarEntry
{
    explicit GlobalvarEntry(BinaryReader& br);
};

struct ShaderEntry
{
    std::string name;

    explicit ShaderEntry(BinaryReader& br);
};

struct ObjectEntry
{
    std::string name;

    explicit ObjectEntry(BinaryReader& br);
};

struct RoomEntry
{
    std::string name;

    explicit RoomEntry(BinaryReader& br);
};

struct TextureEntry
{
    explicit TextureEntry(BinaryReader& br);
};

struct StringEntry
{
    std::string data;

    explicit StringEntry(BinaryReader& br);
};

struct ConstEntry
{
    std::string name, value;

    explicit ConstEntry(BinaryReader& br);
};

struct SoundEntry
{
    std::string name;
    uint32_t flags;
    uint32_t typeOffset;
    uint32_t fileOffset;
    float volume;
    float pitch;
    int32_t groupID;
    int32_t audioID;

    explicit SoundEntry(BinaryReader& br);
};

struct TimelineEntry
{
    std::string name;

    explicit TimelineEntry(BinaryReader& br);
};

struct FunctionDefEntry
{
    static const int static_size = 12;

    std::string name;
    uint32_t occurrenceCount;
    uint32_t firstOccurrence;

    explicit FunctionDefEntry(BinaryReader& br);
};

struct FontEntry
{
    std::string name;

    explicit FontEntry(BinaryReader& br);
};

struct VariableDefEntry
{
    static const int static_size = 20;

    std::string name;
    int32_t occurrenceCount;
    int32_t firstOccurrence;

    explicit VariableDefEntry(BinaryReader& br);
};

struct LocalVariableDefEntry
{
    static const int static_size = 8;

    VariableType type;
    std::string name;

    explicit LocalVariableDefEntry(BinaryReader& br);
};

struct LocalScriptDefEntry
{
    int size;
    std::string name;
    std::vector<LocalVariableDefEntry> refVar;

    explicit LocalScriptDefEntry(BinaryReader& br);
};


/* ***************** *
 * Chunks definition *
 * ***************** */
struct GmGen8Chunk : GmChunk
{
    byte debug;
    byte bytecodeVersion;
    std::string filename;
    std::string confname;
    uint32_t gameID;
    std::string name;
    uint32_t major, minor, release, build;
    uint32_t defaultWindowWidth;
    uint32_t defaultWindowHeight;
    uint32_t infoFlags;
    uint64_t timestamp;
    std::string dispname;
    uint32_t activeTargets;
    uint32_t steamAppID;

    explicit GmGen8Chunk(BinaryReader& br);
};

struct GmOptnChunk : GmChunk
{
    uint32_t infoFlags;
    GmPointerList<ConstEntry> constants;

    explicit GmOptnChunk(BinaryReader& br);
};

struct GmLangChunk : GmChunk
{
    /* TBD */
    explicit GmLangChunk(BinaryReader& br);
};

struct GmExtnChunk : GmChunk
{
    /* TBD */
    explicit GmExtnChunk(BinaryReader& br);
};

struct GmSondChunk : GmListChunk<SoundEntry>
{
    explicit GmSondChunk(BinaryReader& br);
};

struct GmAgrpChunk : GmChunk
{
    /* TBD */
    explicit GmAgrpChunk(BinaryReader& br);
};

struct GmSprtChunk : GmListChunk<SpriteEntry>
{
    explicit GmSprtChunk(BinaryReader& br);
};

struct GmBgndChunk : GmListChunk<BackgroundEntry>
{
    explicit GmBgndChunk(BinaryReader& br);
};

struct GmPathChunk : GmListChunk<PathEntry>
{
    explicit GmPathChunk(BinaryReader& br);
};

struct GmScptChunk : GmListChunk<ScriptDefEntry>
{
    explicit GmScptChunk(BinaryReader& br);
};

struct GmGlobChunk : GmListChunk<GlobalvarEntry>
{
    explicit GmGlobChunk(BinaryReader& br);
};

struct GmShdrChunk : GmListChunk<ShaderEntry>
{
    explicit GmShdrChunk(BinaryReader& br);
};

struct GmFontChunk : GmListChunk<FontEntry>
{
    /* TBD */
    explicit GmFontChunk(BinaryReader& br);
};

struct GmTmlnChunk : GmListChunk<TimelineEntry>
{
    /* TBD */
    explicit GmTmlnChunk(BinaryReader& br);
};

struct GmObjtChunk : GmListChunk<ObjectEntry>
{
    /* TBD */
    explicit GmObjtChunk(BinaryReader& br);
};

struct GmRoomChunk : GmListChunk<RoomEntry>
{
    /* TBD */
    explicit GmRoomChunk(BinaryReader& br);
};

struct GmDaflChunk : GmChunk
{
    /* TBD */
    explicit GmDaflChunk(BinaryReader& br);
};

struct GmTpagChunk : GmListChunk<TextureEntry>
{
    /* TBD */
    explicit GmTpagChunk(BinaryReader& br);
};

struct GmCodeChunk : GmListChunk<ScriptEntry>
{
    AsmCommand* at(int off);
    void postInit(GmForm& f);

    explicit GmCodeChunk(BinaryReader& br);
};

struct GmVariChunk : GmChunk
{
    std::vector<VariableDefEntry> refVar;

    void postInit(GmForm& f);

    explicit GmVariChunk(BinaryReader& br);
};

struct GmFuncChunk : GmChunk
{
    std::vector<FunctionDefEntry> refFunc;
    std::vector<LocalScriptDefEntry> refScript;

    void postInit(GmForm& f);

    explicit GmFuncChunk(BinaryReader& br);
};

struct GmStrgChunk : GmListChunk<StringEntry>
{
    explicit GmStrgChunk(BinaryReader& br);

    std::string const& get(int index) const;
};

struct GmTxtrChunk : GmChunk
{
    /* TBD */
    explicit GmTxtrChunk(BinaryReader& br);
};

struct GmAudoChunk : GmChunk
{
    /* TBD */
    explicit GmAudoChunk(BinaryReader& br);
};

#endif // GMCHUNK_H
