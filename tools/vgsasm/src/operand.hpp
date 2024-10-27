/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

bool operand_is_condition(Operand op)
{
    switch (op) {
        case Operand::NZ: return true;
        case Operand::Z: return true;
        case Operand::NC: return true;
        case Operand::C: return true;
        case Operand::PO: return true;
        case Operand::PE: return true;
        case Operand::P: return true;
        case Operand::M: return true;
    }
    return false;
}

bool operand_is_condition(std::string str)
{
    auto op = operandTable.find(str);
    if (op != operandTable.end()) {
        return operand_is_condition(op->second);
    }
    return false;
}

void operand_parse(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        // Other ならチェック
        if (it->first == TokenType::Other) {
            auto op = operandTable.find(it->second);
            if (op != operandTable.end()) {
                it->first = TokenType::Operand;
            }
        }
    }
}
