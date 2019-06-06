#include "asmcommand.h"

#include <cstdio>
#include <iomanip>
#include <stdexcept>

#include "gmform.h"
#include "algext.h"


const uint32_t AsmCommand::SaveStateHigh16      = 0x455f;

AsmCommand::AsmCommand()
    : data(0)
    , extra{0}
    , size(0)
    , addr(-1)
{}

AsmCommand::AsmCommand(const byte* raw, int off)
    : data(*(uint32_t*)(raw + off))
    , extra{0}
    , size(4)
    , addr(off)
{
    switch (operation()) 
	{

        /* TODO */
        case (Operation::Cmp):
        case (Operation::Conv):
        case (Operation::Mul):
        case (Operation::Div):
        case (Operation::Add):
        case (Operation::Sub):
        case (Operation::And):
        case (Operation::Or):
        case (Operation::Xor):
        case (Operation::Rem):
        case (Operation::Mod):
        case (Operation::Neg):
        case (Operation::Not):
        case (Operation::Shl):
        case (Operation::Shr):
        case (Operation::Ret):
        case (Operation::Exit):
        case (Operation::Pop):
        case (Operation::Jmp):
        case (Operation::JZ):
        case (Operation::JNZ):
        case (Operation::PushEnv):
        case (Operation::PopEnv):
        case (Operation::Break):
        case (Operation::Dup):
            break;

        /* Do nothing */
        case (Operation::PushI16):
            break;

        /* Push */
        case (Operation::PushCst):
        case (Operation::PushLoc):
        case (Operation::PushGlb):
        case (Operation::PushVar):
        case (Operation::Call):
        case (Operation::Set):
            if (dataType() != DataType::Int16) 
			{
                size += 4;
                extra[0] = *((uint32_t*)(raw + off) + 1);
                if (dataType() == DataType::Double || dataType() == DataType::Int64) 
				{
                    size += 4;
                    extra[1] = *((uint32_t*)(raw + off) + 2);
                }
            }
            break;

        /* Error */
        case (Operation::Nop):
        case (Operation::Save):
            break;
    }

    if (data >> 16 == SaveStateHigh16) 
	{
        data |= static_cast<uint32_t>(Operation::Save) << 24;
    }
}

TypePair::TypePair()
    : pair_(0)
{}

TypePair::TypePair(byte p)
    : pair_(p)
{}

DataType TypePair::first() const
{
    return DataType(pair_ & 0x0f);
}

DataType TypePair::second() const
{
    return DataType((pair_ >> 4) & 0x0f);
}

std::vector<AsmCommand> Disassemble(const std::vector<byte>& bc)
{
    std::vector<AsmCommand> out;

    for (size_t off = 0; off < bc.size();) 
	{
        AsmCommand current(bc.data(), off);
        out.push_back(current);
        off += current.size;
    }

    return std::move(out);
}

const char* Operation2String(Operation op)
{
    static char tmp[64];

    switch (op) 
	{
		CASE_RETURN_SCOPED(Operation, Nop)
		CASE_RETURN_SCOPED(Operation, Conv)
		CASE_RETURN_SCOPED(Operation, Mul)
		CASE_RETURN_SCOPED(Operation, Div)
		CASE_RETURN_SCOPED(Operation, Rem)
		CASE_RETURN_SCOPED(Operation, Mod)
		CASE_RETURN_SCOPED(Operation, Add)
		CASE_RETURN_SCOPED(Operation, Sub)
		CASE_RETURN_SCOPED(Operation, And)
		CASE_RETURN_SCOPED(Operation, Or)
		CASE_RETURN_SCOPED(Operation, Xor)
		CASE_RETURN_SCOPED(Operation, Neg)
		CASE_RETURN_SCOPED(Operation, Not)
		CASE_RETURN_SCOPED(Operation, Shl)
		CASE_RETURN_SCOPED(Operation, Shr)
		CASE_RETURN_SCOPED(Operation, Cmp)
		CASE_RETURN_SCOPED(Operation, Set)
		CASE_RETURN_SCOPED(Operation, Dup)
		CASE_RETURN_SCOPED(Operation, Ret)
		CASE_RETURN_SCOPED(Operation, Exit)
		CASE_RETURN_SCOPED(Operation, Pop)
		CASE_RETURN_SCOPED(Operation, Jmp)
		CASE_RETURN_SCOPED(Operation, JZ)
		CASE_RETURN_SCOPED(Operation, JNZ)
		CASE_RETURN_SCOPED(Operation, PushEnv)
		CASE_RETURN_SCOPED(Operation, PopEnv)
		CASE_RETURN_SCOPED(Operation, PushCst)
		CASE_RETURN_SCOPED(Operation, PushLoc)
		CASE_RETURN_SCOPED(Operation, PushGlb)
		CASE_RETURN_SCOPED(Operation, PushVar)
		CASE_RETURN_SCOPED(Operation, PushI16)
		CASE_RETURN_SCOPED(Operation, Call)
		CASE_RETURN_SCOPED(Operation, Break)
		CASE_RETURN_SCOPED(Operation, Save)
    }

    sprintf(tmp, "<%x>", static_cast<unsigned>(op));
    return tmp;
}

std::string Operation2PrettyString(Operation op)
{
    switch (op) 
	{
        case (Operation::Add):
        case (Operation::And):
        case (Operation::Call):
        case (Operation::Cmp):
        case (Operation::Conv):
        case (Operation::Div):
        case (Operation::Dup):
        case (Operation::Exit):
        case (Operation::Jmp):
        case (Operation::JNZ):
        case (Operation::JZ):
        case (Operation::Mod):
        case (Operation::Mul):
        case (Operation::Neg):
        case (Operation::Nop):
        case (Operation::Not):
        case (Operation::Or):
        case (Operation::Pop):
        case (Operation::Rem):
        case (Operation::Ret):
        case (Operation::Shl):
        case (Operation::Shr):
        case (Operation::Sub):
        case (Operation::Set):
        case (Operation::Xor):
        case (Operation::Save):
        case (Operation::Break): 
			{
                std::string ret(Operation2String(op));
                string_lower(ret);
                return ret;
            }
            break;

        case (Operation::PopEnv):
            return "pop.env";

        case (Operation::PushCst):
            return "push.const";

        case (Operation::PushEnv):
            return "push.env";

        case (Operation::PushGlb):
            return "push.glob";

        case (Operation::PushI16):
            return "push.int16";

        case (Operation::PushLoc):
            return "push.local";

        case (Operation::PushVar):
            return "push.var";

    }

    return "#ERR";
}

const char* Comparison2String(Comparison c)
{
    switch (c) 
	{
        case (Comparison::LT):
            return "<";
        case (Comparison::LE):
            return "<=";
        case (Comparison::EQ):
            return "==";
        case (Comparison::NE):
            return "!=";
        case (Comparison::GE):
            return ">=";
        case (Comparison::GT):
            return ">";
    }
    return "???";
}

const char* DataType2String(DataType t)
{
    switch (t) 
	{
		CASE_RETURN_SCOPED(DataType, Double);
		CASE_RETURN_SCOPED(DataType, Float);
		CASE_RETURN_SCOPED(DataType, Int16);
		CASE_RETURN_SCOPED(DataType, Int32);
		CASE_RETURN_SCOPED(DataType, Int64);
		CASE_RETURN_SCOPED(DataType, Bool);
		CASE_RETURN_SCOPED(DataType, Variable);
		CASE_RETURN_SCOPED(DataType, String);
		CASE_RETURN_SCOPED(DataType, Instance);
    }
    return "???";
}

std::string DataType2PrettyString(DataType t)
{
    if (t == DataType::Variable) 
	{
        return "var";
    }

    std::string ret(DataType2String(t));
    string_lower(ret);
    return ret;
}

const char* InstanceType2String(InstanceType t)
{
    static char tmp[32];
    switch (t) 
	{
		CASE_RETURN_SCOPED(InstanceType, StackTopOrGlobal);
		CASE_RETURN_SCOPED(InstanceType, Self);
		CASE_RETURN_SCOPED(InstanceType, Other);
		CASE_RETURN_SCOPED(InstanceType, All);
		CASE_RETURN_SCOPED(InstanceType, Noone);
		CASE_RETURN_SCOPED(InstanceType, Global);
		CASE_RETURN_SCOPED(InstanceType, Unknown1);
		CASE_RETURN_SCOPED(InstanceType, Local);
    }
    sprintf(tmp, "inst_%d", (int)t);
    return tmp;
}

std::string InstanceType2PrettyString(InstanceType t)
{
    switch (t) 
	{
        case (InstanceType::All):
        case (InstanceType::Self):
        case (InstanceType::Other):
        case (InstanceType::Noone):
        case (InstanceType::Global):
        case (InstanceType::Local): 
			{
                std::string ret(InstanceType2String(t));
                string_lower(ret);
                return ret;
            }
            break;

        case (InstanceType::StackTopOrGlobal):
            return "stog";

        case (InstanceType::Unknown1):
            return "UNKNOWN";
    }
    return std::to_string((int)t);
}

Operation AsmCommand::operation() const
{
    return Operation((data >> 24) & 0xff);
}

Comparison AsmCommand::cmpType() const
{
    return Comparison((data >> 8) & 0xff);
}

TypePair AsmCommand::typePair() const
{
    return TypePair((data >> 16) & 0xff);
}

DataType AsmCommand::dataType() const
{
    return typePair().first();
}

int16_t AsmCommand::dataInt16() const
{
    return data & 0xffff;
}

int32_t AsmCommand::dataInt32() const
{
    return *extra;
}

int AsmCommand::dataInt24() const
{
    int ret = data & 0x00ffffff;
    /* Hack! */
    if (ret >> 16 == 0x7f) 
	{
        ret |= 0xff800000;
    }
    return ret;
}

ScopedVariable AsmCommand::variable() const
{
    return ScopedVariable{
        InstanceType((int16_t)(data & 0xffff)),
        VariableType((extra[0] >> 24) & 0xff),
        (extra[0] & 0x00ffffff)
    };
}

int64_t AsmCommand::dataInt64() const
{
    return *(uint64_t*)extra;
}

float AsmCommand::dataFloat() const
{
    return *(float*)extra;
}

double AsmCommand::dataDouble() const
{
    return *(double*)extra;
}

std::ostream& operator<< (std::ostream& out, const TypePair& p)
{
    return out << DataType2PrettyString(p.first()) << ":" << DataType2PrettyString(p.second());
}

std::ostream& operator<< (std::ostream& out, const ScopedVariable& var)
{
    out << InstanceType2PrettyString(var.scope) << "." << var.nameIndex;
    var.printVarType(out);
    return out;
}

int32_t AsmCommand::jumpAddr() const
{
    return addr + dataInt24() * 4;
}

bool OperationIsPush(Operation op)
{
    return
        op == Operation::PushCst ||
        op == Operation::PushGlb ||
        op == Operation::PushI16 ||
        op == Operation::PushLoc ||
        op == Operation::PushVar;
}

std::string AsmCommand::toString() const
{
    return text;
}

void AsmCommand::initText(const GmForm* f)
{
    char tmp[16];
    sprintf(tmp, "0x%08x", addr);
    std::ostringstream out;
    out << tmp << ": " << Operation2PrettyString(operation()) << " ";

    switch (operation()) 
	{
        case (Operation::Nop):
            /* Error */
            break;

        case (Operation::Cmp):
            out << Comparison2String(cmpType()) << " " << typePair();
            break;

        case (Operation::Conv):
        case (Operation::Mul):
        case (Operation::Div):
        case (Operation::Add):
        case (Operation::Sub):
        case (Operation::And):
        case (Operation::Or):
        case (Operation::Rem):
        case (Operation::Not):
        case (Operation::Xor):
        case (Operation::Mod):
        case (Operation::Shl):
        case (Operation::Shr):
            out << typePair();
            break;

        case (Operation::Set):
            out << typePair() << " " << InstanceType2PrettyString(variable().scope) << ".";
            if (f) 
			{
                out << f->nameByAddr(addr);
            } 
			else 
			{
                out << variable().nameIndex;
            }
            variable().printVarType(out);
            break;

        case (Operation::PushI16):
        case (Operation::PushCst):
        case (Operation::PushLoc):
        case (Operation::PushGlb):
        case (Operation::PushVar):
            out << DataType2PrettyString(dataType()) << " ";
            switch (dataType()) 
			{
                case (DataType::Int16):
                    out << dataInt16();
                    break;

                case (DataType::Double):
                    out << std::setprecision(16) << dataDouble();
                    break;

                case (DataType::Int64):
                    out << dataInt64();
                    break;

                case (DataType::Float):
                    out << dataFloat();
                    break;

                case (DataType::String):
                    out << '"' << f->strings.get(dataInt32()) << '"';
                    break;

                case (DataType::Int32):
                case (DataType::Bool):
                case (DataType::Instance):
                    out << dataInt32();
                    break;

                case (DataType::Variable):
                    out << InstanceType2PrettyString(variable().scope) << ".";
                    if (f) {
                        out << f->nameByAddr(addr);
                    } else {
                        out << variable().nameIndex;
                    }
                    variable().printVarType(out);
                    break;
            }
            break;

        case (Operation::Neg):
        case (Operation::Ret):
        case (Operation::Dup):
            out << DataType2PrettyString(dataType());
            break;

        case (Operation::Exit):
        case (Operation::Pop):
        case (Operation::PushEnv):
        case (Operation::PopEnv):
        case (Operation::Break):
            break;

        case (Operation::Save):
            out << "addr";
            if (dataInt16() == static_cast<int16_t>(SaveState::Index)) 
			{
                out << ",index";
            }
            break;

        case (Operation::Call):
            if (f) 
			{
                out << f->nameByAddr(addr);
            } 
			else 
			{
                out << "func_" << dataInt32();
            }
            out << "[" << dataInt16() << "]";
            break;

        case (Operation::Jmp):
        case (Operation::JZ):
        case (Operation::JNZ):
            sprintf(tmp, "0x%08x", jumpAddr());
            out << tmp;
            break;
    }

    text = std::move(out.str());
}

void ScopedVariable::printVarType(std::ostream& out) const
{
    switch (type) 
	{
        case (VariableType::Array):
            out << "[]";
            break;

        case (VariableType::StackTop):
            out << "*";
            break;

        case (VariableType::RoomScope):
            out << "&";
            break;

        case (VariableType::Normal):
            break;
    }
}
