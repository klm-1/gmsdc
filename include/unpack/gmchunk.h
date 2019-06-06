#ifndef GMCHUNK_H
#define GMCHUNK_H

#include <string>
#include <cstdint>
#include <vector>
#include <map>

#include "gmheader.h"
#include "utils.h"
#include "asmcommand.h"


#define PTR_LOOP_INIT() const byte* _data = raw + off;

#define PTR_LOOP_READ(_type, _var) \
    _var = *(_type*)_data; \
    _data += sizeof(_type);

#define PTR_LOOP_READ_ARRAY(_type, _var, _count) \
    for(int _i = 0; _i < (int)(_count); ++_i) { PTR_LOOP_READ(_type, _var[_i]) }

#define PTR_LOOP_OFFSET() ((int)(_data - raw))

#define PTR_LOOP_SHIFT(_delta) _data += _delta;


class GmForm;

/* **** *
 * Meta *
 * **** */
struct GmChunk {
    GmHeader header;
    int size, offset;

    GmChunk();
    GmChunk(const byte* raw, int off);

    std::string name() const;
};


template<class T>
struct GmList 
{
    std::vector<T> content;
    int size, offset;

    GmList()
        : content()
        , size(0)
        , offset(-1)
    {}

    GmList(const byte* raw, int off)
        : content()
        , size(0)
        , offset(off)
    {
        uint32_t* np = (uint32_t*)(raw + off);
        uint32_t n = *np;
        content.reserve(n);

        for (uint32_t i = 0; i < n; ++i) 
		{
            content.push_back(T(raw, np[i + 1]));
        }

        size = (n + 1) * sizeof(uint32_t) + n * sizeof(T);
    }

    T&       operator[](int i)       { return content[i]; }
    T const& operator[](int i) const { return content[i]; }
          auto begin()       { return content.begin(); }
    const auto begin() const { return content.begin(); }
          auto end()       { return content.end(); }
	const auto end() const { return content.end(); }
    int count() const { return content.size(); }
};


template<class T>
struct GmListChunk : GmChunk 
{
    GmList<T> content;

    GmListChunk()
        : GmChunk()
        , content()
    {}

    GmListChunk(const byte* raw, int off)
        : GmChunk(raw, off)
        , content(raw, off + HEADER_SIZE)
    {}

    T&       operator[](int i)       { return content[i]; }
    T const& operator[](int i) const { return content[i]; }
	      auto begin()       { return content.begin(); }
    const auto begin() const { return content.begin(); }
          auto end()       { return content.end(); }
    const auto end() const { return content.end(); }
    int count() const { return content.count(); }
};


/* ************ *
 * List entries *
 * ************ */
struct PathPoint 
{
    float x, y, speed;
};

struct SpriteEntry 
{
    uint32_t nameOffset;
    uint32_t width, height;
    uint32_t left, right, bottom, top;
    uint32_t pad0[3];
    uint32_t bboxMode;
    uint32_t separateMasks;
    uint32_t originX, originY;
    uint32_t textureCount;
    uint32_t textureOffset;

    const char* name;

    SpriteEntry() = default;
    SpriteEntry(const byte* raw, int off);
};

struct BackgroundEntry 
{
    uint32_t nameOffset;
    uint32_t pad0[3];
    uint32_t textureOffset;

    const char* name;

    BackgroundEntry() = default;
    BackgroundEntry(const byte* raw, int off);
};

struct PathEntry 
{
    uint32_t nameOffset;
    uint32_t isSmooth, isClosed;
    uint32_t precision;
    std::vector<PathPoint> points;

    const char* name;

    PathEntry() = default;
    PathEntry(const byte* raw, int off);
};

struct ScriptDefEntry 
{
    uint32_t nameOffset;
    uint32_t codeID;

    const char* name;

    ScriptDefEntry() = default;
    ScriptDefEntry(const byte* raw, int off);
};

struct ScriptEntry 
{
    using v_code_t = std::vector<AsmCommand>;

    const char* name;
    v_code_t code;
    uint32_t offset, codeOffset;

    ScriptEntry() = default;
    ScriptEntry(const byte* raw, int off);

    auto begin() { return code.begin(); }
    auto end() { return code.end(); }

    friend std::ostream& operator<< (std::ostream&, const ScriptEntry&);
};

struct GlobalvarEntry 
{
    GlobalvarEntry() = default;
    GlobalvarEntry(const byte* raw, int off);
};

struct ShaderEntry 
{
    uint32_t nameOffset;

    const char* name;

    ShaderEntry() = default;
    ShaderEntry(const byte* raw, int off);
};

struct ObjectEntry 
{
    uint32_t nameOffset;

    const char* name;

    ObjectEntry() = default;
    ObjectEntry(const byte* raw, int off);
};

struct RoomEntry 
{
    uint32_t nameOffset;

    const char* name;

    RoomEntry() = default;
    RoomEntry(const byte* raw, int off);
};

struct TextureEntry 
{
    TextureEntry() = default;
    TextureEntry(const byte* raw, int off);
};

struct StringEntry 
{
    std::string data;

    StringEntry() = default;
    StringEntry(const byte* raw, int off);
};

struct ConstEntry 
{
    uint32_t nameOffset;
    uint32_t valueOffset;

    ConstEntry() = default;
    ConstEntry(const byte* raw, int off);
};

struct SoundEntry 
{
    uint32_t nameOffset;
    uint32_t flags;
    uint32_t typeOffset;
    uint32_t fileOffset;
    uint32_t pad0;
    float volume;
    float pitch;
    int32_t groupID;
    int32_t audioID;

    const char* name;

    SoundEntry() = default;
    SoundEntry(const byte* raw, int off);
};

struct TimelineEntry 
{
    uint32_t nameOffset;

    const char* name;

    TimelineEntry() = default;
    TimelineEntry(const byte* raw, int off);
};

struct FunctionDefEntry 
{
    static const int static_size = 12;

    uint32_t nameOffset;
    uint32_t occurrenceCount;
    uint32_t firstOccurrence;

    const char* name;

    FunctionDefEntry() = default;
    FunctionDefEntry(const byte* raw, int off);
};

struct FontEntry 
{
    uint32_t nameOffset;

    const char* name;

    FontEntry() = default;
    FontEntry(const byte* raw, int off);
};

struct VariableDefEntry 
{
    static const int static_size = 20;

    const char* name;
    uint32_t occurrenceCount;
    uint32_t firstOccurrence;

    VariableDefEntry() = default;
    VariableDefEntry(const byte* raw, int off);
};

struct LocalVariableDefEntry 
{
    static const int static_size = 8;

    VariableType type;
    const char* name;

    LocalVariableDefEntry() = default;
    LocalVariableDefEntry(const byte* raw, int off);
};

struct LocalScriptDefEntry 
{
    int size;
    const char* name;
    std::vector<LocalVariableDefEntry> refVar;

    LocalScriptDefEntry() = default;
    LocalScriptDefEntry(const byte* raw, int off);
};


/* ***************** *
 * Chunks definition *
 * ***************** */
struct GmGen8Chunk : GmChunk 
{
    byte debug;
    byte bytecodeVersion;
    byte pad0[2];
    uint32_t filenameOffset;
    uint32_t configOffset;
    uint32_t pad1; // Last object?
    uint32_t pad2; // Last title?
    uint32_t gameID;
    uint32_t pad3[4];
    uint32_t nameOffset;
    uint32_t major, minor, release, build;
    uint32_t defaultWindowWidth;
    uint32_t defaultWindowHeight;
    uint32_t infoFlags;
    byte licenseMd5[16];
    uint32_t licenseCRC;
    uint64_t timestamp;
    uint32_t displayNameOffset;
    uint32_t activeTargets;
    uint32_t pad4[4];
    uint32_t steamAppID;
    uint32_t pad5;

    GmGen8Chunk() = default;
    GmGen8Chunk(const byte* raw, int off);
};

struct GmOptnChunk : GmChunk 
{
    uint32_t pad0[2];
    uint32_t infoFlags;
    uint32_t pad1[12];
    GmList<ConstEntry> constants;

    GmOptnChunk() = default;
    GmOptnChunk(const byte* raw, int off);
};

struct GmLangChunk : GmChunk 
{
    /* TBD */
    GmLangChunk() = default;
    GmLangChunk(const byte* raw, int off);
};

struct GmExtnChunk : GmChunk 
{
    /* TBD */
    GmExtnChunk() = default;
    GmExtnChunk(const byte* raw, int off);
};

struct GmSondChunk : GmListChunk<SoundEntry> 
{
    GmSondChunk() = default;
    GmSondChunk(const byte* raw, int off);
};

struct GmAgrpChunk : GmChunk 
{
    /* TBD */
    GmAgrpChunk() = default;
    GmAgrpChunk(const byte* raw, int off);
};

struct GmSprtChunk : GmListChunk<SpriteEntry> 
{
    GmSprtChunk() = default;
    GmSprtChunk(const byte* raw, int off);
};

struct GmBgndChunk : GmListChunk<BackgroundEntry> 
{
    GmBgndChunk() = default;
    GmBgndChunk(const byte* raw, int off);
};

struct GmPathChunk : GmListChunk<PathEntry> 
{
    GmPathChunk() = default;
    GmPathChunk(const byte* raw, int off);
};

struct GmScptChunk : GmListChunk<ScriptDefEntry> 
{
    GmScptChunk() = default;
    GmScptChunk(const byte* raw, int off);
};

struct GmGlobChunk : GmListChunk<GlobalvarEntry> 
{
    GmGlobChunk() = default;
    GmGlobChunk(const byte* raw, int off);
};

struct GmShdrChunk : GmListChunk<ShaderEntry> 
{
    GmShdrChunk() = default;
    GmShdrChunk(const byte* raw, int off);
};

struct GmFontChunk : GmListChunk<FontEntry> 
{
    /* TBD */
    GmFontChunk() = default;
    GmFontChunk(const byte* raw, int off);
};

struct GmTmlnChunk : GmListChunk<TimelineEntry> 
{
    /* TBD */
    GmTmlnChunk() = default;
    GmTmlnChunk(const byte* raw, int off);
};

struct GmObjtChunk : GmListChunk<ObjectEntry> 
{
    /* TBD */
    GmObjtChunk() = default;
    GmObjtChunk(const byte* raw, int off);
};

struct GmRoomChunk : GmListChunk<RoomEntry> 
{
    /* TBD */
    GmRoomChunk() = default;
    GmRoomChunk(const byte* raw, int off);
};

struct GmDaflChunk : GmChunk 
{
    /* TBD */
    GmDaflChunk() = default;
    GmDaflChunk(const byte* raw, int off);
};

struct GmTpagChunk : GmListChunk<TextureEntry> 
{
    /* TBD */
    GmTpagChunk() = default;
    GmTpagChunk(const byte* raw, int off);
};

struct GmCodeChunk : GmListChunk<ScriptEntry> 
{
    AsmCommand* at(int off);
    void postInit(GmForm& f);

    GmCodeChunk() = default;
    GmCodeChunk(const byte* raw, int off);
};

struct GmVariChunk : GmChunk 
{
    std::vector<VariableDefEntry> refVar;
    std::map<int, int> addressToIndex;

    void postInit(GmForm& f);
    const char* varName(int cmd_addr) const;

    GmVariChunk() = default;
    GmVariChunk(const byte* raw, int off);
};

struct GmFuncChunk : GmChunk 
{
    std::vector<FunctionDefEntry> refFunc;
    std::vector<LocalScriptDefEntry> refScript;
    std::map<int, int> addressToIndex;

    void postInit(GmForm& f);
    const char* funcName(int cmd_addr) const;

    GmFuncChunk() = default;
    GmFuncChunk(const byte* raw, int off);
};

struct GmStrgChunk : GmListChunk<StringEntry> 
{
    GmStrgChunk() = default;
    GmStrgChunk(const byte* raw, int off);

    std::string get(int index) const;
};

struct GmTxtrChunk : GmChunk 
{
    /* TBD */
    GmTxtrChunk() = default;
    GmTxtrChunk(const byte* raw, int off);
};

struct GmAudoChunk : GmChunk 
{
    /* TBD */
    GmAudoChunk() = default;
    GmAudoChunk(const byte* raw, int off);
};

#endif // GMCHUNK_H
