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
            auto tokens = split_token(it->second, '.');
            if (tokens.size() < 2) {
                // NOTE: this error case will not pass
                line->error = true;
                line->errmsg = "No structure field name specified in `offset`: " + it->second;
                return;
            }
            auto s = structTable.find(tokens[0]);
            if (s == structTable.end()) {
                line->error = true;
                line->errmsg = "Undefined structure " + tokens[0] + " is specified in offset.";
                return;
            }
            Struct* str = s->second;
            int offset = 0;
            for (auto token = tokens.begin() + 1; token != tokens.end(); token++) {
                StructField* field = nullptr;
                for (auto f : str->fields) {
                    if (f->name == *token) {
                        field = f;
                        break;
                    } else {
                        offset += f->size;
                    }
                }
                if (!field) {
                    line->error = true;
                    line->errmsg = "Field name `" + (*token) + "` is not defined in structure `" + (str->name) + "`.";
                    return;
                }
                if (token + 1 != tokens.end()) {
                    if (field->typeName.empty()) {
                        line->error = true;
                        line->errmsg = "Non-structured field `" + (*token) + "` with period.";
                        return;
                    }
                    str = structTable.find(field->typeName)->second;
                }
            }
            it->first = TokenType::Numeric;
            it->second = std::to_string(offset);
        }
    }
}
