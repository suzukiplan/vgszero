#include "common.h"

void init_define()
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

bool parse_define(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->second == "#DEFINE") {
            it++;
            if (it == line->token.end() || it->first != TokenType::Other) {
                line->error = true;
                line->errmsg = "No definition name specified in #define.";
                return false;
            }
            auto name = it->second;
            auto d = defineTable.find(name);
            if (d != defineTable.end()) {
                line->error = true;
                line->errmsg = "Duplicate definition name " + name + " in #define";
                return false;
            }
            defineTable[name].push_back(std::make_pair(TokenType::Delete, ""));
            it++;
            while (it != line->token.end()) {
                defineTable[name].push_back(std::make_pair(it->first, it->second));
                it++;
            }
            return true;
        }
    }
    return false;
}

void replace_define(LineData* line)
{
    bool result = false;
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
                result = true;
                break;
            }
        }
    }
}