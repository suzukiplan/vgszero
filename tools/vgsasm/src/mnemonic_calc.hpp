/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"
#include "mnemonic.h"

void mnemonic_calc8(LineData* line, uint8_t code)
{
    if (mnemonic_format_test(line, 2, TokenType::Operand)) {
        auto op = operandTable[line->token[1].second];
        if (op == Operand::IXH || op == Operand::IXL) {
            line->machine.push_back(0xDD);
        } else if (op == Operand::IYH || op == Operand::IYL) {
            line->machine.push_back(0xFD);
        }
        switch (op) {
            case Operand::A: code |= 0x07; break;
            case Operand::B: code |= 0x00; break;
            case Operand::C: code |= 0x01; break;
            case Operand::D: code |= 0x02; break;
            case Operand::E: code |= 0x03; break;
            case Operand::H: code |= 0x04; break;
            case Operand::L: code |= 0x05; break;
            case Operand::IXH: code |= 0x04; break;
            case Operand::IXL: code |= 0x05; break;
            case Operand::IYH: code |= 0x04; break;
            case Operand::IYL: code |= 0x05; break;
            default:
                line->error = true;
                line->errmsg = "Illegal 8-bit arithmetic instruction.";
                return;
        }
        line->machine.push_back(code);
    } else if (mnemonic_format_test(line, 2, TokenType::Numeric)) {
        auto n = atoi(line->token[1].second.c_str());
        if (mnemonic_range(line, n, -128, 255)) {
            line->machine.push_back(code | 0x46);
            line->machine.push_back(n & 0xFF);
        }
    } else if (4 <= line->token.size() &&
               line->token[1].first == TokenType::AddressBegin &&
               line->token[2].first == TokenType::Operand) {
        auto op = operandTable[line->token[2].second];
        if (op == Operand::HL) {
            if (4 != line->token.size() && line->token[3].first == TokenType::AddressEnd) {
                line->error = true;
                line->errmsg = "Illegal 8-bit arithmetic instruction.";
            } else {
                line->machine.push_back(code | 0x06);
            }
        } else if (op == Operand::IX || op == Operand::IY) {
            line->machine.push_back(op == Operand::IX ? 0xDD : 0xFD);
            line->machine.push_back(code | 0x06);
            if (4 == line->token.size() && line->token[3].first == TokenType::AddressEnd) {
                line->machine.push_back(0x00);
            } else if (6 == line->token.size() &&
                       line->token[3].first == TokenType::Plus &&
                       line->token[4].first == TokenType::Numeric &&
                       line->token[5].first == TokenType::AddressEnd) {
                auto n = atoi(line->token[4].second.c_str());
                if (mnemonic_range(line, n, 0, 127)) {
                    line->machine.push_back(n & 0xFF);
                }
            } else if (6 == line->token.size() &&
                       line->token[3].first == TokenType::Minus &&
                       line->token[4].first == TokenType::Numeric &&
                       line->token[5].first == TokenType::AddressEnd) {
                auto n = atoi(line->token[4].second.c_str());
                if (mnemonic_range(line, n, 0, 128)) {
                    line->machine.push_back((0 - n) & 0xFF);
                }
            } else {
                line->error = true;
                line->errmsg = "Illegal 8-bit arithmetic instruction.";
            }
        } else {
            line->error = true;
            line->errmsg = "Illegal 8-bit arithmetic instruction.";
        }
    } else if (3 < line->token.size() && line->token[1].first == TokenType::Operand && line->token[2].first == TokenType::Split) {
        auto op = operandTable[line->token[1].second];
        if (op == Operand::A) {
            auto it = line->token.begin() + 1;
            line->token.erase(it);
            line->token.erase(it);
            mnemonic_calc8(line, code);
        } else {
            line->error = true;
            line->errmsg = "Illegal arithmetic instruction.";
        }
    } else {
        line->error = true;
        line->errmsg = "Illegal 8-bit arithmetic instruction.";
    }
}

void mnemonic_calc16(LineData* line, uint8_t code)
{
    bool supportImmediate = true;
    bool supportIXY = true;
    auto mne = mnemonicTable[line->token[0].second];
    switch (mne) {
        case Mnemonic::ADC:
        case Mnemonic::SBC:
            line->machine.push_back(0xED);
            supportImmediate = false;
            supportIXY = false;
            break;
    }
    if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Operand)) {
        auto op1 = operandTable[line->token[1].second];
        auto op2 = operandTable[line->token[3].second];
        if (op1 == Operand::BC && op2 == Operand::A) {
            ML_ADD_A_C;
            ML_LD_C_A;
            ML_JR_NC(1);
            ML_INC_B;
            return;
        } else if (op1 == Operand::DE && op2 == Operand::A) {
            ML_ADD_A_E;
            ML_LD_E_A;
            ML_JR_NC(1);
            ML_INC_D;
            return;
        } else if (op1 == Operand::HL && op2 == Operand::A) {
            ML_ADD_A_L;
            ML_LD_L_A;
            ML_JR_NC(1);
            ML_INC_H;
            return;
        } else if (op1 == Operand::HL) {
            if (op2 != Operand::BC && op2 != Operand::DE && op2 != Operand::HL && op2 != Operand::SP) {
                line->error = true;
                line->errmsg = "Illegal 16-bit arithmetic instruction.";
                return;
            }
        } else if (supportIXY && op1 == Operand::IX) {
            line->machine.push_back(0xDD);
            if (op2 != Operand::BC && op2 != Operand::DE && op2 != Operand::IX && op2 != Operand::SP) {
                line->error = true;
                line->errmsg = "Illegal 16-bit arithmetic instruction.";
                return;
            }
        } else if (supportIXY && op1 == Operand::IY) {
            line->machine.push_back(0xFD);
            if (op2 != Operand::BC && op2 != Operand::DE && op2 != Operand::IY && op2 != Operand::SP) {
                line->error = true;
                line->errmsg = "Illegal 16-bit arithmetic instruction.";
                return;
            }
        } else {
            line->error = true;
            line->errmsg = "Illegal 16-bit arithmetic instruction.";
            return;
        }
        switch (op2) {
            case Operand::BC: line->machine.push_back(code); break;
            case Operand::DE: line->machine.push_back(code | 0x10); break;
            case Operand::HL: line->machine.push_back(code | 0x20); break;
            case Operand::IX: line->machine.push_back(code | 0x20); break;
            case Operand::IY: line->machine.push_back(code | 0x20); break;
            case Operand::SP: line->machine.push_back(code | 0x30); break;
        }
    } else if (supportImmediate &&
               mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Numeric)) {
        auto op = operandTable[line->token[1].second];
        auto nn = atoi(line->token[3].second.c_str());
        if (op != Operand::HL && op != Operand::IX && op != Operand::IY) {
            line->error = true;
            line->errmsg = "Illegal 16-bit arithmetic instruction.";
            return;
        }
        if (mnemonic_range(line, nn, -32768, 65535)) {
            uint8_t nl = nn & 0xFF;
            uint8_t nh = (nn & 0xFF00) >> 8;
            ML_PUSH_DE;
            ML_LD_D_n(nh);
            ML_LD_E_n(nl);
            switch (op) {
                case Operand::IX: line->machine.push_back(0xDD); break;
                case Operand::IY: line->machine.push_back(0xFD); break;
            }
            line->machine.push_back(code | 0x10);
            ML_POP_DE;
        }
    } else if (mne == Mnemonic::ADD && mnemonic_format_test(line, 6, TokenType::Operand, TokenType::Split, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd)) {
        auto op1 = operandTable[line->token[1].second];
        auto op2 = operandTable[line->token[4].second];
        if (op1 == Operand::HL && op2 == Operand::IX) {
            ML_PUSH_DE;
            ML_LD_E_IX(0);
            ML_LD_D_IX(1);
            ML_ADD_HL_DE;
            ML_POP_DE;
        } else if (op1 == Operand::HL && op2 == Operand::IY) {
            ML_PUSH_DE;
            ML_LD_E_IY(0);
            ML_LD_D_IY(1);
            ML_ADD_HL_DE;
            ML_POP_DE;
        } else {
            line->error = true;
            line->errmsg = "Illegal 16-bit arithmetic instruction.";
        }
    } else if (mne == Mnemonic::ADD && mnemonic_format_test(line, 8, TokenType::Operand, TokenType::Split, TokenType::AddressBegin, TokenType::Operand, TokenType::PlusOrMinus, TokenType::Numeric, TokenType::AddressEnd)) {
        auto op1 = operandTable[line->token[1].second];
        auto op2 = operandTable[line->token[4].second];
        auto d = atoi(line->token[6].second.c_str());
        if (line->token[5].first == TokenType::Minus) { d = -d; }
        if (mnemonic_range(line, d, -128, 126)) {
            if (op1 == Operand::HL && op2 == Operand::IX) {
                ML_PUSH_DE;
                ML_LD_E_IX(d);
                ML_LD_D_IX(d + 1);
                ML_ADD_HL_DE;
                ML_POP_DE;
            } else if (op1 == Operand::HL && op2 == Operand::IY) {
                ML_PUSH_DE;
                ML_LD_E_IY(d);
                ML_LD_D_IY(d + 1);
                ML_ADD_HL_DE;
                ML_POP_DE;
            } else {
                line->error = true;
                line->errmsg = "Illegal 16-bit arithmetic instruction.";
            }
        }
    } else {
        line->error = true;
        line->errmsg = "Illegal 16-bit arithmetic instruction.";
    }
}

void mnemonic_calcOH(LineData* line, uint8_t code8, uint8_t code16)
{
    if (mnemonic_format_test(line, 4, TokenType::AddressBegin, TokenType::Numeric, TokenType::AddressEnd)) {
        // 即値アドレス演算（auto-expand）
        auto addr = atoi(line->token[2].second.c_str());
        if (mnemonic_range(line, addr, 0, 0xFFFF)) {
            switch (mnemonicTable[line->token[0].second]) {
                case Mnemonic::ADD:
                    ML_PUSH_HL;
                    ML_LD_L_n(addr & 0x00FF);
                    ML_LD_H_n((addr & 0xFF00) >> 8);
                    ML_ADD_HL;
                    ML_POP_HL;
                    break;
                case Mnemonic::ADC:
                    ML_PUSH_HL;
                    ML_LD_L_n(addr & 0x00FF);
                    ML_LD_H_n((addr & 0xFF00) >> 8);
                    ML_ADC_HL;
                    ML_POP_HL;
                    break;
                case Mnemonic::SUB:
                    ML_PUSH_HL;
                    ML_LD_L_n(addr & 0x00FF);
                    ML_LD_H_n((addr & 0xFF00) >> 8);
                    ML_SUB_HL;
                    ML_POP_HL;
                    break;
                case Mnemonic::SBC:
                    ML_PUSH_HL;
                    ML_LD_L_n(addr & 0x00FF);
                    ML_LD_H_n((addr & 0xFF00) >> 8);
                    ML_SBC_HL;
                    ML_POP_HL;
                    break;
                default:
                    line->error = true;
                    line->errmsg = "Unsupported immediate address 8bit arithmetic instruction: " + line->token[0].second;
            }
        }
    } else if (0 == code16) {
        mnemonic_calc8(line, code8);
    } else if (mnemonic_format_test(line, 2, TokenType::Operand)) {
        mnemonic_calc8(line, code8);
    } else if (mnemonic_format_test(line, 2, TokenType::Numeric)) {
        mnemonic_calc8(line, code8);
    } else if (3 <= line->token.size() && line->token[1].first == TokenType::AddressBegin) {
        mnemonic_calc8(line, code8);
    } else if (3 < line->token.size() && line->token[1].first == TokenType::Operand && line->token[2].first == TokenType::Split) {
        auto op = operandTable[line->token[1].second];
        if (op == Operand::A) {
            auto it = line->token.begin() + 1;
            line->token.erase(it);
            line->token.erase(it);
            mnemonic_calc8(line, code8);
        } else if (mnemonic_is_reg16(op)) {
            mnemonic_calc16(line, code16);
        } else {
            line->error = true;
            line->errmsg = "Illegal arithmetic instruction.";
        }
    } else {
        line->error = true;
        line->errmsg = "Illegal arithmetic instruction.";
    }
}
