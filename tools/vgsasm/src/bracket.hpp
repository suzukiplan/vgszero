/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

static bool isOperator(TokenType type)
{
    switch (type) {
        case TokenType::Plus:
        case TokenType::Minus:
        case TokenType::Mul:
        case TokenType::Div:
            return true;
    }
    return false;
}

// 特定箇所の BrakectBegin ~ BracketEnd を AddressBegin ~ AddressEnd に置き換える
void bracket_to_address(LineData* line)
{
    std::vector<std::pair<TokenType, std::string>>::iterator prev;

    // 条件に該当するBrakectBegin を探索
    prev = line->token.end();
    int nest = 0;
    bool isAddress = false;
    auto begin = line->token.begin();
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::BracketBegin) {
            nest++;
            if (1 == nest && !isAddress) {
                // 直前のトークンがニーモニック or コンマなら Address にする
                if (prev != line->token.end() && (TokenType::Mnemonic == prev->first || TokenType::Split == prev->first)) {
                    it->first = TokenType::AddressBegin;
                    isAddress = true;
                    begin = it;
                }
            }
        } else if (it->first == TokenType::BracketEnd) {
            nest--;
            if (0 == nest && isAddress) {
                if (it + 1 == line->token.end() || !isOperator((it + 1)->first)) {
                    // AddressBegin に対応する AddressEnd を設定
                    it->first = TokenType::AddressEnd;
                    isAddress = false;
                } else {
                    // 次にトークンが演算子なので AddressBegin を BracketBegin に戻す
                    begin->first = TokenType::BracketBegin;
                }
            }
        }
        prev = it;
    }
}

// `(` `数値` `)` を `数値` に置き換える
bool bracket_eliminate(LineData* line)
{
    std::vector<std::pair<TokenType, std::string>>::iterator prev;
    std::vector<std::pair<TokenType, std::string>>::iterator next;

    // 条件に該当する Numeric を探索
    bool result = false;
    prev = line->token.end();
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::Numeric) {
            next = it + 1;
            if (prev != line->token.end() && next != line->token.end()) {
                if (prev->first == TokenType::BracketBegin && next->first == TokenType::BracketEnd) {
                    line->token.erase(next);
                    line->token.erase(prev);
                    result = true;
                    it = line->token.begin();
                    prev = line->token.end();
                }
            }
        }
        prev = it;
    }
    return result;
}
