#include "common.h"
#include "mnemonic.h"

void mnemonic_PUSH(LineData* line)
{
    if (mnemonic_format_check(line, 2, TokenType::Operand)) {
        switch (operandTable[line->token[1].second]) {
            case Operand::BC: ML_PUSH_BC; break;
            case Operand::DE: ML_PUSH_DE; break;
            case Operand::HL: ML_PUSH_HL; break;
            case Operand::AF: ML_PUSH_AF; break;
            case Operand::IX: ML_PUSH_IX; break;
            case Operand::IY: ML_PUSH_IY; break;
            default:
                line->error = true;
                line->errmsg = "Unsupported PUSH operand: " + line->token[1].second;
        }
    }
}

void mnemonic_POP(LineData* line)
{
    if (mnemonic_format_check(line, 2, TokenType::Operand)) {
        switch (operandTable[line->token[1].second]) {
            case Operand::BC: ML_POP_BC; break;
            case Operand::DE: ML_POP_DE; break;
            case Operand::HL: ML_POP_HL; break;
            case Operand::AF: ML_POP_AF; break;
            case Operand::IX: ML_POP_IX; break;
            case Operand::IY: ML_POP_IY; break;
            default:
                line->error = true;
                line->errmsg = "Unsupported POP operand: " + line->token[1].second;
        }
    }
}
