#include "common.h"

// 特定箇所の BrakectBegin ~ BracketEnd を AddressBegin ~ AddressEnd に置き換える
void bracket_to_address(LineData* line)
{
    std::vector<std::pair<TokenType, std::string>>::iterator prev;

    // 条件に該当するBrakectBegin を探索
    prev = line->token.end();
    int nest = 0;
    bool isAddress = false;
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::BracketBegin) {
            nest++;
            if (1 == nest && !isAddress) {
                // 直前のトークンがニーモニック or コンマなら Address にする
                if (prev != line->token.end() && (TokenType::Mnemonic == prev->first || TokenType::Split == prev->first)) {
                    it->first = TokenType::AddressBegin;
                    isAddress = true;
                }
            }
        } else if (it->first == TokenType::BracketEnd) {
            nest--;
            if (0 == nest && isAddress) {
                // AddressBegin に対応する AddressEnd を設定
                it->first = TokenType::AddressEnd;
                isAddress = false;
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
    prev = line->token.end();
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::Numeric) {
            next = it + 1;
            if (prev != line->token.end() && next != line->token.end()) {
                if (prev->first == TokenType::BracketBegin && next->first == TokenType::BracketEnd) {
                    line->token.erase(next);
                    line->token.erase(prev);
                    return true;
                }
            }
        }
        prev = it;
    }
    return false;
}