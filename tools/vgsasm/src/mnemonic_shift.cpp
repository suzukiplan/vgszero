#include "common.h"
#include "mnemonic.h"

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
