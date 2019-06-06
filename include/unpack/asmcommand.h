#ifndef ASMCOMMAND_H
#define ASMCOMMAND_H

#include <vector>
#include <string>

#include "utils.h"


class GmForm;

enum class DataType 
{
    Double,
    Float,
    Int32,
    Int64,
    Bool,
    Variable,
    String,
    Instance,
    Int16 = 0x0f,
};

enum class Comparison 
{
    LT = 1,
    LE = 2,
    EQ = 3,
    NE = 4,
    GE = 5,
    GT = 6
};

enum class Operation 
{
    Nop     = 0x00,  // Fake NOP operation
    Save = 0xF5,  // Non-documented operation

    Conv    = 0x07,
    Mul     = 0x08,
    Div     = 0x09,
    Rem     = 0x0A,
    Mod     = 0x0B,
    Add     = 0x0C,
    Sub     = 0x0D,
    And     = 0x0E,
    Or      = 0x0F,
    Xor     = 0x10,
    Neg     = 0x11,
    Not     = 0x12,
    Shl     = 0x13,
    Shr     = 0x14,
    Cmp     = 0x15,
    Set     = 0x45,
    Dup     = 0x86,
    Ret     = 0x9C,
    Exit    = 0x9D,
    Pop     = 0x9E,
    Jmp     = 0xB6,
    JNZ     = 0xB7,
    JZ      = 0xB8,
    PushEnv = 0xBA,
    PopEnv  = 0xBB,
    PushCst = 0xC0,
    PushLoc = 0xC1,
    PushGlb = 0xC2,
    PushVar = 0xC3,
    PushI16 = 0x84,
    Call    = 0xD9,
    Break   = 0xFF,
};

enum class InstanceType 
{
    Local            = -7,
    Unknown1         = -6,
    Global           = -5,
    Noone            = -4,
    All              = -3,
    Other            = -2,
    Self             = -1,
    StackTopOrGlobal = 0,
    /* ID > 0, */
};

enum class VariableType 
{
    Array     = 0x00,
    StackTop  = 0x80,
    Normal    = 0xA0,
    RoomScope = 0xE0,
};

enum class SaveState 
{
    Addr      = 5,
    Index     = 6,
};

struct ScopedVariable
{
    InstanceType scope;
    VariableType type;
    uint32_t nameIndex;

    friend std::ostream& operator<< (std::ostream& out, const ScopedVariable& var);

    void printVarType(std::ostream& out) const;
};

class TypePair
{
public:
    TypePair();
    TypePair(byte p);

    DataType first() const;
    DataType second() const;

    friend std::ostream& operator<< (std::ostream& out, const TypePair& p);

private:
    byte pair_;
};


struct AsmCommand 
{
    static const uint32_t SaveStateHigh16;

    uint32_t data;
    uint32_t extra[2];
    int size, addr;
    std::string text;

    AsmCommand();
    AsmCommand(const byte* raw, int off);

    Operation operation() const;
    Comparison cmpType() const;
    TypePair typePair() const;
    DataType dataType() const;
    int16_t dataInt16() const;
    int32_t dataInt24() const;
    int32_t jumpAddr() const;
    int32_t dataInt32() const;
    int64_t dataInt64() const;
    float dataFloat() const;
    double dataDouble() const;
    ScopedVariable variable() const;
    std::string toString() const;

    void initText(const GmForm* f);

    friend std::ostream& operator<< (std::ostream& out, const AsmCommand& cmd);
};

std::vector<AsmCommand> Disassemble(const std::vector<byte>& bc);

bool OperationIsPush(Operation op);

const char* Operation2String(Operation op);
std::string Operation2PrettyString(Operation op);

const char* Comparison2String(Comparison c);

const char* DataType2String(DataType t);
std::string DataType2PrettyString(DataType t);

const char* InstanceType2String(InstanceType t);
std::string InstanceType2PrettyString(InstanceType t);

#endif // ASMCOMMAND_H
