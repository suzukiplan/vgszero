/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

std::map<std::string, Macro*> macroTable;
static std::map<std::string, bool> dupTable;

static bool check_caller_dup(Macro* macro)
{
    if (dupTable.end() != dupTable.find(macro->name)) {
        return true;
    }
    dupTable[macro->name] = true;
    for (auto caller : macro->caller) {
        if (check_caller_dup(caller)) {
            return true;
        }
    }
    return false;
}

void parse_macro(LineData* line)
{
    if (line->token.empty()) {
        return;
    }
    auto it = line->token.begin();
    if (it->second != "#MACRO") {
        return;
    }
    it->first = TokenType::Macro;
    it++;
    if (it == line->token.end() || it->first != TokenType::Other) {
        line->error = true;
        line->errmsg = "Invalid #macro syntax.";
        return;
    }
    it->first = TokenType::Delete;
    auto name = it->second;
    (it - 1)->second = name;
    it++;

    // 重複チェック
    if (macroTable.find(name) != macroTable.end()) {
        line->error = true;
        line->errmsg = "Duplicate #macro names are defined: " + name;
        return;
    }
    addNameTable(name, line);
    if (line->error) {
        return;
    }

    // テーブル登録
    auto macro = new Macro(name, line);
    macroTable[name] = macro;

    // マクロ名の次がカッコになっているかチェック
    if (it == line->token.end() || it->first != TokenType::BracketBegin) {
        line->error = true;
        line->errmsg = "Invalid #macro syntax. (Parentheses)";
        return;
    }
    it->first = TokenType::Delete;
    it++;

    // 引数リストを取得
    bool first = true;
    for (; it != line->token.end() && it->first != TokenType::BracketEnd; it++) {
        if (!first) {
            // 第2引数以降はコンマで区切られているかチェック
            if (it->first != TokenType::Split) {
                line->error = true;
                line->errmsg = "Invalid #macro syntax. (Arguments not separated by commas)";
                return;
            }
            it->first = TokenType::Delete;
            it++;
            if (it == line->token.end()) {
                break;
            }
        }
        first = false;
        // 引数名が Other かチェック
        if (it->first != TokenType::Other) {
            line->error = true;
            line->errmsg = "Invalid #macro syntax. (Invalid argument name: " + it->second + ")";
            return;
        }
        // 重複する引数名が登録されていないかチェック
        for (auto a : macro->args) {
            if (a == it->second) {
                line->error = true;
                line->errmsg = "Invalid #macro syntax. (Duplicate argument names: " + it->second + ")";
                return;
            }
        }
        macro->args.push_back(it->second);
        it->first = TokenType::Delete;
    }
    if (it == line->token.end()) {
        line->error = true;
        line->errmsg = "Invalid #macro syntax. (Parentheses not closed)";
        return;
    }
    it->first = TokenType::Delete;
    return;
}

void macro_syntax_check(std::vector<LineData*>* lines)
{
    for (auto it = macroTable.begin(); it != macroTable.end(); it++) {
        for (auto arg : it->second->args) {
            if (checkNameTable(arg)) {
                it->second->refer->error = true;
                it->second->refer->errmsg = "A macro argument name conflict with the name used for structs, defines, etc.: " + arg;
            }
        }
    }

    bool searchScopeBegin = false;
    bool storeTokenToMacro = false;
    Macro* macro = nullptr;
    LineData* newLine = nullptr;
    for (auto it = lines->begin(); it != lines->end(); it++) {
        auto line = *it;
        for (auto token = line->token.begin(); token != line->token.end(); token++) {
            if (storeTokenToMacro) {
                if (!newLine) {
                    newLine = new LineData(line);
                    newLine->token.clear();
                }
                if (token->first == TokenType::ScopeEnd) {
                    if (token + 1 != line->token.end()) {
                        line->error = true;
                        line->errmsg = "Invalid #macro syntax. (Unexpected token: " + (token + 1)->second + ")";
                    }
                    token->first = TokenType::Delete;
                    storeTokenToMacro = false;
                    searchScopeBegin = false;
                    break;
                }
                newLine->token.push_back(std::make_pair(token->first, token->second));
                if (token->first == TokenType::Label || token->first == TokenType::LabelAt) {
                    line->error = true;
                    line->errmsg = "Invalid #macro syntax. (Label in macro: " + token->second + ")";
                }
                token->first = TokenType::Delete;
            } else if (searchScopeBegin) {
                if (token->first == TokenType::ScopeBegin) {
                    token->first = TokenType::Delete;
                    storeTokenToMacro = true;
                } else {
                    line->error = true;
                    line->errmsg = "Invalid #macro syntax. (Unexpected token: " + token->second + ")";
                }
            } else if (token->first == TokenType::Macro) {
                token->first = TokenType::Delete;
                macro = macroTable[token->second];
                searchScopeBegin = true;
            }
        }
        if (newLine) {
            if (newLine->token.empty()) {
                delete newLine;
            } else {
                macro->lines.push_back(newLine);
            }
            newLine = nullptr;
        }
    }

    // Other -> MacroCaller
    for (auto it = lines->begin(); it != lines->end(); it++) {
        auto line = *it;
        for (auto token = line->token.begin(); token != line->token.end(); token++) {
            if (token->first == TokenType::Other) {
                if (macroTable.find(token->second) != macroTable.end()) {
                    token->first = TokenType::MacroCaller;
                }
            }
        }
    }

    // Other -> MacroCaller in Macro
    for (auto m = macroTable.begin(); m != macroTable.end(); m++) {
        for (auto it = m->second->lines.begin(); it != m->second->lines.end(); it++) {
            auto line = *it;
            for (auto token = line->token.begin(); token != line->token.end(); token++) {
                if (token->first == TokenType::Other) {
                    auto caller = macroTable.find(token->second);
                    if (caller != macroTable.end()) {
                        token->first = TokenType::MacroCaller;
                        m->second->caller.push_back(caller->second);
                    }
                }
            }
        }
    }

    // 循環チェック
    for (auto m = macroTable.begin(); m != macroTable.end(); m++) {
        dupTable.clear();
        if (check_caller_dup(m->second)) {
            m->second->refer->error = true;
            m->second->refer->errmsg = "Macro calls are circulating: " + m->first;
        }
    }

    // 取得したマクロを全表示
#if 0
    for (auto m : macroTable) {
        printf("macro: %s (", m.first.c_str());
        bool f = true;
        for (auto arg : m.second->args) {
            if (!f) {
                printf(", ");
            }
            f = false;
            printf("%s", arg.c_str());
        }
        printf(")\n");
        for (auto l : m.second->lines) {
            printf("    ");
            l->printDebug();
        }
    }
#endif
}

void extract_macro_call(std::vector<LineData*>* lines)
{
    for (auto it = lines->begin(); it != lines->end(); it++) {
        // マクロ呼び出し行かチェック
        auto line = *it;
        if (line->token.empty() || line->token[0].first != TokenType::MacroCaller) {
            continue;
        }
        auto mit = macroTable.find(line->token[0].second);
        if (mit == macroTable.end()) {
            continue;
        }
        // マクロを検出
        auto macro = mit->second;
        auto token = line->token.begin() + 1;

        // 引数リストを作成
        std::vector<std::pair<TokenType, std::string>> args;
        bool first = true;
        for (; token != line->token.end(); token++) {
            if (token->first == TokenType::BracketEnd) {
                break;
            }
            if (!first) {
                if (token->first != TokenType::Split) {
                    line->error = true;
                    line->errmsg = "Unexpected macro argument symbol: " + token->second;
                    break;
                }
                token++;
                if (token == line->token.end()) {
                    line->error = true;
                    line->errmsg = "Invalid macro argument syntax.";
                    break;
                }
            }
            first = false;
            if (token->first != TokenType::Numeric && token->first != TokenType::LabelJump) {
                line->error = true;
                line->errmsg = "Invalid macro argument type: " + token->second;
                break;
            }
            args.push_back(std::make_pair(token->first, token->second));
        }
        if (line->error) {
            continue;
        }

        // 引数の数が定義と一致するかチェック
        if (macro->args.size() != args.size()) {
            line->error = true;
            line->errmsg = "Macro argument mismatch: " + macro->name;
            continue;
        }

        // 引数を展開したコードリストを作成
        std::vector<LineData*> newLines;
        for (auto ml : macro->lines) {
            auto newLine = new LineData(ml);
            for (auto mt = newLine->token.begin(); mt != newLine->token.end(); mt++) {
                for (int i = 0; i < macro->args.size(); i++) {
                    if (macro->args[i] == mt->second) {
                        mt->first = args[i].first;
                        mt->second = args[i].second;
                    }
                }
            }
            newLines.push_back(newLine);
        }

        // 新しいコードリストへ置き換える
        lines->erase(it);
        lines->insert(it, newLines.begin(), newLines.end());

        // 再び行頭から探索
        it = lines->begin();
    }
}
