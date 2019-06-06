#ifndef BASEBLOCK_H
#define BASEBLOCK_H

#include "asmcommand.h"
#include "gmform.h"


class GmAST;

class BaseBlock
{
public:
    BaseBlock();
    BaseBlock(const std::vector<AsmCommand>& p);

    int addr() const;
    int pastTheEndAddr() const;
    int jumpTargetAddr() const;
    bool hasJumpIfFalse() const;
    bool hasJumpIfTrue() const;
    bool hasJumpIf() const;
    bool hasJumpAlways() const;
    bool hasJump() const;
    bool isNumber() const;
    bool isNumber(int x) const;
    bool endsAsSwitchHeader() const;
    bool endsAsSwitchCase() const;
    bool endsWithExit() const;
    bool startsWithPop() const;
    bool isNop() const;
    bool valid() const;

    const auto begin() const { return code_.begin(); }
    const auto end() const { return code_.end(); }

private:
    std::vector<AsmCommand> code_;
};

#endif // BASEBLOCK_H
