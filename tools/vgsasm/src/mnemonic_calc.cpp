#include "common.h"
#include "mnemonic.h"

void mnemonic_calc8(LineData* line, uint8_t code)
{
    if (mnemonic_format_test(line, 2, TokenType::Operand)) {
        auto op = operandTable[line->token[1].second];
        if (op == Operand::IXH || op == Operand::IXL) {
            line->machine.push_back(0xDD);
        } else if (op == Operand::IYH || op == Operand::IYL) {
            line->machine.push_back(0xFD);
        }
        switch (op) {
            case Operand::A: code |= 0x07; break;
            case Operand::B: code |= 0x00; break;
            case Operand::C: code |= 0x01; break;
            case Operand::D: code |= 0x02; break;
            case Operand::E: code |= 0x03; break;
            case Operand::H: code |= 0x04; break;
            case Operand::L: code |= 0x05; break;
            case Operand::IXH: code |= 0x04; break;
            case Operand::IXL: code |= 0x05; break;
            case Operand::IYH: code |= 0x04; break;
            case Operand::IYL: code |= 0x05; break;
            default:
                line->error = true;
                line->errmsg = "Illegal 8-bit arithmetic instruction.";
                return;
        }
        line->machine.push_back(code);
    } else if (mnemonic_format_test(line, 2, TokenType::Numeric)) {
        auto n = atoi(line->token[1].second.c_str());
        if (mnemonic_range(line, n, -128, 255)) {
            line->machine.push_back(code | 0x46);
            line->machine.push_back(n & 0xFF);
        }
    } else if (4 <= line->token.size() &&
               line->token[1].first == TokenType::AddressBegin &&
               line->token[2].first == TokenType::Operand) {
        auto op = operandTable[line->token[2].second];
        if (op == Operand::HL) {
            if (4 != line->token.size() && line->token[3].first == TokenType::AddressEnd) {
                line->error = true;
                line->errmsg = "Illegal 8-bit arithmetic instruction.";
            } else {
                line->machine.push_back(code | 0x06);
            }
        } else if (op == Operand::IX || op == Operand::IY) {
            line->machine.push_back(op == Operand::IX ? 0xDD : 0xFD);
            line->machine.push_back(code | 0x06);
            if (4 == line->token.size() && line->token[3].first == TokenType::AddressEnd) {
                line->machine.push_back(0x00);
            } else if (6 == line->token.size() &&
                       line->token[3].first == TokenType::Plus &&
                       line->token[4].first == TokenType::Numeric &&
                       line->token[5].first == TokenType::AddressEnd) {
                auto n = atoi(line->token[4].second.c_str());
                if (mnemonic_range(line, n, 0, 127)) {
                    line->machine.push_back(n & 0xFF);
                }
            } else if (6 == line->token.size() &&
                       line->token[3].first == TokenType::Minus &&
                       line->token[4].first == TokenType::Numeric &&
                       line->token[5].first == TokenType::AddressEnd) {
                auto n = atoi(line->token[4].second.c_str());
                if (mnemonic_range(line, n, 0, 128)) {
                    line->machine.push_back((0 - n) & 0xFF);
                }
            } else {
                line->error = true;
                line->errmsg = "Illegal 8-bit arithmetic instruction.";
            }
        } else {
            line->error = true;
            line->errmsg = "Illegal 8-bit arithmetic instruction.";
        }
    } else if (3 < line->token.size() && line->token[1].first == TokenType::Operand && line->token[2].first == TokenType::Split) {
        auto op = operandTable[line->token[1].second];
        if (op == Operand::A) {
            auto it = line->token.begin() + 1;
            line->token.erase(it);
            line->token.erase(it);
            mnemonic_calc8(line, code);
        } else {
            line->error = true;
            line->errmsg = "Illegal arithmetic instruction.";
        }
    } else {
        line->error = true;
        line->errmsg = "Illegal 8-bit arithmetic instruction.";
    }
}

void mnemonic_calc16(LineData* line, uint8_t code)
{
    bool supportImmediate = true;
    bool supportIXY = true;
    switch (mnemonicTable[line->token[0].second]) {
        case Mnemonic::ADC:
        case Mnemonic::SBC:
            line->machine.push_back(0xED);
            supportImmediate = false;
            supportIXY = false;
            break;
    }
    if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Operand)) {
        auto op1 = operandTable[line->token[1].second];
        auto op2 = operandTable[line->token[3].second];
        if (op1 == Operand::BC && op2 == Operand::A) {
            ML_ADD_A_C;
            ML_LD_C_A;
            ML_JR_NC(1);
            ML_INC_B;
            return;
        } else if (op1 == Operand::DE && op2 == Operand::A) {
            ML_ADD_A_E;
            ML_LD_E_A;
            ML_JR_NC(1);
            ML_INC_D;
            return;
        } else if (op1 == Operand::HL && op2 == Operand::A) {
            ML_ADD_A_L;
            ML_LD_L_A;
            ML_JR_NC(1);
            ML_INC_H;
            return;
        } else if (op1 == Operand::HL) {
            if (op2 != Operand::BC && op2 != Operand::DE && op2 != Operand::HL && op2 != Operand::SP) {
                line->error = true;
                line->errmsg = "Illegal 16-bit arithmetic instruction.";
                return;
            }
        } else if (supportIXY && op1 == Operand::IX) {
            line->machine.push_back(0xDD);
            if (op2 != Operand::BC && op2 != Operand::DE && op2 != Operand::IX && op2 != Operand::SP) {
                line->error = true;
                line->errmsg = "Illegal 16-bit arithmetic instruction.";
                return;
            }
        } else if (supportIXY && op1 == Operand::IY) {
            line->machine.push_back(0xFD);
            if (op2 != Operand::BC && op2 != Operand::DE && op2 != Operand::IY && op2 != Operand::SP) {
                line->error = true;
                line->errmsg = "Illegal 16-bit arithmetic instruction.";
                return;
            }
        } else {
            line->error = true;
            line->errmsg = "Illegal 16-bit arithmetic instruction.";
            return;
        }
        switch (op2) {
            case Operand::BC: line->machine.push_back(code); break;
            case Operand::DE: line->machine.push_back(code | 0x10); break;
            case Operand::HL: line->machine.push_back(code | 0x20); break;
            case Operand::IX: line->machine.push_back(code | 0x20); break;
            case Operand::IY: line->machine.push_back(code | 0x20); break;
            case Operand::SP: line->machine.push_back(code | 0x30); break;
        }
    } else if (supportImmediate &&
               mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Numeric)) {
        auto op = operandTable[line->token[1].second];
        auto nn = atoi(line->token[3].second.c_str());
        if (op != Operand::HL && op != Operand::IX && op != Operand::IY) {
            line->error = true;
            line->errmsg = "Illegal 16-bit arithmetic instruction.";
            return;
        }
        if (mnemonic_range(line, nn, -32768, 65535)) {
            uint8_t nl = nn & 0xFF;
            uint8_t nh = (nn & 0xFF00) >> 8;
            ML_PUSH_DE;
            ML_LD_D_n(nh);
            ML_LD_E_n(nl);
            switch (op) {
                case Operand::IX: line->machine.push_back(0xDD); break;
                case Operand::IY: line->machine.push_back(0xFD); break;
            }
            line->machine.push_back(code | 0x10);
            ML_POP_DE;
        }
    } else {
        line->error = true;
        line->errmsg = "Illegal 16-bit arithmetic instruction.";
    }
}

void mnemonic_calcOH(LineData* line, uint8_t code8, uint8_t code16)
{
    if (mnemonic_format_test(line, 4, TokenType::AddressBegin, TokenType::Numeric, TokenType::AddressEnd)) {
        // 即値アドレス演算（auto-expand）
        auto addr = atoi(line->token[2].second.c_str());
        if (mnemonic_range(line, addr, 0, 0xFFFF)) {
            switch (mnemonicTable[line->token[0].second]) {
                case Mnemonic::ADD:
                    ML_PUSH_HL;
                    ML_LD_L_n(addr & 0x00FF);
                    ML_LD_H_n((addr & 0xFF00) >> 8);
                    ML_ADD_HL;
                    ML_POP_HL;
                    break;
                case Mnemonic::ADC:
                    ML_PUSH_HL;
                    ML_LD_L_n(addr & 0x00FF);
                    ML_LD_H_n((addr & 0xFF00) >> 8);
                    ML_ADC_HL;
                    ML_POP_HL;
                    break;
                case Mnemonic::SUB:
                    ML_PUSH_HL;
                    ML_LD_L_n(addr & 0x00FF);
                    ML_LD_H_n((addr & 0xFF00) >> 8);
                    ML_SUB_HL;
                    ML_POP_HL;
                    break;
                case Mnemonic::SBC:
                    ML_PUSH_HL;
                    ML_LD_L_n(addr & 0x00FF);
                    ML_LD_H_n((addr & 0xFF00) >> 8);
                    ML_SBC_HL;
                    ML_POP_HL;
                    break;
                default:
                    line->error = true;
                    line->errmsg = "Unsupported immediate address 8bit arithmetic instruction: " + line->token[0].second;
            }
        }
    } else if (0 == code16) {
        mnemonic_calc8(line, code8);
    } else if (mnemonic_format_test(line, 2, TokenType::Operand)) {
        mnemonic_calc8(line, code8);
    } else if (mnemonic_format_test(line, 2, TokenType::Numeric)) {
        mnemonic_calc8(line, code8);
    } else if (3 <= line->token.size() && line->token[1].first == TokenType::AddressBegin) {
        mnemonic_calc8(line, code8);
    } else if (3 < line->token.size() && line->token[1].first == TokenType::Operand && line->token[2].first == TokenType::Split) {
        auto op = operandTable[line->token[1].second];
        if (op == Operand::A) {
            auto it = line->token.begin() + 1;
            line->token.erase(it);
            line->token.erase(it);
            mnemonic_calc8(line, code8);
        } else if (mnemonic_is_reg16(op)) {
            mnemonic_calc16(line, code16);
        } else {
            line->error = true;
            line->errmsg = "Illegal arithmetic instruction.";
        }
    } else {
        line->error = true;
        line->errmsg = "Illegal arithmetic instruction.";
    }
}

void mnemonic_shift(LineData* line, uint8_t code)
{
    if (mnemonic_format_test(line, 2, TokenType::Operand)) {
        line->machine.push_back(0xCB);
        switch (operandTable[line->token[1].second]) {
            case Operand::A: line->machine.push_back(code | 0x07); return;
            case Operand::B: line->machine.push_back(code | 0x00); return;
            case Operand::C: line->machine.push_back(code | 0x01); return;
            case Operand::D: line->machine.push_back(code | 0x02); return;
            case Operand::E: line->machine.push_back(code | 0x03); return;
            case Operand::H: line->machine.push_back(code | 0x04); return;
            case Operand::L: line->machine.push_back(code | 0x05); return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd)) {
        switch (operandTable[line->token[2].second]) {
            case Operand::HL:;
                line->machine.push_back(0xCB);
                line->machine.push_back(code | 0x06);
                return;
            case Operand::IX:;
                line->machine.push_back(0xDD);
                line->machine.push_back(0xCB);
                line->machine.push_back(0x00);
                line->machine.push_back(code | 0x06);
                return;
            case Operand::IY:;
                line->machine.push_back(0xFD);
                line->machine.push_back(0xCB);
                line->machine.push_back(0x00);
                line->machine.push_back(code | 0x06);
                return;
        }
    } else if (mnemonic_format_test(line, 6, TokenType::AddressBegin, TokenType::Operand, TokenType::PlusOrMinus, TokenType::Numeric, TokenType::AddressEnd)) {
        int n = atoi(line->token[4].second.c_str());
        if (line->token[3].first == TokenType::Plus) {
            if (!mnemonic_range(line, n, 0, 127)) {
                return;
            }
        } else {
            if (!mnemonic_range(line, n, 0, 128)) {
                return;
            }
            n = 0 - n;
        }
        switch (operandTable[line->token[2].second]) {
            case Operand::IX:
                line->machine.push_back(0xDD);
                line->machine.push_back(0xCB);
                line->machine.push_back(n);
                line->machine.push_back(code | 0x06);
                return;
            case Operand::IY:
                line->machine.push_back(0xFD);
                line->machine.push_back(0xCB);
                line->machine.push_back(n);
                line->machine.push_back(code | 0x06);
                return;
        }
    } else if (mnemonic_format_test(line, 8, TokenType::AddressBegin, TokenType::Operand, TokenType::PlusOrMinus, TokenType::Numeric, TokenType::AddressEnd, TokenType::And, TokenType::Operand)) {
        int n = atoi(line->token[4].second.c_str());
        if (line->token[3].first == TokenType::Plus) {
            if (!mnemonic_range(line, n, 0, 127)) {
                return;
            }
        } else {
            if (!mnemonic_range(line, n, 0, 128)) {
                return;
            }
            n = 0 - n;
        }
        auto op1 = operandTable[line->token[2].second];
        auto op2 = operandTable[line->token[7].second];
        if (code == 0x00 && op1 == Operand::IX) {
            line->machine.push_back(0xDD);
            line->machine.push_back(0xCB);
            line->machine.push_back(n);
            switch (op2) {
                case Operand::B: line->machine.push_back(0x00); return;
                case Operand::C: line->machine.push_back(0x01); return;
                case Operand::D: line->machine.push_back(0x02); return;
                case Operand::E: line->machine.push_back(0x03); return;
                case Operand::H: line->machine.push_back(0x04); return;
                case Operand::L: line->machine.push_back(0x05); return;
                case Operand::F: line->machine.push_back(0x06); return;
                case Operand::A: line->machine.push_back(0x07); return;
            }
        } else if (op1 == Operand::IX && op2 == Operand::B) {
            line->machine.push_back(0xDD);
            line->machine.push_back(0xCB);
            line->machine.push_back(n);
            switch (mnemonicTable[line->token[0].second]) {
                case Mnemonic::RRC: line->machine.push_back(0x08); return;
                case Mnemonic::RL: line->machine.push_back(0x10); return;
                case Mnemonic::RR: line->machine.push_back(0x18); return;
                case Mnemonic::SLA: line->machine.push_back(0x20); return;
                case Mnemonic::SRA: line->machine.push_back(0x28); return;
                case Mnemonic::SLL: line->machine.push_back(0x30); return;
                case Mnemonic::SRL: line->machine.push_back(0x38); return;
            }
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal shift/rotate instruction.";
    }
}

void mnemonic_INC(LineData* line)
{
    if (mnemonic_format_test(line, 2, TokenType::Operand)) {
        switch (operandTable[line->token[1].second]) {
            case Operand::A: ML_INC_A; return;
            case Operand::B: ML_INC_B; return;
            case Operand::C: ML_INC_C; return;
            case Operand::D: ML_INC_D; return;
            case Operand::E: ML_INC_E; return;
            case Operand::H: ML_INC_H; return;
            case Operand::L: ML_INC_L; return;
            case Operand::IXH: ML_INC_IXH; return;
            case Operand::IXL: ML_INC_IXL; return;
            case Operand::IYH: ML_INC_IYH; return;
            case Operand::IYL: ML_INC_IYL; return;
            case Operand::BC: ML_INC_BC; return;
            case Operand::DE: ML_INC_DE; return;
            case Operand::HL: ML_INC_HL; return;
            case Operand::SP: ML_INC_SP; return;
            case Operand::IX: ML_INC_IX; return;
            case Operand::IY: ML_INC_IY; return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd)) {
        switch (operandTable[line->token[2].second]) {
            case Operand::HL: ML_INC_ADDR_HL; return;
            case Operand::IX: ML_INC_ADDR_IX(0); return;
            case Operand::IY: ML_INC_ADDR_IY(0); return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::AddressBegin, TokenType::Numeric, TokenType::AddressEnd)) {
        int addr = atoi(line->token[2].second.c_str());
        if (mnemonic_range(line, addr, 0x0000, 0xFFFF)) {
            ML_PUSH_HL;
            ML_LD_L_n(addr & 0x00FF);
            ML_LD_H_n((addr & 0xFF00) >> 8);
            ML_INC_ADDR_HL;
            ML_POP_HL;
            return;
        }
    } else if (mnemonic_format_test(line, 6, TokenType::AddressBegin, TokenType::Operand, TokenType::PlusOrMinus, TokenType::Numeric, TokenType::AddressEnd)) {
        int n = atoi(line->token[4].second.c_str());
        if (line->token[3].first == TokenType::Plus) {
            if (!mnemonic_range(line, n, 0, 127)) {
                return;
            }
        } else {
            if (!mnemonic_range(line, n, 0, 128)) {
                return;
            }
            n = 0 - n;
        }
        switch (operandTable[line->token[2].second]) {
            case Operand::IX: ML_INC_ADDR_IX(n); return;
            case Operand::IY: ML_INC_ADDR_IY(n); return;
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal INC instruction.";
    }
}

void mnemonic_DEC(LineData* line)
{
    if (mnemonic_format_test(line, 2, TokenType::Operand)) {
        switch (operandTable[line->token[1].second]) {
            case Operand::A: ML_DEC_A; return;
            case Operand::B: ML_DEC_B; return;
            case Operand::C: ML_DEC_C; return;
            case Operand::D: ML_DEC_D; return;
            case Operand::E: ML_DEC_E; return;
            case Operand::H: ML_DEC_H; return;
            case Operand::L: ML_DEC_L; return;
            case Operand::IXH: ML_DEC_IXH; return;
            case Operand::IXL: ML_DEC_IXL; return;
            case Operand::IYH: ML_DEC_IYH; return;
            case Operand::IYL: ML_DEC_IYL; return;
            case Operand::BC: ML_DEC_BC; return;
            case Operand::DE: ML_DEC_DE; return;
            case Operand::HL: ML_DEC_HL; return;
            case Operand::SP: ML_DEC_SP; return;
            case Operand::IX: ML_DEC_IX; return;
            case Operand::IY: ML_DEC_IY; return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd)) {
        switch (operandTable[line->token[2].second]) {
            case Operand::HL: ML_DEC_ADDR_HL; return;
            case Operand::IX: ML_DEC_ADDR_IX(0); return;
            case Operand::IY: ML_DEC_ADDR_IY(0); return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::AddressBegin, TokenType::Numeric, TokenType::AddressEnd)) {
        int addr = atoi(line->token[2].second.c_str());
        if (mnemonic_range(line, addr, 0x0000, 0xFFFF)) {
            ML_PUSH_HL;
            ML_LD_L_n(addr & 0x00FF);
            ML_LD_H_n((addr & 0xFF00) >> 8);
            ML_DEC_ADDR_HL;
            ML_POP_HL;
            return;
        }
    } else if (mnemonic_format_test(line, 6, TokenType::AddressBegin, TokenType::Operand, TokenType::PlusOrMinus, TokenType::Numeric, TokenType::AddressEnd)) {
        int n = atoi(line->token[4].second.c_str());
        if (line->token[3].first == TokenType::Plus) {
            if (!mnemonic_range(line, n, 0, 127)) {
                return;
            }
        } else {
            if (!mnemonic_range(line, n, 0, 128)) {
                return;
            }
            n = 0 - n;
        }
        switch (operandTable[line->token[2].second]) {
            case Operand::IX: ML_DEC_ADDR_IX(n); return;
            case Operand::IY: ML_DEC_ADDR_IY(n); return;
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal DEC instruction.";
    }
}

void mnemonic_bit_op(LineData* line, Mnemonic mne)
{
    if (mnemonic_format_test(line, 4, TokenType::Numeric, TokenType::Split, TokenType::Operand)) {
        int b = atoi(line->token[1].second.c_str());
        if (!mnemonic_range(line, b, 0, 7)) {
            return;
        }
        b <<= 3;
        uint8_t r;
        switch (operandTable[line->token[3].second]) {
            case Operand::A: r = 0b111; break;
            case Operand::B: r = 0b000; break;
            case Operand::C: r = 0b001; break;
            case Operand::D: r = 0b010; break;
            case Operand::E: r = 0b011; break;
            case Operand::H: r = 0b100; break;
            case Operand::L: r = 0b101; break;
            default:
                line->error = true;
                line->errmsg = "Illegal BIT/SET/RES instruction.";
                return;
        }
        uint8_t c;
        switch (mne) {
            case Mnemonic::BIT: c = 0b01000000; break;
            case Mnemonic::SET: c = 0b11000000; break;
            case Mnemonic::RES: c = 0b10000000; break;
            default:
                line->error = true;
                line->errmsg = "Illegal BIT/SET/RES instruction.";
                return;
        }
        line->machine.push_back(0xCB);
        line->machine.push_back(c | b | r);
        return;
    } else if (mnemonic_format_test(line, 6, TokenType::Numeric, TokenType::Split, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd)) {
        bool setZero = true;
        switch (operandTable[line->token[4].second]) {
            case Operand::HL: setZero = false; break;
            case Operand::IX: line->machine.push_back(0xDD); break;
            case Operand::IY: line->machine.push_back(0xFD); break;
            default:
                line->error = true;
                line->errmsg = "Illegal BIT/SET/RES instruction.";
                return;
        }
        int b = atoi(line->token[1].second.c_str());
        if (!mnemonic_range(line, b, 0, 7)) {
            return;
        }
        b <<= 3;
        uint8_t c;
        switch (mne) {
            case Mnemonic::BIT: c = 0b01000110; break;
            case Mnemonic::SET: c = 0b11000110; break;
            case Mnemonic::RES: c = 0b10000110; break;
            default:
                line->error = true;
                line->errmsg = "Illegal BIT/SET/RES instruction.";
                return;
        }
        line->machine.push_back(0xCB);
        if (setZero) {
            line->machine.push_back(0x00);
        }
        line->machine.push_back(c | b);
        return;
    } else if (mnemonic_format_test(line, 8,
                                    TokenType::Numeric,
                                    TokenType::Split,
                                    TokenType::AddressBegin,
                                    TokenType::Operand,
                                    TokenType::PlusOrMinus,
                                    TokenType::Numeric,
                                    TokenType::AddressEnd)) {
        switch (operandTable[line->token[4].second]) {
            case Operand::IX: line->machine.push_back(0xDD); break;
            case Operand::IY: line->machine.push_back(0xFD); break;
            default:
                line->error = true;
                line->errmsg = "Illegal BIT/SET/RES instruction.";
                return;
        }
        int b = atoi(line->token[1].second.c_str());
        if (!mnemonic_range(line, b, 0, 7)) {
            return;
        }
        b <<= 3;
        int n = atoi(line->token[6].second.c_str());
        if (line->token[5].first == TokenType::Plus) {
            if (!mnemonic_range(line, n, 0, 127)) {
                return;
            }
        } else {
            if (!mnemonic_range(line, n, 0, 128)) {
                return;
            }
            n = 0 - n;
        }
        uint8_t c;
        switch (mne) {
            case Mnemonic::BIT: c = 0b01000110; break;
            case Mnemonic::SET: c = 0b11000110; break;
            case Mnemonic::RES: c = 0b10000110; break;
            default:
                line->error = true;
                line->errmsg = "Illegal BIT/SET/RES instruction.";
                return;
        }
        line->machine.push_back(0xCB);
        line->machine.push_back(n & 0xFF);
        line->machine.push_back(c | b);
        return;
    } else if ((mne == Mnemonic::SET || mne == Mnemonic::RES) &&
               mnemonic_format_test(line, 10,
                                    TokenType::Numeric,
                                    TokenType::Split,
                                    TokenType::AddressBegin,
                                    TokenType::Operand,
                                    TokenType::PlusOrMinus,
                                    TokenType::Numeric,
                                    TokenType::AddressEnd,
                                    TokenType::And,
                                    TokenType::Operand) &&
               operandTable[line->token[4].second] == Operand::IX &&
               operandTable[line->token[9].second] == Operand::B) {
        int b = atoi(line->token[1].second.c_str());
        if (!mnemonic_range(line, b, 0, 7)) {
            return;
        }
        b <<= 3;
        int n = atoi(line->token[6].second.c_str());
        if (line->token[5].first == TokenType::Plus) {
            if (!mnemonic_range(line, n, 0, 127)) {
                return;
            }
        } else {
            if (!mnemonic_range(line, n, 0, 128)) {
                return;
            }
            n = 0 - n;
        }
        uint8_t c = mne == Mnemonic::SET ? 0xC0 : 0x80;
        line->machine.push_back(0xDD);
        line->machine.push_back(0xCB);
        line->machine.push_back(n & 0xFF);
        line->machine.push_back(c | b);
        return;
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal BIT/SET/RES instruction.";
    }
}
