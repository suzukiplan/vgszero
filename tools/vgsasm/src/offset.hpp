/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

void parse_offset(LineData* line)
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

void replace_offset(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::Offset) {
            auto tokens = split_token(it->second, '.');
            if (2 != tokens.size()) {
                line->error = true;
                line->errmsg = "No structure field name specified in `offset`: " + it->second;
                return;
            }
            auto s = structTable.find(tokens[0]);
            if (s == structTable.end()) {
                line->error = true;
                line->errmsg = "Undefined structure " + it->second + " is specified in offset.";
                return;
            }
            int offset = 0;
            bool found = false;
            for (auto f : s->second->fields) {
                if (f->name == tokens[1]) {
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
                line->errmsg = "Field name `" + tokens[1] + "` is not defined in structure `" + tokens[0] + "`.";
                return;
            }
        }
    }
}
