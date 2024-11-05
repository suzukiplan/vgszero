/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

void define_init()
{
    defineTable["JNZ"] = {
        std::make_pair<TokenType, std::string>(TokenType::Mnemonic, "JP"),
        std::make_pair<TokenType, std::string>(TokenType::Operand, "NZ"),
        std::make_pair<TokenType, std::string>(TokenType::Split, ",")};
    defineTable["JZ"] = {
        std::make_pair<TokenType, std::string>(TokenType::Mnemonic, "JP"),
        std::make_pair<TokenType, std::string>(TokenType::Operand, "Z"),
        std::make_pair<TokenType, std::string>(TokenType::Split, ",")};
    defineTable["JNC"] = {
        std::make_pair<TokenType, std::string>(TokenType::Mnemonic, "JP"),
        std::make_pair<TokenType, std::string>(TokenType::Operand, "NC"),
        std::make_pair<TokenType, std::string>(TokenType::Split, ",")};
    defineTable["JC"] = {
        std::make_pair<TokenType, std::string>(TokenType::Mnemonic, "JP"),
        std::make_pair<TokenType, std::string>(TokenType::Operand, "C"),
        std::make_pair<TokenType, std::string>(TokenType::Split, ",")};
    defineTable["JPE"] = {
        std::make_pair<TokenType, std::string>(TokenType::Mnemonic, "JP"),
        std::make_pair<TokenType, std::string>(TokenType::Operand, "PE"),
        std::make_pair<TokenType, std::string>(TokenType::Split, ",")};
    defineTable["JPO"] = {
        std::make_pair<TokenType, std::string>(TokenType::Mnemonic, "JP"),
        std::make_pair<TokenType, std::string>(TokenType::Operand, "PO"),
        std::make_pair<TokenType, std::string>(TokenType::Split, ",")};
    defineTable["JPP"] = {
        std::make_pair<TokenType, std::string>(TokenType::Mnemonic, "JP"),
        std::make_pair<TokenType, std::string>(TokenType::Operand, "P"),
        std::make_pair<TokenType, std::string>(TokenType::Split, ",")};
    defineTable["JPM"] = {
        std::make_pair<TokenType, std::string>(TokenType::Mnemonic, "JP"),
        std::make_pair<TokenType, std::string>(TokenType::Operand, "M"),
        std::make_pair<TokenType, std::string>(TokenType::Split, ",")};
    defineTable["JM"] = {
        std::make_pair<TokenType, std::string>(TokenType::Mnemonic, "JP"),
        std::make_pair<TokenType, std::string>(TokenType::Operand, "M"),
        std::make_pair<TokenType, std::string>(TokenType::Split, ",")};
}

bool define_parse(LineData* line)
{
    int cnt = 0;
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->second == "#DEFINE") {
            cnt++;
        }
    }
    if (0 == cnt) {
        return false;
    }
    if (1 < cnt) {
        line->error = true;
        line->errmsg = "Multiple #defines cannot be defined on a single line.";
        return false;
    }
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->second == "#DEFINE") {
            if (it != line->token.begin()) {
                line->error = true;
                line->errmsg = "#define must appear at the beginning of the line.";
                return false;
            }
            it->first = TokenType::Delete;
            it++;
            if (it == line->token.end() || it->first != TokenType::Other) {
                line->error = true;
                line->errmsg = "No definition name specified in #define.";
                return false;
            }
            it->first = TokenType::Delete;
            auto name = it->second;
            auto d = defineTable.find(name);
            if (d != defineTable.end()) {
                line->error = true;
                line->errmsg = "Duplicate definition name " + name + " in #define.";
                return false;
            }
            it++;
            if (it == line->token.end()) {
                defineTable[name].push_back(std::make_pair(TokenType::Delete, ""));
            } else {
                while (it != line->token.end()) {
                    if (name == it->second) {
                        line->error = true;
                        line->errmsg = name + " is included in #define " + name + ".";
                        return false;
                    }
                    defineTable[name].push_back(std::make_pair(it->first, it->second));
                    it->first = TokenType::Delete;
                    it++;
                }
                // nametable に定義名を追加
                nametable_add(name, line);
                // ドット付きの場合は左辺が未登録なら登録
                auto dot = name.find('.');
                if (-1 != dot) {
                    auto left = name.substr(0, dot);
                    if (nameTable.find(left) == nameTable.end()) {
                        nametable_add(left, line);
                        nametable_add(left + ".", line);
                    }
                }
            }
            return true;
        }
    }
    return false;
}

void define_replace(LineData* line)
{
    bool replace = true;
    int tryCount = 0;
    int maxTryCount = (int)line->token.size() + 1;
    while (replace) {
        replace = false;
        tryCount++;
        if (maxTryCount < tryCount) {
            line->error = true;
            line->errmsg = "Detect circular references in #define definition names.";
            break;
        }
        for (auto it = line->token.begin(); it != line->token.end(); it++) {
            auto d = defineTable.find(it->second);
            if (d != defineTable.end()) {
                line->token.erase(it);
                line->token.insert(it, d->second.begin(), d->second.end());
                replace = true;
                break;
            } else {
                auto dot = it->second.find(".");
                if (-1 != dot) {
                    auto left = it->second.substr(0, dot);
                    auto right = it->second.substr(dot + 1);
                    if (defineTable.find(left + ".") != defineTable.end()) {
                        line->error = true;
                        line->errmsg = "`" + right + "` was not defined at `" + left + "`";
                    } else {
                        for (auto d : defineTable) {
                            dot = d.first.find(".");
                            if (-1 != dot) {
                                if (d.first.substr(0, dot) == left) {
                                    line->error = true;
                                    line->errmsg = "`" + right + "` was not defined at `" + left + "`";
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
