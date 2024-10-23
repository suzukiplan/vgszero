#include "common.h"

std::string evaluate_formulas(std::vector<std::pair<TokenType, std::string>>* token)
{
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
                for (auto itW = it + 1; itW->first != TokenType::BracketEnd; token->erase(itW)) {
                    work.push_back(std::make_pair(itW->first, itW->second));
                }
                evaluate_formulas(&work);
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

void evaluate_formulas(LineData* line)
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
    auto errmsg = evaluate_formulas(&line->token);
    if (!errmsg.empty()) {
        line->error = true;
        line->errmsg = errmsg;
    }
}

void evaluate_formulas_array(LineData* line)
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
                    auto error = evaluate_formulas(&elements);
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

void replace_assignment(LineData* line)
{
    std::vector<std::pair<TokenType, std::string>> left;
    std::vector<std::pair<TokenType, std::string>> right;
    bool isLeft = true;
    TokenType assignment = TokenType::None;
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::Equal ||
            it->first == TokenType::EqualAnd ||
            it->first == TokenType::EqualMinus ||
            it->first == TokenType::EqualOr ||
            it->first == TokenType::EqualPlus ||
            it->first == TokenType::EqualXor ||
            it->first == TokenType::EqualShiftLeft ||
            it->first == TokenType::EqualShiftRight) {
            if (assignment != TokenType::None) {
                line->error = true;
                line->errmsg = "Multiple assignments are described.";
                return;
            }
            assignment = it->first;
            isLeft = false;
            continue;
        }
        if (isLeft) {
            left.push_back(std::make_pair(it->first, it->second));
        } else {
            right.push_back(std::make_pair(it->first, it->second));
        }
    }
    if (assignment == TokenType::None) {
        return;
    }
    if (left.empty() || right.empty()) {
        line->error = true;
        line->errmsg = "Illegal assignment expression without left or right side.";
        return;
    }
    line->token.clear();
    std::pair<TokenType, std::string> mne;
    switch (assignment) {
        case TokenType::Equal:
            mne = std::make_pair(TokenType::Other, "LD");
            break;
        case TokenType::EqualAnd:
            mne = std::make_pair(TokenType::Other, "AND");
            break;
        case TokenType::EqualMinus:
            mne = std::make_pair(TokenType::Other, "SUB");
            break;
        case TokenType::EqualOr:
            mne = std::make_pair(TokenType::Other, "OR");
            break;
        case TokenType::EqualPlus:
            mne = std::make_pair(TokenType::Other, "ADD");
            break;
        case TokenType::EqualXor:
            mne = std::make_pair(TokenType::Other, "XOR");
            break;
        case TokenType::EqualShiftLeft:
            mne = std::make_pair(TokenType::Other, "SL");
            break;
        case TokenType::EqualShiftRight:
            mne = std::make_pair(TokenType::Other, "SR");
            break;
        default:
            puts("logic error");
            exit(-1);
    }
    line->token.insert(line->token.begin(), mne);
    line->token.insert(line->token.end(), left.begin(), left.end());
    line->token.insert(line->token.end(), std::make_pair<TokenType, std::string>(TokenType::Split, ","));
    line->token.insert(line->token.end(), right.begin(), right.end());
    line->isAssignmnt = true;
}

std::string oct2dec(const char* oct)
{
    int result = 0;
    int len = 0;
    while (*oct) {
        if (isdigit(*oct) && *oct < '8') {
            result <<= 3;
            result += (*oct) - '0';
            len++;
        } else {
            return "";
        }
        oct++;
    }
    return 0 < len ? std::to_string(result) : "";
}

std::string hex2dec(const char* hex)
{
    int result = 0;
    int len = 0;
    while (*hex) {
        if (isdigit(*hex)) {
            result <<= 4;
            result += (*hex) - '0';
            len++;
        } else if ('A' <= *hex && *hex <= 'F') {
            result <<= 4;
            result += (*hex) - 'A' + 10;
            len++;
        } else {
            return "";
        }
        hex++;
    }
    return 0 < len ? std::to_string(result) : "";
}

std::string bin2dec(const char* bin)
{
    int result = 0;
    int len = 0;
    while (*bin) {
        if ('0' == *bin || '1' == *bin) {
            result <<= 1;
            result += (*bin) - '0';
            len++;
        } else {
            return "";
        }
        bin++;
    }
    return 0 < len ? std::to_string(result) : "";
}

void parse_numeric(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first != TokenType::Other) {
            continue;
        }
        auto str = it->second.c_str();
        bool minus = false;
        if ('-' == *str) {
            minus = true;
            str++;
        }
        if ('$' == *str || 0 == strncmp("0X", str, 2)) {
            auto dec = hex2dec(str + ('$' == *str ? 1 : 2));
            if (!dec.empty()) {
                it->first = TokenType::Numeric;
                it->second = minus ? "-" + dec : dec;
            } else {
                line->error = true;
                line->errmsg = "Invalid hexadecimal number";
                return;
            }
        } else if ('%' == *str || 0 == strncmp("0B", str, 2)) {
            auto dec = bin2dec(str + ('%' == *str ? 1 : 2));
            if (!dec.empty()) {
                it->first = TokenType::Numeric;
                it->second = minus ? "-" + dec : dec;
            } else {
                line->error = true;
                line->errmsg = "Invalid binary number";
                return;
            }
        } else {
            bool isNumeric = true;
            while (*str) {
                if (!isdigit(*str)) {
                    isNumeric = false;
                    break;
                }
                str++;
            }
            if (isNumeric) {
                it->first = TokenType::Numeric;
            }
        }
    }
}

// , - n を , -n にする
void parse_numeric_minus(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::Split || it->first == TokenType::Mnemonic) {
            if (it + 1 != line->token.end() && it + 2 != line->token.end()) {
                if ((it + 1)->first == TokenType::Minus && (it + 2)->first == TokenType::Numeric) {
                    (it + 1)->first = TokenType::Delete;
                    (it + 2)->second = "-" + (it + 2)->second;
                    it += 2;
                }
            }
        }
    }
}

// , + n を , n にする
void parse_numeric_plus(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::Split || it->first == TokenType::Mnemonic) {
            if (it + 1 != line->token.end() && it + 2 != line->token.end()) {
                if ((it + 1)->first == TokenType::Plus && (it + 2)->first == TokenType::Numeric) {
                    (it + 1)->first = TokenType::Delete;
                    it += 2;
                }
            }
        }
    }
}

static bool isIncrementableRegister(LineData* line, std::vector<std::pair<TokenType, std::string>>::iterator it)
{
    if (it == line->token.end()) {
        return false;
    }
    if (it->first != TokenType::Operand) {
        return false;
    }
    switch (operandTable[it->second]) {
        case Operand::A: return true;
        case Operand::B: return true;
        case Operand::C: return true;
        case Operand::D: return true;
        case Operand::E: return true;
        case Operand::H: return true;
        case Operand::L: return true;
        case Operand::IXH: return true;
        case Operand::IXL: return true;
        case Operand::BC: return true;
        case Operand::DE: return true;
        case Operand::HL: return true;
        case Operand::IX: return true;
        case Operand::IY: return true;
    }
    return false;
}

void split_increment(std::vector<LineData*>* lines)
{
    for (auto it = lines->begin(); it != lines->end(); it++) {
        auto line = *it;
        auto prevToken = line->token.end();
        auto nextToken = line->token.end();
        for (auto token = line->token.begin(); token != line->token.end(); token++) {
            nextToken = token + 1;
            if (token->first == TokenType::Inc || token->first == TokenType::Dec) {
                auto inc = token->first == TokenType::Inc;
                auto pre = isIncrementableRegister(line, prevToken);
                auto post = isIncrementableRegister(line, nextToken);
                // 前後にレジスタが指定されてなければエラー
                if (!pre && !post) {
                    line->error = true;
                    line->errmsg = "The `++` or `--` can only be specified before or after the register.";
                    return;
                }
                // 前後両方がレジスタでもエラー
                if (pre && post) {
                    line->error = true;
                    line->errmsg = "Illegal `++` or `--` sequence.";
                    return;
                }
                // トークンを後で削除するようにマーク
                token->first = TokenType::Delete;
                // 命令行を作成
                auto newLine = new LineData(line);
                newLine->token.clear();
                newLine->token.push_back(std::make_pair(TokenType::Mnemonic, inc ? "INC" : "DEC"));
                newLine->token.push_back(std::make_pair(TokenType::Operand, pre ? prevToken->second : nextToken->second));
                // 命令行を挿入
                if (pre) {
                    lines->insert(it + 1, newLine);
                } else {
                    lines->insert(it, newLine);
                }
                // 最初から探索し直す
                it = lines->begin();
                break;
            }
            prevToken = token;
        }
    }
}
