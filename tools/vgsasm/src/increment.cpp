#include "common.h"

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
