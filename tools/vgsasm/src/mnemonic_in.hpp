/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"
#include "mnemonic.h"

void mnemonic_IN(LineData* line)
{
    if (mnemonic_format_test(line, 6, TokenType::Operand, TokenType::Split, TokenType::AddressBegin, TokenType::Numeric, TokenType::AddressEnd) &&
        operandTable[line->token[1].second] == Operand::A) {
        int n = atoi(line->token[4].second.c_str());
        if (mnemonic_range(line, n, 0, 255)) {
            ML_IN_A(n);
            return;
        }
    } else if (mnemonic_format_test(line, 6, TokenType::Operand, TokenType::Split, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd) &&
               operandTable[line->token[4].second] == Operand::C) {
        line->machine.push_back(0xED);
        switch (operandTable[line->token[1].second]) {
            case Operand::B: line->machine.push_back(0x40); return;
            case Operand::C: line->machine.push_back(0x48); return;
            case Operand::D: line->machine.push_back(0x50); return;
            case Operand::E: line->machine.push_back(0x58); return;
            case Operand::H: line->machine.push_back(0x60); return;
            case Operand::L: line->machine.push_back(0x68); return;
            case Operand::F: line->machine.push_back(0x70); return; // undocumented
            case Operand::A: line->machine.push_back(0x78); return;
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal IN instruction.";
    }
}
