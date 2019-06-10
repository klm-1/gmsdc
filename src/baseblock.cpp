#include "baseblock.h"

#include <stdexcept>
#include <cstdio>

#include "gmast.h"
#include "algext.h"


BaseBlock::BaseBlock()
    : code_()
{}

BaseBlock::BaseBlock(const std::vector<AsmCommand>& p)
    : code_(p)
{}

int BaseBlock::addr() const
{
    return code_.front().addr;
}

int BaseBlock::pastTheEndAddr() const
{
    const auto& c = code_.back();
    return c.addr + c.size;
}

bool BaseBlock::hasJumpIfTrue() const
{
    return code_.back().operation() == Operation::JNZ;
}

bool BaseBlock::hasJumpIfFalse() const
{
    return code_.back().operation() == Operation::JZ;
}

bool BaseBlock::hasJumpAlways() const
{
    return code_.back().operation() == Operation::Jmp;
}

bool BaseBlock::hasJumpIf() const
{
    return hasJumpIfTrue() || hasJumpIfFalse();
}

bool BaseBlock::hasJump() const
{
    return hasJumpAlways() || hasJumpIf();
}

int BaseBlock::jumpTargetAddr() const
{
    if (!hasJump())
	{
        throw std::runtime_error("Trying to get jump target of BB that has no jump");
    }
    return code_.back().jumpAddr();
}

bool BaseBlock::isNumber() const
{
    return code_.size() == 1
           && OperationIsPush(code_[0].operation());
}

bool BaseBlock::isNumber(int x) const
{
    return isNumber()
           && code_[0].dataInt16() == x;
}

bool BaseBlock::endsAsSwitchHeader() const
{
    return code_.size() > 4
           && endsAsSwitchCase();
}

bool BaseBlock::endsAsSwitchCase() const
{
    size_t n = code_.size();
    return n >= 4
           && code_[n - 2].operation() == Operation::Cmp
           && code_[n - 2].cmpType() == Comparison::EQ
           && OperationIsPush(code_[n - 3].operation())
           && code_[n - 4].operation() == Operation::Dup;
}

bool BaseBlock::endsWithExit() const
{
    return !code_.empty()
           && code_.back().operation() == Operation::Exit;
}

bool BaseBlock::startsWithPop() const
{
    return !code_.empty()
           && code_[0].operation() == Operation::Pop;
}

bool BaseBlock::isNop() const
{
    return code_.empty()
           || (code_.size() == 1 && hasJump());
}

bool BaseBlock::valid() const
{
    return !code_.empty();
}
