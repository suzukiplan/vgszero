#include "common.h"
#include "mnemonic.h"

void mnemonic_DB(LineData* line)
{
    if (line->token.size() < 2) {
        return;
    }
    for (auto it = line->token.begin() + 1; it != line->token.end(); it++) {
        switch (it->first) {
            case TokenType::Numeric: {
                auto n = atoi(it->second.c_str());
                if (n < -128 || 255 < n) {
                    line->error = true;
                    line->errmsg = "An invalid range of number specified: " + it->second;
                    return;
                }
                line->machine.push_back(n & 0xFF);
                break;
            }
            case TokenType::String: {
                for (auto str = it->second.c_str(); *str; str++) {
                    line->machine.push_back(*str);
                }
                break;
            }
            default:
                line->error = true;
                line->errmsg = "Unexpected symbols detected in DB: " + it->second;
                return;
        }
        if (it + 1 != line->token.end()) {
            it++;
            if (it->first != TokenType::Split) {
                line->error = true;
                line->errmsg = "Symbols are not specified as comma-separated: " + it->second;
                return;
            }
            if (it + 1 == line->token.end()) {
                line->error = true;
                line->errmsg = "No symbol is specified after the comma.";
                return;
            }
        }
    }
}

void mnemonic_DW(LineData* line)
{
    if (line->token.size() < 2) {
        return;
    }
    for (auto it = line->token.begin() + 1; it != line->token.end(); it++) {
        switch (it->first) {
            case TokenType::Numeric: {
                auto n = atoi(it->second.c_str());
                if (n < -32768 || 65535 < n) {
                    line->error = true;
                    line->errmsg = "An invalid range of number specified: " + it->second;
                    return;
                }
                line->machine.push_back(n & 0x00FF);
                line->machine.push_back((n & 0xFF00) >> 8);
                break;
            }
            case TokenType::String: {
                for (auto str = it->second.c_str(); *str; str++) {
                    line->machine.push_back(*str);
                    line->machine.push_back(0x00);
                }
                break;
            }
            case TokenType::LabelJump: {
                tempAddrs.push_back(new TempAddr(line, it->second, line->machine.size(), false));
                line->machine.push_back(0x00);
                line->machine.push_back(0x00);
                break;
            }
            default:
                line->error = true;
                line->errmsg = "Unexpected symbols detected in DB: " + it->second;
                return;
        }
        if (it + 1 != line->token.end()) {
            it++;
            if (it->first != TokenType::Split) {
                line->error = true;
                line->errmsg = "Symbols are not specified as comma-separated: " + it->second;
                return;
            }
            if (it + 1 == line->token.end()) {
                line->error = true;
                line->errmsg = "No symbol is specified after the comma.";
                return;
            }
        }
    }
}

void mnemonic_EX(LineData* line)
{
    if (line->token.size() < 4) {
        line->error = true;
        line->errmsg = "Excessive or insufficient number of operands.";
    }
    if (line->token[1].first == TokenType::Operand) {
        if (mnemonic_format_check(line, 4, TokenType::Operand, TokenType::Split, TokenType::Operand)) {
            auto left = operandTable[line->token[1].second];
            auto right = operandTable[line->token[3].second];
            if (left == Operand::DE && right == Operand::HL) {
                line->machine.push_back(0xEB);
            } else if (left == Operand::AF && right == Operand::AF_dash) {
                line->machine.push_back(0x08);
            } else {
                line->error = true;
                line->errmsg = "Incorrect EX syntax.";
            }
        }
    } else if (line->token[1].first == TokenType::AddressBegin) {
        if (mnemonic_format_check(line, 6, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd, TokenType::Split, TokenType::Operand)) {
            auto left = operandTable[line->token[2].second];
            auto right = operandTable[line->token[5].second];
            if (left == Operand::SP && right == Operand::HL) {
                line->machine.push_back(0xE3);
            } else if (left == Operand::SP && right == Operand::IX) {
                line->machine.push_back(0xDD);
                line->machine.push_back(0xE3);
            } else if (left == Operand::SP && right == Operand::IY) {
                line->machine.push_back(0xFD);
                line->machine.push_back(0xE3);
            } else {
                line->error = true;
                line->errmsg = "Incorrect EX syntax.";
            }
        }
    } else {
        line->error = true;
        line->errmsg = "Incorrect EX syntax.";
    }
}

void mnemonic_PUSH(LineData* line)
{
    if (mnemonic_format_check(line, 2, TokenType::Operand)) {
        switch (operandTable[line->token[1].second]) {
            case Operand::BC: ML_PUSH_BC; break;
            case Operand::DE: ML_PUSH_DE; break;
            case Operand::HL: ML_PUSH_HL; break;
            case Operand::AF: ML_PUSH_AF; break;
            case Operand::IX: ML_PUSH_IX; break;
            case Operand::IY: ML_PUSH_IY; break;
            default:
                line->error = true;
                line->errmsg = "Unsupported PUSH operand: " + line->token[1].second;
        }
    }
}

void mnemonic_POP(LineData* line)
{
    if (mnemonic_format_check(line, 2, TokenType::Operand)) {
        switch (operandTable[line->token[1].second]) {
            case Operand::BC: ML_POP_BC; break;
            case Operand::DE: ML_POP_DE; break;
            case Operand::HL: ML_POP_HL; break;
            case Operand::AF: ML_POP_AF; break;
            case Operand::IX: ML_POP_IX; break;
            case Operand::IY: ML_POP_IY; break;
            default:
                line->error = true;
                line->errmsg = "Unsupported POP operand: " + line->token[1].second;
        }
    }
}

void mnemonic_IN(LineData* line)
{
    if (mnemonic_format_test(line, 6, TokenType::Operand, TokenType::Split, TokenType::AddressBegin, TokenType::Numeric, TokenType::AddressEnd) &&
        operandTable[line->token[1].second] == Operand::A) {
        int n = atoi(line->token[4].second.c_str());
        if (mnemonic_range(line, n, 0, 255)) {
            ML_IN_A(n);
            return;
        }
    } else if (mnemonic_format_test(line, 6, TokenType::Operand, TokenType::Split, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd) &&
               operandTable[line->token[4].second] == Operand::C) {
        line->machine.push_back(0xED);
        switch (operandTable[line->token[1].second]) {
            case Operand::B: line->machine.push_back(0x40); return;
            case Operand::C: line->machine.push_back(0x48); return;
            case Operand::D: line->machine.push_back(0x50); return;
            case Operand::E: line->machine.push_back(0x58); return;
            case Operand::H: line->machine.push_back(0x60); return;
            case Operand::L: line->machine.push_back(0x68); return;
            case Operand::F: line->machine.push_back(0x70); return; // undocumented
            case Operand::A: line->machine.push_back(0x78); return;
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal IN instruction.";
    }
}

void mnemonic_OUT(LineData* line)
{
    if (mnemonic_format_test(line, 6, TokenType::AddressBegin, TokenType::Numeric, TokenType::AddressEnd, TokenType::Split, TokenType::Operand) &&
        operandTable[line->token[5].second] == Operand::A) {
        int n = atoi(line->token[2].second.c_str());
        if (mnemonic_range(line, n, 0, 255)) {
            ML_OUT_A(n);
            return;
        }
    } else if (mnemonic_format_test(line, 6, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd, TokenType::Split, TokenType::Operand) &&
               operandTable[line->token[2].second] == Operand::C) {
        line->machine.push_back(0xED);
        switch (operandTable[line->token[5].second]) {
            case Operand::B: line->machine.push_back(0x41); return;
            case Operand::C: line->machine.push_back(0x49); return;
            case Operand::D: line->machine.push_back(0x51); return;
            case Operand::E: line->machine.push_back(0x59); return;
            case Operand::H: line->machine.push_back(0x61); return;
            case Operand::L: line->machine.push_back(0x69); return;
            case Operand::A: line->machine.push_back(0x79); return;
        }
    } else if (mnemonic_format_test(line, 6, TokenType::Operand, TokenType::Split, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd) &&
               operandTable[line->token[1].second] == Operand::F &&
               operandTable[line->token[4].second] == Operand::C) {
        line->machine.push_back(0xED);
        line->machine.push_back(0x71);
        return;
    } else if (mnemonic_format_test(line, 4, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd) &&
               operandTable[line->token[2].second] == Operand::C) {
        line->machine.push_back(0xED);
        line->machine.push_back(0x71);
        return;
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal OUT instruction.";
    }
}
