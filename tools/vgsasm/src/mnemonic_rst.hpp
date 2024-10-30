/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"
#include "mnemonic.h"

void mnemonic_RST(LineData* line)
{
    if (mnemonic_format_test(line, 2, TokenType::Numeric)) {
        auto n = atoi(line->token[1].second.c_str());
        switch (n) {
            case 0: line->machine.push_back(0xC7); return;
            case 1: line->machine.push_back(0xCF); return;
            case 2: line->machine.push_back(0xD7); return;
            case 3: line->machine.push_back(0xDF); return;
            case 4: line->machine.push_back(0xE7); return;
            case 5: line->machine.push_back(0xEF); return;
            case 6: line->machine.push_back(0xF7); return;
            case 7: line->machine.push_back(0xFF); return;
            case 0x08: line->machine.push_back(0xCF); return;
            case 0x10: line->machine.push_back(0xD7); return;
            case 0x18: line->machine.push_back(0xDF); return;
            case 0x20: line->machine.push_back(0xE7); return;
            case 0x28: line->machine.push_back(0xEF); return;
            case 0x30: line->machine.push_back(0xF7); return;
            case 0x38: line->machine.push_back(0xFF); return;
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal RST instruction.";
    }
}
