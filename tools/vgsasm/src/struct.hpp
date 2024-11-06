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

void struct_parse_name(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::Other) {
            // A.B (ドットあり 2 トークン) または A (ドットなし) ならチェック
            auto token = it->second;
            if (token.c_str()[0] == '.') {
                // 構造体ではない or 構造体配列のフィールド指定なので無視
                continue;
            }
            auto tokens = split_token(token, '.');
            if (1 == tokens.size()) {
                // 1 トークンの場合構造体名であればスタートアドレスに置換
                auto str = structTable.find(tokens[0]);
                if (str != structTable.end()) {
                    it->first = TokenType::StructName;
                } else {
                    // 構造体ではないので無視
                }
            } else if (2 == tokens.size()) {
                // 2 トークンの場合構造体名のフィールド名に該当するアドレスに置換
                auto str = structTable.find(tokens[0]);
                if (str != structTable.end()) {
                    bool found = false;
                    for (auto field : str->second->fields) {
                        if (field->name == tokens[1]) {
                            it->first = TokenType::StructName;
                            found = true;
                            break;
                        }
                    }
                    // フィールド名が見つからなかった場合はエラー
                    if (!found) {
                        line->error = true;
                        line->errmsg = "Unknown field name in structure " + tokens[0] + ": " + tokens[1];
                    }
                } else {
                    // 構造体ではないので無視
                }
            } else {
                // 構造体ではないので無視
            }
        }
    }
}

void struct_parse_array(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::StructName) {
            auto str = structTable.find(it->second);
            if (++it == line->token.end()) {
                return;
            }
            if (it->first != TokenType::ArrayBegin) {
                continue;
            }
            (it - 1)->first = TokenType::Delete;
            it->first = TokenType::Delete;
            // ArrayBegin があれば同一行内に必ず ArrayEnd がある
            if ((++it)->first != TokenType::Numeric) {
                line->error = true;
                line->errmsg = "Illegal array structure element: " + it->second;
                return;
            }
            auto arrayNum = atoi(it->second.c_str());
            it->first = TokenType::Delete;
            if ((++it)->first != TokenType::ArrayEnd) {
                line->error = true;
                line->errmsg = "Illegal array structure element: " + it->second;
                return;
            }
            it->first = TokenType::Numeric;
            if ((it + 1)->first == TokenType::Other && (it + 1)->second.c_str()[0] == '.') {
                (it + 1)->first = TokenType::Delete;
                auto fieldName = (it + 1)->second.substr(1);
                bool found = false;
                int offset = 0;
                for (auto field : str->second->fields) {
                    if (field->name == fieldName) {
                        found = true;
                        it->second = std::to_string(str->second->start + str->second->size * arrayNum + offset);
                        break;
                    }
                    offset += field->size;
                }
                if (!found) {
                    line->error = true;
                    line->errmsg = "Undefined field name `" + fieldName + "` in struct `" + str->second->name + "` was specified.";
                    return;
                }
            } else {
                it->second = std::to_string(str->second->start + str->second->size * arrayNum);
            }
        }
    }
}

void struct_replace(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::StructName) {
            auto token = it->second;
            auto tokens = split_token(token, '.');
            auto str = structTable.find(tokens[0]);
            if (tokens.size() == 1) {
                if (it + 1 != line->token.end() && (it + 1)->first == TokenType::ArrayBegin) {
                    // skip replace array style
                } else {
                    it->first = TokenType::Numeric;
                    it->second = std::to_string(str->second->start);
                }
            } else {
                it->first = TokenType::Numeric;
                for (auto field : str->second->fields) {
                    if (field->name == tokens[1]) {
                        it->second = std::to_string(field->address);
                    }
                }
            }
        }
    }
}
