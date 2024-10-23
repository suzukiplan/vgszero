#include "common.h"

void parse_binary(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->second == "#BINARY") {
            it->first = TokenType::Binary;
            if (it + 1 == line->token.end() || (it + 1)->first != TokenType::String) {
                line->error = true;
                line->errmsg = "#binary syntax error.";
                return;
            }
            it->second = (it + 1)->second;
            it++;
            it->first = TokenType::Delete;
            auto path = it->second.c_str();

            int offset = 0;
            int size = 0;
            if (it + 1 != line->token.end() && (it + 1)->first == TokenType::Split &&
                it + 2 != line->token.end() && (it + 2)->first == TokenType::Numeric) {
                offset = atoi((it + 2)->second.c_str());
                if (offset < 0) {
                    line->error = true;
                    line->errmsg = "#binary invalid offset: " + std::to_string(offset);
                    return;
                }
                (it + 1)->first = TokenType::Delete;
                (it + 2)->first = TokenType::Delete;
                it += 2;
                if (it + 1 != line->token.end() && (it + 1)->first == TokenType::Split &&
                    it + 2 != line->token.end() && (it + 2)->first == TokenType::Numeric) {
                    size = atoi((it + 2)->second.c_str());
                    if (size < 1) {
                        line->error = true;
                        line->errmsg = "#binary invalid size: " + std::to_string(size);
                        return;
                    }
                    (it + 1)->first = TokenType::Delete;
                    (it + 2)->first = TokenType::Delete;
                    it += 2;
                }
            }

            char basePath[4096];
            if ('/' != path[0]) {
                strcpy(basePath, line->path.c_str());
                char* cp = strrchr(basePath, '/');
                if (cp) {
                    cp++;
                    *cp = 0;
                    strcat(basePath, path);
                } else {
                    strcpy(basePath, path);
                }
                path = basePath;
            }

            auto fp = fopen(path, "rb");
            if (!fp) {
                line->error = true;
                line->errmsg = "#binary file open error: ";
                line->errmsg += path;
                return;
            }

            if (0 == size) {
                fseek(fp, 0, SEEK_END);
                size = ftell(fp);
            }

            if (fseek(fp, offset, SEEK_SET) < 0) {
                fclose(fp);
                line->error = true;
                line->errmsg = "#binary seek failed: " + std::to_string(offset);
                return;
            }

            if (size < 0) {
                line->error = true;
                line->errmsg = "I/O error";
                fclose(fp);
                return;
            }

            if (0 < size) {
                auto buf = (uint8_t*)malloc(size);
                if (!buf) {
                    line->error = true;
                    line->errmsg = "No memory.";
                    fclose(fp);
                    return;
                }
                if (size != fread(buf, 1, size, fp)) {
                    line->error = true;
                    line->errmsg = "I/O error";
                    free(buf);
                    fclose(fp);
                    return;
                }
                for (int i = 0; i < (int)size; i++) {
                    line->machine.push_back(buf[i]);
                }
                free(buf);
            }

            fclose(fp);
        }
    }
}

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

static int lastOrg = -1;

void parse_org(LineData* line)
{
    if (line->token.empty()) {
        return;
    }
    if (line->token[0].second != "ORG") {
        return;
    }
    if (line->token.size() != 2 || line->token[1].first != TokenType::Numeric) {
        line->error = true;
        line->errmsg = "ORG formatting is incorrect.";
        return;
    }
    auto org = atoi(line->token[1].second.c_str());
    if (0xFFFF < org || org < 0) {
        line->error = true;
        line->errmsg = "org is not the correct address (0x0000 to 0xFFFF)";
        return;
    }
    if (lastOrg >= org) {
        line->error = true;
        line->errmsg = "Multiple orgs must be written in ascending order from the beginning of the line.";
        return;
    }
    lastOrg = org;

    line->token[0].first = TokenType::Org;
    line->token[0].second = line->token[1].second;
    line->token.erase(line->token.begin() + 1);
}

void setpc_org(std::vector<LineData*>* lines)
{
    auto prev = lines->end();
    for (auto it = lines->begin(); it != lines->end(); it++) {
        // 直前の行が ORG の行を探す
        if (prev != lines->end() && !(*prev)->token.empty() && (*prev)->token[0].first == TokenType::Org) {
            (*it)->programCounterInit = true;
            (*it)->programCounter = atoi((*prev)->token[0].second.c_str());
            lines->erase(prev);
            it = lines->begin();
        }
        prev = it;
    }
}

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
