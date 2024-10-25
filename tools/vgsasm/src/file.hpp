/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"
#include "sha1.hpp"
#include "mnemonic.h"

int arrayCount = 0;
int bracketCount = 0;
int scopeCount = 0;
LineData* lastScopeBegin = nullptr;

LineData::LineData(const char* path, int lineNumber, std::string text)
{
    this->path = path;
    this->lineNumber = lineNumber;
    this->text = text;
    this->programCounter = 0;
    this->programCounterInit = false;
    this->isAssignmnt = false;
    char formed[4096];
    memset(formed, 0, sizeof(formed));

    if (sizeof(formed) <= this->text.length()) {
        // 1行4096byte以上なのでエラー扱い
        this->error = true;
        this->errmsg = "One line is 4096 bytes or more.";
        return;
    } else {
        // 整形テキストを作成
        this->error = false;
        this->errmsg = "";
        strcpy(formed, this->text.c_str());

        // 行末が CR なら潰しておく
        auto len = strlen(formed);
        if (0 < len && formed[len - 1] == '\r') {
            formed[len - 1] = 0;
        }

        // 文字列リテラルを一時的に　0x01 に置換
        std::vector<std::string> slets;
        char* cp = formed;
        int ptr = 0;
        for (; cp[ptr]; ptr++) {
            if ('\"' == cp[ptr]) {
                ptr++;
                char* ed = &cp[ptr];
                while (*ed) {
                    if (*ed == '\"') {
                        break;
                    } else if (*ed == '\\') {
                        ed++;
                        if (0 == *ed) {
                            break;
                        }
                    }
                    ed++;
                }
                if (!*ed) {
                    this->error = true;
                    this->errmsg = "String literals are not closed with double quotes.";
                    return;
                } else {
                    ed--;
                    char slet[4096];
                    memset(slet, 0, sizeof(slet));
                    int len = (int)(ed - (cp + ptr)) + 1;
                    memcpy(slet, cp + ptr, len);
                    slets.push_back(slet);
                    for (int i = 0; i < len; i++) {
                        cp[ptr++] = 0x20;
                    }
                    ptr++;
                }
            }
        }

        // 文字リテラルを一時的に 0x02 に置換（※シングルクオートも含めて）
        std::vector<uint8_t> schrs;
        cp = formed;
        ptr = 0;
        for (; cp[ptr]; ptr++) {
            if ('\'' == cp[ptr]) {
                if ('\\' == cp[ptr + 1]) {
                    if (isdigit(cp[ptr + 2])) {
                        // 8進数
                        char* ep = &cp[ptr + 3];
                        while (0 != *ep && '\'' != *ep) { ep++; }
                        if (*ep) {
                            *ep = 0;
                            auto str = oct2dec(&cp[ptr + 2]);
                            if (!str.empty()) {
                                schrs.push_back(atoi(str.c_str()) & 0xFF);
                                while (&cp[ptr] != ep) {
                                    cp[ptr] = 0x02;
                                    ptr++;
                                }
                                *ep = 0x02;
                                ptr++;
                            } else {
                                *ep = '\'';
                            }
                        }
                    } else if ('x' == cp[ptr + 2] || 'X' == cp[ptr + 2]) {
                        // 16進数
                        char* ep = &cp[ptr + 3];
                        while (0 != *ep && '\'' != *ep) { ep++; }
                        if (*ep) {
                            *ep = 0;
                            auto str = hex2dec(&cp[ptr + 3]);
                            if (!str.empty()) {
                                schrs.push_back(atoi(str.c_str()) & 0xFF);
                                while (cp + ptr != ep) {
                                    cp[ptr] = 0x02;
                                    ptr++;
                                }
                                *ep = 0x02;
                                ptr++;
                            } else {
                                *ep = '\'';
                            }
                        }
                    } else if (0 != cp[ptr + 2] && '\'' == cp[ptr + 3]) {
                        cp[ptr++] = 0x02;
                        cp[ptr++] = 0x02;
                        auto esc = cp[ptr];
                        cp[ptr++] = 0x02;
                        cp[ptr++] = 0x02;
                        switch (tolower(esc)) {
                            case 'a': schrs.push_back('\a'); break;
                            case 'b': schrs.push_back('\b'); break;
                            case 'f': schrs.push_back('\f'); break;
                            case 'n': schrs.push_back('\n'); break;
                            case 'r': schrs.push_back('\r'); break;
                            case 't': schrs.push_back('\t'); break;
                            case 'v': schrs.push_back('\v'); break;
                            case '\\': schrs.push_back('\\'); break;
                            case '\?': schrs.push_back('\?'); break;
                            case '\'': schrs.push_back('\''); break;
                            case '\"': schrs.push_back('\"'); break;
                            default:
                                this->error = true;
                                this->errmsg = "Invalid escape sequence: \\";
                                this->errmsg += esc;
                                return;
                        }
                    }
                } else if (0 != cp[ptr + 1]) {
                    if ('\'' == cp[ptr + 2]) {
                        // アスキーコード
                        schrs.push_back(cp[ptr + 1]);
                        cp[ptr] = 0x02;
                        cp[ptr + 1] = 0x02;
                        cp[ptr + 2] = 0x02;
                        ptr += 2;
                    }
                }
            }
        }

        // コメント削除
        cp = strchr(formed, ';');
        if (cp) {
            *cp = 0;
        }
        cp = strstr(formed, "//");
        if (cp) {
            *cp = 0;
        }

        // 小文字は大文字、TABは空白に変換
        for (cp = formed; *cp; cp++) {
            if (islower(*cp)) {
                *cp = toupper(*cp);
            }
            if ('\t' == *cp) {
                *cp = ' ';
            }
        }

        // 前後の空白をトリム
        trim_string(formed);

        // トークン分割
        cp = formed;
        char* ed;
        bool parseEnd = false;
        int sletIndex = 0;
        int schrsIndex = 0;
        while (!parseEnd && !this->error) {
            switch (*cp) {
                case '\0':
                    parseEnd = true;
                    break;
                case ' ':
                    cp++;
                    break;
                case ',':
                    cp++;
                    this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Split, ","));
                    break;
                case '=':
                    cp++;
                    this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Equal, "="));
                    break;
                case '+':
                    if ('+' == *(cp + 1)) {
                        cp += 2;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Inc, "++"));
                    } else if ('=' == *(cp + 1)) {
                        cp += 2;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::EqualPlus, "+="));
                    } else {
                        cp++;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Plus, "+"));
                    }
                    break;
                case '<':
                    if ('<' == *(cp + 1) && '=' == *(cp + 2)) {
                        cp += 3;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::EqualShiftLeft, "<<="));
                    } else if ('-' == *(cp + 1)) {
                        cp += 2;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::ArrowLeft, "<-"));
                    }
                    break;
                case '>':
                    if ('>' == *(cp + 1) && '=' == *(cp + 2)) {
                        cp += 3;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::EqualShiftRight, ">>="));
                    }
                    break;
                case '-':
                    if ('-' == *(cp + 1)) {
                        cp += 2;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Dec, "--"));
                    } else if ('=' == *(cp + 1)) {
                        cp += 2;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::EqualMinus, "-="));
                    } else if ('>' == *(cp + 1)) {
                        cp += 2;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::ArrowRight, "->"));
                    } else {
                        cp++;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Minus, "-"));
                    }
                    break;
                case '/':
                    if (*(cp + 1) == '=') {
                        cp += 2;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::EqualDiv, "/="));
                    } else {
                        cp++;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Div, "/"));
                    }
                    break;
                case '*':
                    if (*(cp + 1) == '=') {
                        cp += 2;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::EqualMul, "*="));
                    } else {
                        cp++;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Mul, "*"));
                    }
                    break;
                case '%':
                    if (*(cp + 1) == '=') {
                        cp += 2;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::EqualMod, "%="));
                    } else if (isdigit(*(cp + 1))) {
                        for (ed = cp + 2; isdigit(*ed); ed++) {}
                        std::string str = cp;
                        puts((str.substr(0, ed - cp)).c_str());
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Other, str.substr(0, ed - cp)));
                        cp = ed;
                    } else {
                        cp++;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Modulo, "%"));
                    }
                    break;
                case '&':
                    if ('=' == *(cp + 1)) {
                        cp += 2;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::EqualAnd, "&="));
                    } else {
                        cp++;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::And, "&"));
                    }
                    break;
                case '|':
                    if ('=' == *(cp + 1)) {
                        cp += 2;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::EqualOr, "|="));
                    } else {
                        cp++;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Or, "|"));
                    }
                    break;
                case '^':
                    if ('=' == *(cp + 1)) {
                        cp += 2;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::EqualXor, "^="));
                    } else {
                        cp++;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Caret, "^"));
                    }
                    break;
                case '[':
                    cp++;
                    this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::ArrayBegin, "["));
                    arrayCount++;
                    break;
                case ']':
                    cp++;
                    this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::ArrayEnd, "]"));
                    arrayCount--;
                    if (arrayCount < 0) {
                        this->error = true;
                        this->errmsg = "Invalid `]` without corresponding `[`.";
                    }
                    break;
                case '(':
                    cp++;
                    this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::BracketBegin, "("));
                    bracketCount++;
                    break;
                case ')':
                    cp++;
                    this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::BracketEnd, ")"));
                    bracketCount--;
                    if (bracketCount < 0) {
                        this->error = true;
                        this->errmsg = "Invalid `)` without corresponding `(`.";
                    }
                    break;
                case '{':
                    cp++;
                    this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::ScopeBegin, "{"));
                    scopeCount++;
                    if (1 < scopeCount) {
                        this->error = true;
                        this->errmsg = "Duplicate `{` designation.";
                    } else {
                        lastScopeBegin = this;
                    }
                    break;
                case '}':
                    cp++;
                    this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::ScopeEnd, "}"));
                    scopeCount--;
                    if (scopeCount < 0) {
                        this->error = true;
                        this->errmsg = "Invalid `}` without corresponding `{`.";
                    }
                    break;
                case '\"': {
                    this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::String, slets[sletIndex++].c_str()));
                    cp = strchr(cp + 1, '\"') + 1;
                    break;
                }
                case 0x02: {
                    char num[32];
                    sprintf(num, "%d", (int)schrs[schrsIndex++]);
                    this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Numeric, num));
                    while (0x02 == *cp) { cp++; }
                    break;
                }
                default: {
                    ed = cp + 1;
                    while (*ed) {
                        if (' ' == *ed ||
                            '[' == *ed ||
                            ']' == *ed ||
                            '(' == *ed ||
                            ')' == *ed ||
                            '{' == *ed ||
                            '}' == *ed ||
                            '\"' == *ed ||
                            0x02 == *ed ||
                            '+' == *ed ||
                            '-' == *ed ||
                            '*' == *ed ||
                            '/' == *ed ||
                            '&' == *ed ||
                            '=' == *ed ||
                            '|' == *ed ||
                            '^' == *ed ||
                            '<' == *ed ||
                            '>' == *ed ||
                            '%' == *ed ||
                            ',' == *ed) {
                            break;
                        }
                        ed++;
                    }
                    if (*ed) {
                        char work[sizeof(formed)];
                        memset(work, 0, sizeof(work));
                        memcpy(work, cp, ed - cp);
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Other, work));
                        cp = ed;
                        while (' ' == *cp) {
                            cp++;
                        }
                    } else {
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Other, cp));
                        parseEnd = true;
                    }
                }
            }
        }
    }
    // 配列は1行内でクローズ必須
    if (arrayCount) {
        this->error = true;
        this->errmsg = "`[` is not closed with `]`.";
        arrayCount = 0;
    }
    // カッコは1行内でクローズ必須
    if (bracketCount) {
        this->error = true;
        this->errmsg = "`(` is not closed with `)`.";
        bracketCount = 0;
    }
}

LineData::LineData(LineData* line)
{
    this->error = false;
    this->errmsg = "";
    this->path = line->path;
    this->lineNumber = line->lineNumber;
    this->programCounter = line->programCounter;
    this->programCounterInit = line->programCounterInit;
    this->text = line->text;
    this->isAssignmnt = line->isAssignmnt;
    for (auto t : line->token) {
        this->token.push_back(std::make_pair(t.first, t.second));
    }
    for (auto t : line->machine) {
        this->machine.push_back(t);
    }
}

LineData::LineData()
{
    this->error = false;
    this->errmsg = "";
    this->path = "";
    this->lineNumber = 0;
    this->programCounter = 0;
    this->programCounterInit = false;
    this->isAssignmnt = false;
    this->text = "";
    this->token.clear();
    this->machine.clear();
}

void LineData::printDebug()
{
    for (auto token : this->token) {
        if (token.first == TokenType::Mnemonic) {
            if (this->programCounterInit) {
                printf(" $%04X <%s>", this->programCounter, token.second.c_str());
            } else {
                printf(" $\?\?\?\? <%s>", token.second.c_str());
            }
        } else if (token.first == TokenType::Mnemonic) {
            printf(" {%s}", token.second.c_str());
        } else if (token.first == TokenType::Operand) {
            printf(" <%s>", token.second.c_str());
        } else if (token.first == TokenType::AddressBegin) {
            printf(" _(");
        } else if (token.first == TokenType::AddressEnd) {
            printf(" )_");
        } else if (token.first == TokenType::SizeOf) {
            printf(" sizeof(%s)", token.second.c_str());
        } else if (token.first == TokenType::Numeric) {
            printf(" 0x%X", atoi(token.second.c_str()));
        } else if (token.first == TokenType::Label || token.first == TokenType::LabelAt) {
            if (this->programCounterInit) {
                printf(" $%04X %s:", this->programCounter, token.second.c_str());
            } else {
                printf(" $\?\?\?\? %s:", token.second.c_str());
            }
        } else if (token.first == TokenType::Binary) {
            printf(" Binary: \"%s\"", token.second.c_str());
        } else {
            printf(" `%s`", token.second.c_str());
        }
    }
    printf("\n");
}

std::vector<LineData*> readFile(const char* filePath)
{
    try {
        includeFiles.push_back(SHA1::from_file(filePath));
        int lineNumber = 0;
        std::ifstream ifs(filePath, std::ios::binary);
        ifs.seekg(0, std::ios::end);
        auto sz = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::vector<char> buf(sz);
        ifs.read(buf.data(), sz);
        auto str = std::string(buf.data(), sz);
        int first = 0;
        int last = str.find_first_of('\n');
        std::vector<LineData*> result;
        while (first < str.size()) {
            std::string subStr(str, first, last - first);
            auto line = new LineData(filePath, ++lineNumber, subStr);

            // #include は他のプリプロセッサよりも優先して先に展開
            if (2 <= line->token.size() && line->token[0].second == "#INCLUDE" && TokenType::String == line->token[1].first) {
                auto path = line->token[1].second.c_str();
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
                auto sha1 = SHA1::from_file(path);
                bool alreadyIncluded = false;
                for (auto i : includeFiles) {
                    if (i == sha1) {
                        alreadyIncluded = true;
                        break;
                    }
                }
                if (!alreadyIncluded) {
                    auto iLines = readFile(path);
                    if (iLines.empty()) {
                        line->error = true;
                        line->errmsg = "Cannot open \"";
                        line->errmsg += path;
                        line->errmsg += "\"";
                        result.push_back(line);
                    } else {
                        for (auto includeLine : iLines) {
                            result.push_back(includeLine);
                        }
                    }
                }
            } else {
                if (line->token.empty()) {
                    delete line;
                } else {
                    result.push_back(line);
                }
            }

            first = last + 1;
            last = str.find_first_of('\n', first);
            if (last == std::string::npos) {
                last = str.size();
            }
        }

        // スコープが閉じられているかチェック
        if (0 != scopeCount) {
            lastScopeBegin->error = true;
            lastScopeBegin->errmsg = "The scope `{` specified in this line is not closed with `}`.";
        }

        return result;
    } catch (...) {
        std::vector<LineData*> empty;
        return empty;
    }
}
