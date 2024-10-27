/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

std::string formulas_evaluate(std::vector<std::pair<TokenType, std::string>>* token)
{
#if 0
    for (auto t : *token) {
        printf(" %s", t.second.c_str());
    }
    puts("");
#endif

    std::vector<std::pair<TokenType, std::string>>::iterator prev;
    std::vector<std::pair<TokenType, std::string>>::iterator next;
    bool calc;

    // 掛け算と割り算を計算
    calc = true;
    while (calc) {
        calc = false;
        prev = token->end();
        for (auto it = token->begin(); it != token->end() && it->first != TokenType::BracketEnd; it++) {
            if (it->first == TokenType::BracketBegin) {
                std::vector<std::pair<TokenType, std::string>> work;
                int nest = 1;
                for (auto itW = it + 1; nest; token->erase(itW)) {
                    if (itW->first == TokenType::BracketEnd) {
                        if (0 == --nest) {
                            break;
                        }
                    } else if (itW->first == TokenType::BracketBegin) {
                        nest++;
                    }
                    work.push_back(std::make_pair(itW->first, itW->second));
                }
                formulas_evaluate(&work);
                token->erase(it + 1); // remove )
                token->erase(it);     // remove (
                for (int i = 0; i < work.size(); i++) {
                    token->insert(it + i, work[i]);
                }
                it = token->begin();
            }
            if (it->first == TokenType::Mul || it->first == TokenType::Div) {
                // 直前のトークンが　Numeric 型かチェック
                if (token->end() == prev || prev->first != TokenType::Numeric) {
                    return "Illegal expression with no specification of the number of operations to be performed.";
                }
                // 次のトークンが　Numeric 型かチェック
                next = it + 1;
                if (next == token->end() || next->first != TokenType::Numeric) {
                    if (next != token->end() && TokenType::BracketBegin == next->first) {
                        continue;
                    }
                    return "Illegal expression with no specification of the number of operations to be performed.";
                }
                // 計算を実行
                // printf("calc: %s %s %s\n", prev->second.c_str(), it->second.c_str(), next->second.c_str());
                int result = atoi(prev->second.c_str());
                if (it->first == TokenType::Mul) {
                    result *= atoi(next->second.c_str());
                } else {
                    int value = atoi(next->second.c_str());
                    if (0 == value) {
                        return "Division by zero.";
                    }
                    result /= value;
                }
                prev->second = std::to_string(result); // prev を演算結果に書き換え
                token->erase(next);                    // 掛けた or 割った 数は削除
                token->erase(it);                      // * or / は削除
                calc = true;                           // 演算を実行したので再度評価を実施
                break;
            }
            prev = it;
        }
    }

    // 足し算と引き算を評価
    calc = true;
    while (calc) {
        calc = false;
        prev = token->end();
        for (auto it = token->begin(); it != token->end() && it->first != TokenType::BracketEnd; it++) {
            if (it->first == TokenType::Plus || it->first == TokenType::Minus) {
                // 直前のトークンが　Numeric 型かチェック
                if (token->end() == prev || prev->first != TokenType::Numeric) {
                    // 直前のトークンが Operand の場合は無視（IX+dなどに対応するため）
                    if (token->end() != prev && prev->first == TokenType::Operand) {
                        continue;
                    }
                    return "Illegal expression with no specification of the number of operations to be performed.";
                }
                // 次のトークンが　Numeric 型かチェック
                next = it + 1;
                if (next == token->end() || next->first != TokenType::Numeric) {
                    // 次のトークンが BracketBegin の場合は無視
                    if (token->end() != next && next->first == TokenType::BracketBegin) {
                        continue;
                    }
                    return "Illegal expression with no specification of the number of operations to be performed.";
                }
                // 計算を実行
                // printf("calc: %s %s %s\n", prev->second.c_str(), it->second.c_str(), next->second.c_str());
                int result = atoi(prev->second.c_str());
                if (it->first == TokenType::Plus) {
                    result += atoi(next->second.c_str());
                } else {
                    result -= atoi(next->second.c_str());
                }
                prev->second = std::to_string(result); // prev を演算結果に書き換え
                token->erase(next);                    // 足した or引いた 数は削除
                token->erase(it);                      // * or / は削除
                calc = true;                           // 演算を実行したので再度評価を実施
                break;
            }
            prev = it;
        }
    }
    return "";
}

void formulas_evaluate(LineData* line)
{
    // sizeof, offset または 演算子前後が Other の演算を検出した行の計算をスキップ
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        switch (it->first) {
            case TokenType::SizeOf:
            case TokenType::Offset:
                return;
            case TokenType::Plus:
            case TokenType::Minus:
            case TokenType::Mul:
            case TokenType::Div:
                if (it != line->token.begin()) {
                    if ((it - 1)->first == TokenType::Other) {
                        return;
                    }
                }
                if (it + 1 != line->token.end()) {
                    if ((it + 1)->first == TokenType::Other) {
                        return;
                    }
                }
                break;
        }
    }

    // 計算を実行
    auto errmsg = formulas_evaluate(&line->token);
    if (!errmsg.empty()) {
        line->error = true;
        line->errmsg = errmsg;
    }
}

void formulas_evaluate_array(LineData* line)
{
    std::vector<std::pair<TokenType, std::string>> before;
    std::vector<std::pair<TokenType, std::string>> elements;
    std::vector<std::pair<TokenType, std::string>> after;
    int skip = 0;
    bool retry = true;

    while (retry) {
        retry = false;
        int count = 0;
        for (auto it = line->token.begin(); it != line->token.end(); it++) {
            before.push_back(std::make_pair(it->first, it->second));
            if (it->first == TokenType::ArrayBegin) {
                count++;
                if (skip < count) {
                    for (it++; it->first != TokenType::ArrayEnd; it++) {
                        elements.push_back(std::make_pair(it->first, it->second));
                    }
                    after.push_back(std::make_pair(it->first, it->second));
                    for (it++; it != line->token.end(); it++) {
                        after.push_back(std::make_pair(it->first, it->second));
                    }
                    auto error = formulas_evaluate(&elements);
                    if (!error.empty()) {
                        line->error = true;
                        line->errmsg = error;
                        return;
                    }
                    line->token.clear();
                    line->token.insert(line->token.end(), before.begin(), before.end());
                    line->token.insert(line->token.end(), elements.begin(), elements.end());
                    line->token.insert(line->token.end(), after.begin(), after.end());
                    before.clear();
                    elements.clear();
                    after.clear();
                    skip++;
                    retry = true;
                    break;
                }
            }
        }
    }
}
