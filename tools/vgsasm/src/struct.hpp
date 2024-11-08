/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

void struct_parse(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->second == "STRUCT") {
            if (it != line->token.begin()) {
                line->error = true;
                line->errmsg = "`struct` must appear at the beginning of the line.";
            } else {
                it->first = TokenType::Struct;
            }
        }
    }
}

bool struct_syntax_check(std::vector<LineData*>* lines)
{
    // struct を探索
    TokenType expect = TokenType::None;
    Struct* newStruct = nullptr;
    StructField* newField = nullptr;
    bool searchField = false;

    for (auto it1 = lines->begin(); it1 != lines->end(); it1++) {
        auto line = *it1;
        newField = nullptr;
        for (auto it2 = line->token.begin(); it2 != line->token.end(); it2++) {
            if (searchField) {
                if (it2->first == TokenType::ScopeEnd) {
                    it2->first = TokenType::Delete;
                    return true;
                } else {
                    if (newField) {
                        newField->token.push_back(std::make_pair(it2->first, it2->second));
                        it2->first = TokenType::Delete;
                    } else {
                        if (-1 != it2->second.find(".")) {
                            line->error = true;
                            line->errmsg = "Invalid structure field name: " + it2->second;
                            return false;
                        }
                        for (auto field : newStruct->fields) {
                            if (field->name == it2->second) {
                                line->error = true;
                                line->errmsg = "Duplicate field name `" + it2->second + "` in struct `" + newStruct->name + "`";
                                return false;
                            }
                        }
                        newField = new StructField(line, it2->second);
                        newStruct->fields.push_back(newField);
                        it2->first = TokenType::Delete;
                    }
                }
            } else if (TokenType::None == expect) {
                // searching... (Struct が見つかるまで探索を続ける状態)
                if (it2->first == TokenType::Struct) {
                    newStruct = nullptr;
                    expect = TokenType::Other;
                    it2->first = TokenType::Delete;
                }
            } else {
                if (it2->first != expect) {
                    if (it2->first == TokenType::ArrowLeft && expect == TokenType::Numeric) {
                        it2->first = TokenType::Delete;
                        it2++;
                        if (it2 == line->token.end() || newStruct->name == it2->second || it2->first != TokenType::Other) {
                            line->error = true;
                            line->errmsg = "Incorrect syntax for struct arrow.";
                            return false;
                        }
                        newStruct->start = -1;
                        newStruct->after = it2->second;
                        it2->first = TokenType::Delete;
                        expect = TokenType::ScopeBegin;
                        if (it2 + 1 == line->token.end()) {
                            continue;
                        }
                        if ((it2 + 1)->first == TokenType::ArrayBegin) {
                            it2++;
                            it2->first = TokenType::Delete;
                            it2++;
                            if (it2->first != TokenType::Numeric) {
                                line->error = true;
                                line->errmsg = "Unexpected symbol: " + it2->second;
                                return false;
                            }
                            it2->first = TokenType::Delete;
                            newStruct->afterArray = atoi(it2->second.c_str());
                            it2++;
                            if (it2 == line->token.end()) {
                                line->error = true;
                                line->errmsg = "Incorrect syntax for struct array.";
                                return false;
                            } else if (it2->first != TokenType::ArrayEnd) {
                                line->error = true;
                                line->errmsg = "Unexpected symbol: " + it2->second;
                                return false;
                            } else if (newStruct->afterArray < 1) {
                                line->error = true;
                                line->errmsg = "struct array must have a value of 1 or more: " + std::to_string(newStruct->afterArray);
                                return false;
                            }
                            it2->first = TokenType::Delete;
                            continue;
                        } else {
                            continue;
                        }
                    } else {
                        line->error = true;
                        line->errmsg = "Incorrect syntax for struct: " + it2->second;
                        return false;
                    }
                }
                switch (expect) {
                    case TokenType::Other:
                        if (structTable.end() != structTable.find(it2->second)) {
                            line->error = true;
                            line->errmsg = "Duplicate structure name: " + it2->second;
                            return false;
                        }
                        if (-1 != it2->second.find(".")) {
                            line->error = true;
                            line->errmsg = "Invalid structure name: " + it2->second;
                            return false;
                        }
                        nametable_add(it2->second, line);
                        newStruct = new Struct(line, it2->second);
                        structTable[it2->second] = newStruct;
                        structNameList.push_back(newStruct->name);
                        if (it2 + 1 == line->token.end() || (it2 + 1)->first == TokenType::ScopeBegin) {
                            newStruct->start = 0;
                            expect = TokenType::ScopeBegin;
                        } else {
                            expect = TokenType::Numeric;
                        }
                        break;
                    case TokenType::Numeric:
                        newStruct->start = atoi(it2->second.c_str());
                        expect = TokenType::ScopeBegin;
                        break;
                    case TokenType::ScopeBegin:
                        searchField = true;
                        break;
                }
                it2->first = TokenType::Delete;
            }
        }
    }

    return false;
}

bool struct_check_size()
{
    bool needRetry = false;
    for (auto name : structNameList) {
        auto s = structTable.find(name);
        if (!s->second->after.empty()) {
            auto after = structTable.find(s->second->after);
            if (0 == after->second->size) {
                s->second->line->error = true;
                s->second->line->errmsg = "It must be defined on the line after the definition of struct specified by the arrow operator.";
            }
            if (0 < s->second->afterArray) {
                s->second->start = after->second->start + after->second->size * s->second->afterArray;
            } else {
                s->second->start = after->second->start + after->second->size;
            }
        }
        int sizeOfStruct = 0;
        int startAddress = s->second->start;
        bool skip = false;
        for (auto f = s->second->fields.begin(); f != s->second->fields.end(); f++) {
            auto field = *f;
            if (field->token.size() != 2) {
                field->line->error = true;
                field->line->errmsg = "Invalid field " + field->name + " in structure " + s->first;
                continue;
            }
            if (TokenType::Numeric != field->token[1].first) {
                field->line->error = true;
                field->line->errmsg = "Specify non-numeric number of field " + field->name + " in structure " + s->first + ": " + field->token[1].second;
                continue;
            }
            if (field->token[0].second == "DS.B") {
                field->size = 1;
            } else if (field->token[0].second == "DS.W") {
                field->size = 2;
            } else {
                field->typeName = field->token[0].second;
                auto ss = structTable.find(field->token[0].second);
                if (ss == structTable.end()) {
                    field->line->error = true;
                    field->line->errmsg = "Unknown structure name " + field->token[0].second + " specified in field " + field->name + " of structure " + s->first;
                    continue;
                }
                if (0 == ss->second->size) {
                    // 構造体サイズが確定していないのでリトライを要求してスキップ
                    needRetry = true;
                    skip = true;
                    continue;
                }
                field->size = ss->second->size;
            }
            field->count = atoi(field->token[1].second.c_str());
            field->address = startAddress;
            startAddress += field->size * field->count;
            sizeOfStruct += field->size * field->count;
        }
        if (!skip) {
            s->second->size = sizeOfStruct;
        }
    }
    return needRetry;
}

int struct_calc_array_size(LineData* line, std::vector<std::pair<TokenType, std::string>>* tokens)
{
    if (tokens->empty()) {
        line->error = true;
        line->errmsg = "Specify array for elements that are not structures.";
        return -1;
    }
    for (auto token = tokens->begin(); token != tokens->end(); token++) {
        if (token->first == TokenType::Other) {
            auto names = split_token(token->second, '.');
            if (names.empty() || names[0].empty()) {
                line->error = true;
                line->errmsg = "Unexpected symbol: " + token->second;
                return -1;
            }
            // 最初の name が struct かチェック
            auto str = structTable.find(names[0]);
            if (str == structTable.end()) {
                line->error = true;
                line->errmsg = "Unexpected symbol: " + token->second;
                return -1;
            }
            auto result = str->second->start;
            // 2番目以降をチェック
            for (int i = 1; i < names.size(); i++) {
                bool found = false;
                for (auto field : str->second->fields) {
                    if (field->name == names[i]) {
                        found = true;
                        if (!field->typeName.empty()) {
                            str = structTable.find(field->typeName);
                        }
                        break;
                    } else {
                        result += field->size * field->count;
                    }
                }
                if (!found) {
                    line->error = true;
                    line->errmsg = "Undefined field: " + names[i];
                    return -1;
                }
            }
            token->first = TokenType::Numeric;
            token->second = std::to_string(result);
        }
    }
    auto errmsg = formulas_evaluate(tokens);
    if (!errmsg.empty()) {
        line->error = true;
        line->errmsg = errmsg;
        return -1;
    }
    if (tokens->size() != 1) {
        line->error = true;
        line->errmsg = "Illegal array structure elements:";
        for (auto token : *tokens) {
            line->errmsg = line->errmsg + " " + token.second;
        }
        return -1;
    }
    return atoi(tokens->at(0).second.c_str());
}

void struct_array_replace(std::vector<LineData*>* lines)
{
    for (auto it = lines->begin(); it != lines->end(); it++) {
        auto line = *it;
        for (auto token = line->token.begin(); token != line->token.end(); token++) {
            if (token->first == TokenType::ArrayBegin) {
                token->first = TokenType::Delete;
                std::vector<std::pair<TokenType, std::string>> arrayTokens;
                for (++token; token->first != TokenType::ArrayEnd; token++) {
                    arrayTokens.push_back(std::make_pair(token->first, token->second));
                    token->first = TokenType::Delete;
                }
                token->first = TokenType::Array;
                int size = struct_calc_array_size(line, &arrayTokens);
                if (size < 0) {
                    return;
                }
                token->second = std::to_string(size);
            }
        }
    }
}

void struct_replace(std::vector<LineData*>* lines)
{
    for (auto it = lines->begin(); it != lines->end(); it++) {
        auto line = *it;
        for (auto token = line->token.begin(); token != line->token.end(); token++) {
            if (token->first == TokenType::Other) {
                int result = 0;
                bool needCheck = true;
                bool isTop = true;
                Struct* str = nullptr;
                while (needCheck) {
                    auto names = split_token(token->second, '.');
                    if (names.empty() || names[0].empty()) {
                        line->error = true;
                        line->errmsg = "Unexpected symbol: " + token->second;
                        return;
                    }
                    for (int i = 0; i < names.size(); i++) {
                        if (str) {
                            bool found = false;
                            for (auto field : str->fields) {
                                if (field->name == names[i]) {
                                    found = true;
                                    if (!field->typeName.empty()) {
                                        str = structTable.find(field->typeName)->second;
                                    }
                                    break;
                                } else {
                                    result += field->size * field->count;
                                }
                            }
                            if (!found) {
                                line->error = true;
                                line->errmsg = "Undefined field: " + names[i];
                                return;
                            }
                        }
                        if (isTop) {
                            isTop = false;
                            auto s = structTable.find(names[i]);
                            if (s == structTable.end()) {
                                line->error = true;
                                line->errmsg = "Unexpected symbol: " + names[i];
                                return;
                            }
                            str = s->second;
                            result = str->start;
                        }
                    }
                    // 次トークンが配列かチェック
                    if (token + 1 != line->token.end() && (token + 1)->first == TokenType::Array) {
                        if (!str) {
                            line->error = true;
                            line->errmsg = "Specify array for elements that are not structure: " + names[names.size() - 1];
                            return;
                        }
                        token->first = TokenType::Delete;
                        token++;
                        result += atoi(token->second.c_str()) * str->size;
                        // 次トークンがフィールドかチェック
                        if (token + 1 != line->token.end() && (token + 1)->first == TokenType::Other) {
                            token->first = TokenType::Delete;
                            token++;
                            // ドットで始まっていなければエラーにする
                            if (token->second.c_str()[0] != '.') {
                                line->error = true;
                                line->errmsg = "Unexpected symbol: " + token->second;
                                return;
                            }
                            // ドットを取り除く
                            token->second = token->second.substr(1);
                        } else {
                            token->first = TokenType::Numeric;
                            token->second = std::to_string(result);
                            needCheck = false;
                        }
                    } else {
                        token->first = TokenType::Numeric;
                        token->second = std::to_string(result);
                        needCheck = false;
                    }
                }
            }
        }
    }
}
