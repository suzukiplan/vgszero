#include "common.h"
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
                case '+':
                    if ('+' == *(cp + 1)) {
                        cp += 2;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Inc, "++"));
                    } else {
                        cp++;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Plus, "+"));
                    }
                    break;
                case '-':
                    if ('-' == *(cp + 1)) {
                        cp += 2;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Dec, "--"));
                    } else {
                        cp++;
                        this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Minus, "-"));
                    }
                    break;
                case '/':
                    cp++;
                    this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Div, "/"));
                    break;
                case '*':
                    cp++;
                    this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::Mul, "*"));
                    break;
                case '&':
                    cp++;
                    this->token.push_back(std::make_pair<TokenType, std::string>(TokenType::And, "&"));
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
    for (auto t : line->token) {
        this->token.push_back(std::make_pair(t.first, t.second));
    }
    for (auto t : line->machine) {
        this->machine.push_back(t);
    }
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
