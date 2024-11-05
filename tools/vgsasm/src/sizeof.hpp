/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

void sizeof_parse(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::Other) {
            if (it->second == "SIZEOF") {
                it->first = TokenType::Delete;
                it++;
                if (it == line->token.end() || it->first != TokenType::BracketBegin) {
                    line->error = true;
                    line->errmsg = "No `(` in sizeof syntax.";
                    return;
                }
                it->first = TokenType::Delete;
                it++;
                if (it == line->token.end() || it->first != TokenType::Other) {
                    line->error = true;
                    line->errmsg = "No structure name specified in sizeof syntax.";
                    return;
                }
                it->first = TokenType::SizeOf;
                it++;
                if (it == line->token.end() || it->first != TokenType::BracketEnd) {
                    line->error = true;
                    line->errmsg = "No `)` in sizeof syntax.";
                    return;
                }
                it->first = TokenType::Delete;
            }
        }
    }
}

void sizeof_replace(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::SizeOf) {
            auto s = structTable.find(it->second);
            if (s == structTable.end()) {
                auto token = split_token(it->second, '.');
                if (token.size() == 2) {
                    s = structTable.find(token[0]);
                    if (s != structTable.end()) {
                        int size = 0;
                        for (auto field : s->second->fields) {
                            if (field->name == token[1]) {
                                size = field->size * field->count;
                            }
                        }
                        if (!size) {
                            line->error = true;
                            line->errmsg = "Undefined field " + token[1] + " of strcture " + token[0] + " is specified in sizeof.";
                            return;
                        } else {
                            it->first = TokenType::Numeric;
                            it->second = std::to_string(size);
                            continue;
                        }
                    } else {
                        line->error = true;
                        line->errmsg = "Undefined structure " + token[0] + " is specified in sizeof.";
                        return;
                    }
                }
                line->error = true;
                line->errmsg = "Undefined structure " + it->second + " is specified in sizeof.";
                return;
            }
            it->first = TokenType::Numeric;
            it->second = std::to_string(s->second->size);
        }
    }
}
