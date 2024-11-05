/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

void parse_ifdef(std::vector<LineData*>* lines)
{
    bool searchEndif = false;
    bool needRemove = false;
    bool errorDetected = false;
    LineData* lastIfdef = nullptr;
    for (auto it = lines->begin(); it != lines->end(); it++) {
        auto line = *it;
        for (auto token = line->token.begin(); token != line->token.end(); token++) {
            if (!searchEndif) {
                if (token->second == "#IFDEF" || token->second == "#IFNDEF") {
                    auto isNot = token->second == "#IFNDEF";
                    lastIfdef = line;
                    token->first = TokenType::Delete;
                    if (token != line->token.begin()) {
                        line->error = true;
                        line->errmsg = "#ifdef must appear at the beginning of the line.";
                        errorDetected = true;
                        break;
                    } else if (token + 1 == line->token.end()) {
                        line->error = true;
                        line->errmsg = "Definition name not specified in #ifdef.";
                        errorDetected = true;
                        break;
                    } else if (token + 2 != line->token.end()) {
                        line->error = true;
                        line->errmsg = "Multiple definition names specified in #ifdef: " + (token + 2)->second;
                        errorDetected = true;
                        break;
                    }
                    token++;
                    token->first = TokenType::Delete;
                    needRemove = defineTable.find(token->second) == defineTable.end();
                    if (isNot) { needRemove = !needRemove; }
                    searchEndif = true;
                } else if (token->second == "#ELSE" || token->second == "#ENDIF") {
                    line->error = true;
                    line->errmsg = "The corresponding #ifdef is not defined.";
                    errorDetected = true;
                    break;
                }
            } else {
                if (token->second == "#ELSE") {
                    if (token != line->token.begin()) {
                        line->error = true;
                        line->errmsg = "#else must appear at the beginning of the line.";
                        errorDetected = true;
                        break;
                    } else if (token + 1 != line->token.end()) {
                        line->error = true;
                        line->errmsg = "Unexpected symbol has specified: " + (token + 1)->second;
                        errorDetected = true;
                        break;
                    }
                    token->first = TokenType::Delete;
                    needRemove = !needRemove;
                } else if (token->second == "#ENDIF") {
                    if (token != line->token.begin()) {
                        line->error = true;
                        line->errmsg = "#endif must appear at the beginning of the line.";
                        errorDetected = true;
                        break;
                    } else if (token + 1 != line->token.end()) {
                        line->error = true;
                        line->errmsg = "Unexpected symbol has specified: " + (token + 1)->second;
                        errorDetected = true;
                        break;
                    }
                    token->first = TokenType::Delete;
                    searchEndif = false;
                } else if (token->second == "#IFDEF" || token->second == "#IFNDEF") {
                    line->error = true;
                    line->errmsg = "#ifdef is specified as nested.";
                    errorDetected = true;
                    break;
                } else if (needRemove) {
                    token->first = TokenType::Delete;
                }
            }
        }
        if (line->error) {
            break;
        }
    }
    if (!errorDetected && searchEndif) {
        lastIfdef->error = true;
        lastIfdef->errmsg = "#ifdef is not closed with #endif.";
    }
}
