/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"
#include "mnemonic.h"

void mnemonic_DJNZ(LineData* line)
{
    if (mnemonic_format_test(line, 2, TokenType::Numeric)) {
        auto e = atoi(line->token[1].second.c_str());
        if (mnemonic_range(line, e, -128, 127)) {
            line->machine.push_back(0x10);
            line->machine.push_back(e);
            return;
        }
    } else if (mnemonic_format_test(line, 2, TokenType::LabelJump)) {
        line->machine.push_back(0x10);
        tempAddrs.push_back(new TempAddr(line, line->token[1].second, line->machine.size(), true));
        line->machine.push_back(0x00);
        return;
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal DJNZ instruction.";
    }
}
