/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

void offset_parse(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::Other) {
            if (it->second == "OFFSET") {
                it->first = TokenType::Delete;
                it++;
                if (it == line->token.end() || it->first != TokenType::BracketBegin) {
                    line->error = true;
                    line->errmsg = "No `(` in offset syntax.";
                    return;
                }
                it->first = TokenType::Delete;
                it++;
                if (it == line->token.end() || it->first != TokenType::Other) {
                    line->error = true;
                    line->errmsg = "No structure name specified in offset syntax.";
                    return;
                }
                if (-1 == it->second.find('.')) {
                    line->error = true;
                    line->errmsg = "No structure field name specified in offset syntax.";
                    return;
                }
                it->first = TokenType::Offset;
                it++;
                if (it == line->token.end() || it->first != TokenType::BracketEnd) {
                    line->error = true;
                    line->errmsg = "No `)` in offset syntax.";
                    return;
                }
                it->first = TokenType::Delete;
            }
        }
    }
}

void offset_replace(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::Offset) {
            auto dot = it->second.find(".");
            if (-1 == dot) {
                // NOTE: this error case will not pass
                line->error = true;
                line->errmsg = "No structure field name specified in `offset`: " + it->second;
                return;
            }
            auto name = it->second.substr(0, dot);
            auto field = it->second.substr(dot + 1);
            auto s = structTable.find(name);
            if (s == structTable.end()) {
                line->error = true;
                line->errmsg = "Undefined structure " + name + " is specified in offset.";
                return;
            }
            int offset = 0;
            bool found = false;
            for (auto f : s->second->fields) {
                if (f->name == field) {
                    it->first = TokenType::Numeric;
                    it->second = std::to_string(offset);
                    found = true;
                    break;
                } else {
                    offset += f->size;
                }
            }
            if (!found) {
                line->error = true;
                line->errmsg = "Field name `" + field + "` is not defined in structure `" + name + "`.";
                return;
            }
        }
    }
}
