#include "common.h"

std::map<std::string, Operand> operandTable = {
    {"A", Operand::A},
    {"B", Operand::B},
    {"C", Operand::C},
    {"D", Operand::D},
    {"E", Operand::E},
    {"F", Operand::F},
    {"H", Operand::H},
    {"L", Operand::L},
    {"IXH", Operand::IXH},
    {"IXL", Operand::IXL},
    {"IYH", Operand::IYH},
    {"IYL", Operand::IYL},
    {"AF", Operand::AF},
    {"AF'", Operand::AF_dash},
    {"BC", Operand::BC},
    {"DE", Operand::DE},
    {"HL", Operand::HL},
    {"IX", Operand::IX},
    {"IY", Operand::IY},
    {"SP", Operand::SP},
    {"NZ", Operand::NZ},
    {"Z", Operand::Z},
    {"NC", Operand::NC},
    {"PO", Operand::PO},
    {"PE", Operand::PE},
    {"P", Operand::P},
    {"M", Operand::M},
};

static bool parseOperandSkipScope = false;

bool operand_is_condition(Operand op)
{
    switch (op) {
        case Operand::NZ: return true;
        case Operand::Z: return true;
        case Operand::NC: return true;
        case Operand::C: return true;
        case Operand::PO: return true;
        case Operand::PE: return true;
        case Operand::P: return true;
        case Operand::M: return true;
    }
    return false;
}

bool operand_is_condition(std::string str)
{
    auto op = operandTable.find(str);
    if (op != operandTable.end()) {
        return operand_is_condition(op->second);
    }
    return false;
}

void parse_operand(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        // スコープ内では解析をスキップ
        if (parseOperandSkipScope) {
            if (it->first == TokenType::ScopeEnd) {
                parseOperandSkipScope = false;
            }
            continue;
        }

        // スコープ開始を検出したらスキップ
        if (it->first == TokenType::ScopeBegin) {
            parseOperandSkipScope = true;
            continue;
        }

        // Other ならチェック
        if (it->first == TokenType::Other) {
            auto op = operandTable.find(it->second);
            if (op != operandTable.end()) {
                it->first = TokenType::Operand;
            }
        }
    }
}
