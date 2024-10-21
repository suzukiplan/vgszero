#include "common.h"

void parse_sizeof(LineData* line)
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
                    line->errmsg = "No name specified in sizeof syntax.";
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

void replace_sizeof(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::SizeOf) {
            auto s = structTable.find(it->second);
            if (s == structTable.end()) {
                line->error = true;
                line->errmsg = "Undefined structure " + it->second + " is specified in sizeof.";
                return;
            }
            it->first = TokenType::Numeric;
            it->second = std::to_string(s->second->size);
        }
    }
}