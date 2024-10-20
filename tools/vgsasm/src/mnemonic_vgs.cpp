#include "common.h"
#include "mnemonic.h"

static void HAGe_calc(LineData* line, uint8_t port, uint8_t out)
{
    if (mnemonic_format_test(line, 2, TokenType::Operand)) {
        switch (operandTable[line->token[1].second]) {
            case Operand::BC:
                ML_PUSH_AF;
                ML_PUSH_HL;
                ML_LD_H_B;
                ML_LD_L_C;
                ML_LD_A_n(out);
                ML_OUT_A(port);
                ML_LD_B_H;
                ML_LD_C_L;
                ML_POP_HL;
                ML_POP_AF;
                return;
            case Operand::DE:
                ML_PUSH_AF;
                ML_PUSH_HL;
                ML_LD_H_D;
                ML_LD_L_E;
                ML_LD_A_n(out);
                ML_OUT_A(port);
                ML_LD_D_H;
                ML_LD_E_L;
                ML_POP_HL;
                ML_POP_AF;
                return;
            case Operand::HL:
                ML_PUSH_AF;
                ML_LD_A_n(out);
                ML_OUT_A(port);
                ML_POP_AF;
                return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Operand)) {
        if (operandTable[line->token[1].second] == Operand::HL) {
            switch (operandTable[line->token[3].second]) {
                case Operand::A:
                    ML_PUSH_AF;
                    ML_PUSH_BC;
                    ML_LD_C_A;
                    ML_LD_A_n(out | 0x80);
                    ML_OUT_A(port);
                    ML_POP_BC;
                    ML_POP_AF;
                    return;
                case Operand::B:
                    ML_PUSH_AF;
                    ML_PUSH_BC;
                    ML_LD_C_B;
                    ML_LD_A_n(out | 0x80);
                    ML_OUT_A(port);
                    ML_POP_BC;
                    ML_POP_AF;
                    return;
                case Operand::C:
                    ML_PUSH_AF;
                    ML_LD_A_n(out | 0x80);
                    ML_OUT_A(port);
                    ML_POP_AF;
                    return;
                case Operand::D:
                    ML_PUSH_AF;
                    ML_PUSH_BC;
                    ML_LD_C_D;
                    ML_LD_A_n(out | 0x80);
                    ML_OUT_A(port);
                    ML_POP_BC;
                    ML_POP_AF;
                    return;
                case Operand::E:
                    ML_PUSH_AF;
                    ML_PUSH_BC;
                    ML_LD_C_E;
                    ML_LD_A_n(out | 0x80);
                    ML_OUT_A(port);
                    ML_POP_BC;
                    ML_POP_AF;
                    return;
            }
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal " + line->token[0].second + " instruction.";
    }
}

static void HAGe_atan2(LineData* line)
{
    if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Operand) &&
        operandTable[line->token[1].second] == Operand::A) {
        switch (operandTable[line->token[3].second]) {
            case Operand::HL:
                ML_IN_A(0xC8);
                return;
            case Operand::BC:
                ML_PUSH_HL;
                ML_LD_H_B;
                ML_LD_L_C;
                ML_IN_A(0xC8);
                ML_POP_HL;
                return;
            case Operand::DE:
                ML_PUSH_HL;
                ML_LD_H_D;
                ML_LD_L_E;
                ML_IN_A(0xC8);
                ML_POP_HL;
                return;
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal " + line->token[0].second + " instruction.";
    }
}

static bool sincos(LineData* line, Operand op, uint8_t port)
{
    switch (op) {
        case Operand::A:
            ML_OUT_A(port);
            return true;
        case Operand::B:
            ML_LD_A_B;
            ML_OUT_A(port);
            return true;
        case Operand::C:
            ML_LD_A_C;
            ML_OUT_A(port);
            return true;
        case Operand::D:
            ML_LD_A_D;
            ML_OUT_A(port);
            return true;
        case Operand::E:
            ML_LD_A_E;
            ML_OUT_A(port);
            return true;
        case Operand::H:
            ML_LD_A_H;
            ML_OUT_A(port);
            return true;
        case Operand::L:
            ML_LD_A_L;
            ML_OUT_A(port);
            return true;
    }
    return false;
}

static void HAGe_sin(LineData* line)
{
    if (mnemonic_format_test(line, 2, TokenType::Operand)) {
        if (sincos(line, operandTable[line->token[1].second], 0xC6)) {
            return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Operand) &&
               operandTable[line->token[1].second] == Operand::A) {
        if (sincos(line, operandTable[line->token[3].second], 0xC6)) {
            return;
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal " + line->token[0].second + " instruction.";
    }
}

static void HAGe_cos(LineData* line)
{
    if (mnemonic_format_test(line, 2, TokenType::Operand)) {
        if (sincos(line, operandTable[line->token[1].second], 0xC7)) {
            return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Operand) &&
               operandTable[line->token[1].second] == Operand::A) {
        if (sincos(line, operandTable[line->token[3].second], 0xC7)) {
            return;
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal " + line->token[0].second + " instruction.";
    }
}

void mnemonic_MUL(LineData* line) { HAGe_calc(line, 0xC5, 0x00); }
void mnemonic_MULS(LineData* line) { HAGe_calc(line, 0xC5, 0x40); }
void mnemonic_DIV(LineData* line) { HAGe_calc(line, 0xC5, 0x01); }
void mnemonic_DIVS(LineData* line) { HAGe_calc(line, 0xC5, 0x41); }
void mnemonic_MOD(LineData* line) { HAGe_calc(line, 0xC5, 0x02); }
void mnemonic_ATN2(LineData* line) { HAGe_atan2(line); }
void mnemonic_SIN(LineData* line) { HAGe_sin(line); }
void mnemonic_COS(LineData* line) { HAGe_cos(line); }
