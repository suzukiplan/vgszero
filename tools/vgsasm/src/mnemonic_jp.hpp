/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"
#include "mnemonic.h"

uint8_t get_jp_condition(LineData* line, std::string cond)
{
    switch (operandTable[cond]) {
        case Operand::NZ: return 0xC2;
        case Operand::Z: return 0xCA;
        case Operand::NC: return 0xD2;
        case Operand::C: return 0xDA;
        case Operand::PO: return 0xE2;
        case Operand::PE: return 0xEA;
        case Operand::P: return 0xF2;
        case Operand::M: return 0xFA;
    }
    line->error = true;
    line->errmsg = "Invalid condition: " + cond;
    return 0x00;
}

void mnemonic_JP(LineData* line)
{
    if (mnemonic_format_test(line, 2, TokenType::Numeric)) {
        auto addr = atoi(line->token[1].second.c_str());
        if (mnemonic_range(line, addr, 0x0000, 0xFFFF)) {
            line->machine.push_back(0xC3);
            line->machine.push_back(addr & 0xFF);
            line->machine.push_back((addr & 0xFF00) >> 8);
            return;
        }
    } else if (mnemonic_format_test(line, 2, TokenType::LabelJump)) {
        line->machine.push_back(0xC3);
        tempAddrs.push_back(new TempAddr(line, line->token[1].second, line->machine.size(), false));
        line->machine.push_back(0x00);
        line->machine.push_back(0x00);
        return;
    } else if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Numeric) &&
               operand_is_condition(line->token[1].second)) {
        uint8_t code = get_jp_condition(line, line->token[1].second);
        auto addr = atoi(line->token[3].second.c_str());
        if (code && mnemonic_range(line, addr, 0x0000, 0xFFFF)) {
            line->machine.push_back(code);
            line->machine.push_back(addr & 0xFF);
            line->machine.push_back((addr & 0xFF00) >> 8);
            return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::LabelJump) &&
               operand_is_condition(line->token[1].second)) {
        uint8_t code = get_jp_condition(line, line->token[1].second);
        if (code) {
            line->machine.push_back(code);
            tempAddrs.push_back(new TempAddr(line, line->token[3].second, line->machine.size(), false));
            line->machine.push_back(0x00);
            line->machine.push_back(0x00);
            return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd) ||
               mnemonic_format_test(line, 2, TokenType::Operand)) {
        switch (operandTable[line->token[line->token.size() == 4 ? 2 : 1].second]) {
            case Operand::BC:
                ML_PUSH_BC;
                ML_RET;
                return;
            case Operand::DE:
                ML_PUSH_DE;
                ML_RET;
                return;
            case Operand::HL:
                line->machine.push_back(0xE9);
                return;
            case Operand::IX:
                line->machine.push_back(0xDD);
                line->machine.push_back(0xE9);
                return;
            case Operand::IY:
                line->machine.push_back(0xFD);
                line->machine.push_back(0xE9);
                return;
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal JP instruction.";
    }
}
