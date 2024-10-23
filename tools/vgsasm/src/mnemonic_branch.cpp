#include "common.h"
#include "mnemonic.h"

static uint8_t get_jump_condition(LineData* line, std::string cond)
{
    switch (operandTable[cond]) {
        case Operand::NZ: return 0xC2;
        case Operand::Z: return 0xCA;
        case Operand::NC: return 0xD2;
        case Operand::C: return 0xDA;
        case Operand::PO: return 0xE2;
        case Operand::PE: return 0xEA;
        case Operand::P: return 0xF2;
        case Operand::M: return 0xFA;
    }
    line->error = true;
    line->errmsg = "Invalid condition: " + cond;
    return 0x00;
}

static uint8_t get_relative_condition(LineData* line, std::string cond)
{
    switch (operandTable[cond]) {
        case Operand::NZ: return 0x20;
        case Operand::Z: return 0x28;
        case Operand::NC: return 0x30;
        case Operand::C: return 0x38;
    }
    line->error = true;
    line->errmsg = "Invalid condition: " + cond;
    return 0x00;
}

static uint8_t get_call_condition(LineData* line, std::string cond)
{
    switch (operandTable[cond]) {
        case Operand::NZ: return 0xC4;
        case Operand::Z: return 0xCC;
        case Operand::NC: return 0xD4;
        case Operand::C: return 0xDC;
        case Operand::PO: return 0xE4;
        case Operand::PE: return 0xEC;
        case Operand::P: return 0xF4;
        case Operand::M: return 0xFC;
    }
    line->error = true;
    line->errmsg = "Invalid condition: " + cond;
    return 0x00;
}

void mnemonic_JP(LineData* line)
{
    if (mnemonic_format_test(line, 2, TokenType::Numeric)) {
        auto addr = atoi(line->token[1].second.c_str());
        if (mnemonic_range(line, addr, 0x0000, 0xFFFF)) {
            line->machine.push_back(0xC3);
            line->machine.push_back(addr & 0xFF);
            line->machine.push_back((addr & 0xFF00) >> 8);
            return;
        }
    } else if (mnemonic_format_test(line, 2, TokenType::LabelJump)) {
        line->machine.push_back(0xC3);
        tempAddrs.push_back(new TempAddr(line, line->token[1].second, line->machine.size(), false));
        line->machine.push_back(0x00);
        line->machine.push_back(0x00);
        return;
    } else if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Numeric) &&
               operand_is_condition(line->token[1].second)) {
        uint8_t code = get_jump_condition(line, line->token[1].second);
        auto addr = atoi(line->token[3].second.c_str());
        if (code && mnemonic_range(line, addr, 0x0000, 0xFFFF)) {
            line->machine.push_back(code);
            line->machine.push_back(addr & 0xFF);
            line->machine.push_back((addr & 0xFF00) >> 8);
            return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::LabelJump) &&
               operand_is_condition(line->token[1].second)) {
        uint8_t code = get_jump_condition(line, line->token[1].second);
        if (code) {
            line->machine.push_back(code);
            tempAddrs.push_back(new TempAddr(line, line->token[3].second, line->machine.size(), false));
            line->machine.push_back(0x00);
            line->machine.push_back(0x00);
            return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd) ||
               mnemonic_format_test(line, 2, TokenType::Operand)) {
        switch (operandTable[line->token[line->token.size() == 4 ? 2 : 1].second]) {
            case Operand::BC:
                ML_PUSH_BC;
                ML_RET;
                return;
            case Operand::DE:
                ML_PUSH_DE;
                ML_RET;
                return;
            case Operand::HL:
                line->machine.push_back(0xE9);
                return;
            case Operand::IX:
                line->machine.push_back(0xDD);
                line->machine.push_back(0xE9);
                return;
            case Operand::IY:
                line->machine.push_back(0xFD);
                line->machine.push_back(0xE9);
                return;
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal JP instruction.";
    }
}

void mnemonic_JR(LineData* line)
{
    if (mnemonic_format_test(line, 2, TokenType::Numeric)) {
        auto e = atoi(line->token[1].second.c_str());
        if (mnemonic_range(line, e, -128, 127)) {
            line->machine.push_back(0x18);
            line->machine.push_back(e);
            return;
        }
    } else if (mnemonic_format_test(line, 2, TokenType::LabelJump)) {
        line->machine.push_back(0x18);
        tempAddrs.push_back(new TempAddr(line, line->token[1].second, line->machine.size(), true));
        line->machine.push_back(0x00);
        return;
    } else if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Numeric) &&
               operand_is_condition(line->token[1].second)) {
        uint8_t code = get_relative_condition(line, line->token[1].second);
        auto e = atoi(line->token[3].second.c_str());
        if (code && mnemonic_range(line, e, -128, 127)) {
            line->machine.push_back(code);
            line->machine.push_back(e);
            return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::LabelJump) &&
               operand_is_condition(line->token[1].second)) {
        uint8_t code = get_relative_condition(line, line->token[1].second);
        if (code) {
            line->machine.push_back(code);
            tempAddrs.push_back(new TempAddr(line, line->token[3].second, line->machine.size(), true));
            line->machine.push_back(0x00);
            return;
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal JR instruction.";
    }
}

void mnemonic_DJNZ(LineData* line)
{
    if (mnemonic_format_test(line, 2, TokenType::Numeric)) {
        auto e = atoi(line->token[1].second.c_str());
        if (mnemonic_range(line, e, -128, 127)) {
            line->machine.push_back(0x10);
            line->machine.push_back(e);
            return;
        }
    } else if (mnemonic_format_test(line, 2, TokenType::LabelJump)) {
        line->machine.push_back(0x10);
        tempAddrs.push_back(new TempAddr(line, line->token[1].second, line->machine.size(), true));
        line->machine.push_back(0x00);
        return;
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal DJNZ instruction.";
    }
}

void mnemonic_CALL(LineData* line)
{
    if (mnemonic_format_test(line, 2, TokenType::Numeric)) {
        auto addr = atoi(line->token[1].second.c_str());
        if (mnemonic_range(line, addr, 0x0000, 0xFFFF)) {
            line->machine.push_back(0xCD);
            line->machine.push_back(addr & 0xFF);
            line->machine.push_back((addr & 0xFF00) >> 8);
            return;
        }
    } else if (mnemonic_format_test(line, 2, TokenType::LabelJump)) {
        line->machine.push_back(0xCD);
        tempAddrs.push_back(new TempAddr(line, line->token[1].second, line->machine.size(), false));
        line->machine.push_back(0x00);
        line->machine.push_back(0x00);
        return;
    } else if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Numeric)) {
        auto code = get_call_condition(line, line->token[1].second);
        auto addr = atoi(line->token[3].second.c_str());
        if (code && mnemonic_range(line, addr, 0x0000, 0xFFFF)) {
            line->machine.push_back(code);
            line->machine.push_back(addr & 0xFF);
            line->machine.push_back((addr & 0xFF00) >> 8);
            return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::LabelJump)) {
        auto code = get_call_condition(line, line->token[1].second);
        if (code) {
            line->machine.push_back(code);
            tempAddrs.push_back(new TempAddr(line, line->token[3].second, line->machine.size(), false));
            line->machine.push_back(0x00);
            line->machine.push_back(0x00);
            return;
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal CALL instruction.";
    }
}

void mnemonic_RET(LineData* line)
{
    if (line->token.size() == 1) {
        ML_RET;
        return;
    } else if (mnemonic_format_test(line, 2, TokenType::Operand)) {
        auto code = get_call_condition(line, line->token[1].second);
        if (code) {
            line->machine.push_back(code & 0b11111011);
            return;
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal RET instruction.";
    }
}

void mnemonic_RST(LineData* line)
{
    if (mnemonic_format_test(line, 2, TokenType::Numeric)) {
        auto n = atoi(line->token[1].second.c_str());
        switch (n) {
            case 0: line->machine.push_back(0xC7); return;
            case 1: line->machine.push_back(0xCF); return;
            case 2: line->machine.push_back(0xD7); return;
            case 3: line->machine.push_back(0xDF); return;
            case 4: line->machine.push_back(0xE7); return;
            case 5: line->machine.push_back(0xEF); return;
            case 6: line->machine.push_back(0xF7); return;
            case 7: line->machine.push_back(0xFF); return;
            case 0x08: line->machine.push_back(0xCF); return;
            case 0x10: line->machine.push_back(0xD7); return;
            case 0x18: line->machine.push_back(0xDF); return;
            case 0x20: line->machine.push_back(0xE7); return;
            case 0x28: line->machine.push_back(0xEF); return;
            case 0x30: line->machine.push_back(0xF7); return;
            case 0x38: line->machine.push_back(0xFF); return;
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal RST instruction.";
    }
}
