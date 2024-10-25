/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"
#include "mnemonic.h"

static void HAGe_calc(LineData* line, uint8_t port, uint8_t out)
{
    if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Operand)) {
        auto op1 = operandTable[line->token[1].second];
        auto op2 = operandTable[line->token[3].second];
        if (op1 == Operand::HL) {
            switch (op2) {
                case Operand::A: ML_HAG_HL_A(port, out); return;
                case Operand::B: ML_HAG_HL_B(port, out); return;
                case Operand::C: ML_HAG_HL_C(port, out); return;
                case Operand::D: ML_HAG_HL_D(port, out); return;
                case Operand::E: ML_HAG_HL_E(port, out); return;
            }
        }
        switch (op1) {
            case Operand::A:
                switch (op2) {
                    case Operand::A: ML_HAG_A_A(port, out); return;
                    case Operand::B: ML_HAG_A_B(port, out); return;
                    case Operand::C: ML_HAG_A_C(port, out); return;
                    case Operand::D: ML_HAG_A_D(port, out); return;
                    case Operand::E: ML_HAG_A_E(port, out); return;
                    case Operand::H: ML_HAG_A_H(port, out); return;
                    case Operand::L: ML_HAG_A_L(port, out); return;
                }
                break;
            case Operand::B:
                switch (op2) {
                    case Operand::A: ML_HAG_B_A(port, out); return;
                    case Operand::B: ML_HAG_B_B(port, out); return;
                    case Operand::C: ML_HAG_B_C(port, out); return;
                    case Operand::D: ML_HAG_B_D(port, out); return;
                    case Operand::E: ML_HAG_B_E(port, out); return;
                    case Operand::H: ML_HAG_B_H(port, out); return;
                    case Operand::L: ML_HAG_B_L(port, out); return;
                }
                break;
            case Operand::C:
                switch (op2) {
                    case Operand::A: ML_HAG_C_A(port, out); return;
                    case Operand::B: ML_HAG_C_B(port, out); return;
                    case Operand::C: ML_HAG_C_C(port, out); return;
                    case Operand::D: ML_HAG_C_D(port, out); return;
                    case Operand::E: ML_HAG_C_E(port, out); return;
                    case Operand::H: ML_HAG_C_H(port, out); return;
                    case Operand::L: ML_HAG_C_L(port, out); return;
                }
                break;
            case Operand::D:
                switch (op2) {
                    case Operand::A: ML_HAG_D_A(port, out); return;
                    case Operand::B: ML_HAG_D_B(port, out); return;
                    case Operand::C: ML_HAG_D_C(port, out); return;
                    case Operand::D: ML_HAG_D_D(port, out); return;
                    case Operand::E: ML_HAG_D_E(port, out); return;
                    case Operand::H: ML_HAG_D_H(port, out); return;
                    case Operand::L: ML_HAG_D_L(port, out); return;
                }
                break;
            case Operand::E:
                switch (op2) {
                    case Operand::A: ML_HAG_E_A(port, out); return;
                    case Operand::B: ML_HAG_E_B(port, out); return;
                    case Operand::C: ML_HAG_E_C(port, out); return;
                    case Operand::D: ML_HAG_E_D(port, out); return;
                    case Operand::E: ML_HAG_E_E(port, out); return;
                    case Operand::H: ML_HAG_E_H(port, out); return;
                    case Operand::L: ML_HAG_E_L(port, out); return;
                }
                break;
            case Operand::H:
                switch (op2) {
                    case Operand::A: ML_HAG_H_A(port, out); return;
                    case Operand::B: ML_HAG_H_B(port, out); return;
                    case Operand::C: ML_HAG_H_C(port, out); return;
                    case Operand::D: ML_HAG_H_D(port, out); return;
                    case Operand::E: ML_HAG_H_E(port, out); return;
                    case Operand::H: ML_HAG_H_H(port, out); return;
                    case Operand::L: ML_HAG_H_L(port, out); return;
                }
                break;
            case Operand::L:
                switch (op2) {
                    case Operand::A: ML_HAG_L_A(port, out); return;
                    case Operand::B: ML_HAG_L_B(port, out); return;
                    case Operand::C: ML_HAG_L_C(port, out); return;
                    case Operand::D: ML_HAG_L_D(port, out); return;
                    case Operand::E: ML_HAG_L_E(port, out); return;
                    case Operand::H: ML_HAG_L_H(port, out); return;
                    case Operand::L: ML_HAG_L_L(port, out); return;
                }
                break;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Numeric)) {
        auto op = operandTable[line->token[1].second];
        auto n = atoi(line->token[3].second.c_str());
        if (mnemonic_range(line, n, -127, 255)) {
            out |= n < 0 ? 0x40 : 0x00;
            if (out != 0x42) {
                line->error = true;
                line->errmsg = "MOD cannot operate on negative numbers.";
            } else {
                switch (op) {
                    case Operand::A: ML_HAG_A_N(port, out, n); break;
                    case Operand::B: ML_HAG_B_N(port, out, n); break;
                    case Operand::C: ML_HAG_C_N(port, out, n); break;
                    case Operand::D: ML_HAG_D_N(port, out, n); break;
                    case Operand::E: ML_HAG_E_N(port, out, n); break;
                    case Operand::H: ML_HAG_H_N(port, out, n); break;
                    case Operand::L: ML_HAG_L_N(port, out, n); break;
                    case Operand::HL: ML_HAG_HL_N(port, out, n); break;
                }
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
