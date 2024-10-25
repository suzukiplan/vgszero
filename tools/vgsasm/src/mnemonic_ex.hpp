/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"
#include "mnemonic.h"

void mnemonic_EX(LineData* line)
{
    if (line->token.size() < 4) {
        line->error = true;
        line->errmsg = "Excessive or insufficient number of operands.";
    }
    if (line->token[1].first == TokenType::Operand) {
        if (mnemonic_format_check(line, 4, TokenType::Operand, TokenType::Split, TokenType::Operand)) {
            auto left = operandTable[line->token[1].second];
            auto right = operandTable[line->token[3].second];
            if (left == Operand::DE && right == Operand::HL) {
                line->machine.push_back(0xEB);
            } else if (left == Operand::AF && right == Operand::AF_dash) {
                line->machine.push_back(0x08);
            } else {
                line->error = true;
                line->errmsg = "Incorrect EX syntax.";
            }
        }
    } else if (line->token[1].first == TokenType::AddressBegin) {
        if (mnemonic_format_check(line, 6, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd, TokenType::Split, TokenType::Operand)) {
            auto left = operandTable[line->token[2].second];
            auto right = operandTable[line->token[5].second];
            if (left == Operand::SP && right == Operand::HL) {
                line->machine.push_back(0xE3);
            } else if (left == Operand::SP && right == Operand::IX) {
                line->machine.push_back(0xDD);
                line->machine.push_back(0xE3);
            } else if (left == Operand::SP && right == Operand::IY) {
                line->machine.push_back(0xFD);
                line->machine.push_back(0xE3);
            } else {
                line->error = true;
                line->errmsg = "Incorrect EX syntax.";
            }
        }
    } else {
        line->error = true;
        line->errmsg = "Incorrect EX syntax.";
    }
}
