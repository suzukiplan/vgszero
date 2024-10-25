/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"
#include "mnemonic.h"

void mnemonic_DB(LineData* line)
{
    if (line->token.size() < 2) {
        return;
    }
    for (auto it = line->token.begin() + 1; it != line->token.end(); it++) {
        switch (it->first) {
            case TokenType::Numeric: {
                auto n = atoi(it->second.c_str());
                if (n < -128 || 255 < n) {
                    line->error = true;
                    line->errmsg = "An invalid range of number specified: " + it->second;
                    return;
                }
                line->machine.push_back(n & 0xFF);
                break;
            }
            case TokenType::String: {
                for (auto str = it->second.c_str(); *str; str++) {
                    line->machine.push_back(*str);
                }
                break;
            }
            default:
                line->error = true;
                line->errmsg = "Unexpected symbols detected in DB: " + it->second;
                return;
        }
        if (it + 1 != line->token.end()) {
            it++;
            if (it->first != TokenType::Split) {
                line->error = true;
                line->errmsg = "Symbols are not specified as comma-separated: " + it->second;
                return;
            }
            if (it + 1 == line->token.end()) {
                line->error = true;
                line->errmsg = "No symbol is specified after the comma.";
                return;
            }
        }
    }
}

void mnemonic_DW(LineData* line)
{
    if (line->token.size() < 2) {
        return;
    }
    for (auto it = line->token.begin() + 1; it != line->token.end(); it++) {
        switch (it->first) {
            case TokenType::Numeric: {
                auto n = atoi(it->second.c_str());
                if (n < -32768 || 65535 < n) {
                    line->error = true;
                    line->errmsg = "An invalid range of number specified: " + it->second;
                    return;
                }
                line->machine.push_back(n & 0x00FF);
                line->machine.push_back((n & 0xFF00) >> 8);
                break;
            }
            case TokenType::String: {
                for (auto str = it->second.c_str(); *str; str++) {
                    line->machine.push_back(*str);
                    line->machine.push_back(0x00);
                }
                break;
            }
            case TokenType::LabelJump: {
                tempAddrs.push_back(new TempAddr(line, it->second, line->machine.size(), false));
                line->machine.push_back(0x00);
                line->machine.push_back(0x00);
                break;
            }
            default:
                line->error = true;
                line->errmsg = "Unexpected symbols detected in DB: " + it->second;
                return;
        }
        if (it + 1 != line->token.end()) {
            it++;
            if (it->first != TokenType::Split) {
                line->error = true;
                line->errmsg = "Symbols are not specified as comma-separated: " + it->second;
                return;
            }
            if (it + 1 == line->token.end()) {
                line->error = true;
                line->errmsg = "No symbol is specified after the comma.";
                return;
            }
        }
    }
}
