/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"
#include "mnemonic.h"

void mnemonic_shift(LineData* line, uint8_t code)
{
    if (mnemonic_format_test(line, 2, TokenType::Operand) ||
        mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Numeric)) {
        auto op = operandTable[line->token[1].second];
        if (op == Operand::A ||
            op == Operand::B ||
            op == Operand::C ||
            op == Operand::D ||
            op == Operand::E ||
            op == Operand::H ||
            op == Operand::L) {
            int count = line->token.size() == 2 ? 1 : atoi(line->token[3].second.c_str());
            for (int i = 0; i < count; i++) {
                line->machine.push_back(0xCB);
                switch (op) {
                    case Operand::A: line->machine.push_back(code | 0x07); break;
                    case Operand::B: line->machine.push_back(code | 0x00); break;
                    case Operand::C: line->machine.push_back(code | 0x01); break;
                    case Operand::D: line->machine.push_back(code | 0x02); break;
                    case Operand::E: line->machine.push_back(code | 0x03); break;
                    case Operand::H: line->machine.push_back(code | 0x04); break;
                    case Operand::L: line->machine.push_back(code | 0x05); break;
                }
            }
            return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd) ||
               mnemonic_format_test(line, 6, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd, TokenType::Split, TokenType::Numeric)) {
        auto op = operandTable[line->token[2].second];
        if (op == Operand::HL || op == Operand::IX || op == Operand::IY) {
            int count = line->token.size() == 4 ? 1 : atoi(line->token[5].second.c_str());
            for (int i = 0; i < count; i++) {
                switch (op) {
                    case Operand::HL:;
                        line->machine.push_back(0xCB);
                        line->machine.push_back(code | 0x06);
                        break;
                    case Operand::IX:;
                        line->machine.push_back(0xDD);
                        line->machine.push_back(0xCB);
                        line->machine.push_back(0x00);
                        line->machine.push_back(code | 0x06);
                        break;
                    case Operand::IY:;
                        line->machine.push_back(0xFD);
                        line->machine.push_back(0xCB);
                        line->machine.push_back(0x00);
                        line->machine.push_back(code | 0x06);
                        break;
                }
            }
            return;
        }
    } else if (mnemonic_format_test(line, 6, TokenType::AddressBegin, TokenType::Operand, TokenType::PlusOrMinus, TokenType::Numeric, TokenType::AddressEnd) ||
               mnemonic_format_test(line, 8, TokenType::AddressBegin, TokenType::Operand, TokenType::PlusOrMinus, TokenType::Numeric, TokenType::AddressEnd, TokenType::Split, TokenType::Numeric)) {
        int n = atoi(line->token[4].second.c_str());
        if (line->token[3].first == TokenType::Plus) {
            if (!mnemonic_range(line, n, 0, 127)) {
                return;
            }
        } else {
            if (!mnemonic_range(line, n, 0, 128)) {
                return;
            }
            n = 0 - n;
        }
        int count = line->token.size() == 6 ? 1 : atoi(line->token[7].second.c_str());
        auto op = operandTable[line->token[2].second];
        if (op == Operand::IX || op == Operand::IY) {
            for (int i = 0; i < count; i++) {
                switch (op) {
                    case Operand::IX:
                        line->machine.push_back(0xDD);
                        line->machine.push_back(0xCB);
                        line->machine.push_back(n);
                        line->machine.push_back(code | 0x06);
                        break;
                    case Operand::IY:
                        line->machine.push_back(0xFD);
                        line->machine.push_back(0xCB);
                        line->machine.push_back(n);
                        line->machine.push_back(code | 0x06);
                        break;
                }
            }
            return;
        }
    } else if (mnemonic_format_test(line, 8, TokenType::AddressBegin, TokenType::Operand, TokenType::PlusOrMinus, TokenType::Numeric, TokenType::AddressEnd, TokenType::And, TokenType::Operand)) {
        int n = atoi(line->token[4].second.c_str());
        if (line->token[3].first == TokenType::Plus) {
            if (!mnemonic_range(line, n, 0, 127)) {
                return;
            }
        } else {
            if (!mnemonic_range(line, n, 0, 128)) {
                return;
            }
            n = 0 - n;
        }
        auto op1 = operandTable[line->token[2].second];
        auto op2 = operandTable[line->token[7].second];
        if (code == 0x00 && op1 == Operand::IX) {
            line->machine.push_back(0xDD);
            line->machine.push_back(0xCB);
            line->machine.push_back(n);
            switch (op2) {
                case Operand::B: line->machine.push_back(0x00); return;
                case Operand::C: line->machine.push_back(0x01); return;
                case Operand::D: line->machine.push_back(0x02); return;
                case Operand::E: line->machine.push_back(0x03); return;
                case Operand::H: line->machine.push_back(0x04); return;
                case Operand::L: line->machine.push_back(0x05); return;
                case Operand::F: line->machine.push_back(0x06); return;
                case Operand::A: line->machine.push_back(0x07); return;
            }
        } else if (op1 == Operand::IX && op2 == Operand::B) {
            line->machine.push_back(0xDD);
            line->machine.push_back(0xCB);
            line->machine.push_back(n);
            switch (mnemonicTable[line->token[0].second]) {
                case Mnemonic::RRC: line->machine.push_back(0x08); return;
                case Mnemonic::RL: line->machine.push_back(0x10); return;
                case Mnemonic::RR: line->machine.push_back(0x18); return;
                case Mnemonic::SLA: line->machine.push_back(0x20); return;
                case Mnemonic::SRA: line->machine.push_back(0x28); return;
                case Mnemonic::SLL: line->machine.push_back(0x30); return;
                case Mnemonic::SRL: line->machine.push_back(0x38); return;
            }
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal shift/rotate instruction.";
    }
}
