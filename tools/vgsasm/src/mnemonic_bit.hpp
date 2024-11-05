/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"
#include "mnemonic.h"

void mnemonic_bit_op(LineData* line, Mnemonic mne)
{
    if (mnemonic_format_test(line, 4, TokenType::Numeric, TokenType::Split, TokenType::Operand)) {
        int b = atoi(line->token[1].second.c_str());
        if (!mnemonic_range(line, b, 0, 7)) {
            return;
        }
        b <<= 3;
        uint8_t r;
        switch (operandTable[line->token[3].second]) {
            case Operand::A: r = 0b111; break;
            case Operand::B: r = 0b000; break;
            case Operand::C: r = 0b001; break;
            case Operand::D: r = 0b010; break;
            case Operand::E: r = 0b011; break;
            case Operand::H: r = 0b100; break;
            case Operand::L: r = 0b101; break;
            default:
                line->error = true;
                line->errmsg = "Illegal BIT/SET/RES instruction.";
                return;
        }
        uint8_t c;
        switch (mne) {
            case Mnemonic::BIT: c = 0b01000000; break;
            case Mnemonic::SET: c = 0b11000000; break;
            case Mnemonic::RES: c = 0b10000000; break;
            default:
                line->error = true;
                line->errmsg = "Illegal BIT/SET/RES instruction.";
                return;
        }
        line->machine.push_back(0xCB);
        line->machine.push_back(c | b | r);
        return;
    } else if (mnemonic_format_test(line, 6, TokenType::Numeric, TokenType::Split, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd)) {
        bool setZero = true;
        switch (operandTable[line->token[4].second]) {
            case Operand::HL: setZero = false; break;
            case Operand::IX: line->machine.push_back(0xDD); break;
            case Operand::IY: line->machine.push_back(0xFD); break;
            default:
                line->error = true;
                line->errmsg = "Illegal BIT/SET/RES instruction.";
                return;
        }
        int b = atoi(line->token[1].second.c_str());
        if (!mnemonic_range(line, b, 0, 7)) {
            return;
        }
        b <<= 3;
        uint8_t c;
        switch (mne) {
            case Mnemonic::BIT: c = 0b01000110; break;
            case Mnemonic::SET: c = 0b11000110; break;
            case Mnemonic::RES: c = 0b10000110; break;
            default:
                line->error = true;
                line->errmsg = "Illegal BIT/SET/RES instruction.";
                return;
        }
        line->machine.push_back(0xCB);
        if (setZero) {
            line->machine.push_back(0x00);
        }
        line->machine.push_back(c | b);
        return;
    } else if (mnemonic_format_test(line, 8,
                                    TokenType::Numeric,
                                    TokenType::Split,
                                    TokenType::AddressBegin,
                                    TokenType::Operand,
                                    TokenType::PlusOrMinus,
                                    TokenType::Numeric,
                                    TokenType::AddressEnd)) {
        switch (operandTable[line->token[4].second]) {
            case Operand::IX: line->machine.push_back(0xDD); break;
            case Operand::IY: line->machine.push_back(0xFD); break;
            default:
                line->error = true;
                line->errmsg = "Illegal BIT/SET/RES instruction.";
                return;
        }
        int b = atoi(line->token[1].second.c_str());
        if (!mnemonic_range(line, b, 0, 7)) {
            return;
        }
        b <<= 3;
        int n = atoi(line->token[6].second.c_str());
        if (line->token[5].first == TokenType::Plus) {
            if (!mnemonic_range(line, n, 0, 127)) {
                return;
            }
        } else {
            if (!mnemonic_range(line, -n, -128, 0)) {
                return;
            }
            n = 0 - n;
        }
        uint8_t c;
        switch (mne) {
            case Mnemonic::BIT: c = 0b01000110; break;
            case Mnemonic::SET: c = 0b11000110; break;
            case Mnemonic::RES: c = 0b10000110; break;
            default:
                line->error = true;
                line->errmsg = "Illegal BIT/SET/RES instruction.";
                return;
        }
        line->machine.push_back(0xCB);
        line->machine.push_back(n & 0xFF);
        line->machine.push_back(c | b);
        return;
    } else if ((mne == Mnemonic::SET || mne == Mnemonic::RES) &&
               mnemonic_format_test(line, 10,
                                    TokenType::Numeric,
                                    TokenType::Split,
                                    TokenType::AddressBegin,
                                    TokenType::Operand,
                                    TokenType::PlusOrMinus,
                                    TokenType::Numeric,
                                    TokenType::AddressEnd,
                                    TokenType::And,
                                    TokenType::Operand) &&
               operandTable[line->token[4].second] == Operand::IX &&
               operandTable[line->token[9].second] == Operand::B) {
        int b = atoi(line->token[1].second.c_str());
        if (!mnemonic_range(line, b, 0, 7)) {
            return;
        }
        b <<= 3;
        int n = atoi(line->token[6].second.c_str());
        if (line->token[5].first == TokenType::Plus) {
            if (!mnemonic_range(line, n, 0, 127)) {
                return;
            }
        } else {
            if (!mnemonic_range(line, -n, -128, 0)) {
                return;
            }
            n = 0 - n;
        }
        uint8_t c = mne == Mnemonic::SET ? 0xC0 : 0x80;
        line->machine.push_back(0xDD);
        line->machine.push_back(0xCB);
        line->machine.push_back(n & 0xFF);
        line->machine.push_back(c | b);
        return;
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal BIT/SET/RES instruction.";
    }
}
