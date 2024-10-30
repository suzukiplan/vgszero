/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"
#include "mnemonic.h"

uint8_t get_jr_condition(LineData* line, std::string cond)
{
    switch (operandTable[cond]) {
        case Operand::NZ: return 0x20;
        case Operand::Z: return 0x28;
        case Operand::NC: return 0x30;
        case Operand::C: return 0x38;
    }
    line->error = true;
    line->errmsg = "Invalid condition: " + cond;
    return 0x00;
}

void mnemonic_JR(LineData* line)
{
    if (mnemonic_format_test(line, 2, TokenType::Numeric)) {
        auto e = atoi(line->token[1].second.c_str());
        if (mnemonic_range(line, e, -128, 127)) {
            line->machine.push_back(0x18);
            line->machine.push_back(e);
            return;
        }
    } else if (mnemonic_format_test(line, 2, TokenType::LabelJump)) {
        line->machine.push_back(0x18);
        tempAddrs.push_back(new TempAddr(line, line->token[1].second, line->machine.size(), true));
        line->machine.push_back(0x00);
        return;
    } else if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Numeric) &&
               operand_is_condition(line->token[1].second)) {
        uint8_t code = get_jr_condition(line, line->token[1].second);
        auto e = atoi(line->token[3].second.c_str());
        if (code && mnemonic_range(line, e, -128, 127)) {
            line->machine.push_back(code);
            line->machine.push_back(e);
            return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::LabelJump) &&
               operand_is_condition(line->token[1].second)) {
        uint8_t code = get_jr_condition(line, line->token[1].second);
        if (code) {
            line->machine.push_back(code);
            tempAddrs.push_back(new TempAddr(line, line->token[3].second, line->machine.size(), true));
            line->machine.push_back(0x00);
            return;
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal JR instruction.";
    }
}
