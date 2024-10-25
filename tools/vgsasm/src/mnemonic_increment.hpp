/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"
#include "mnemonic.h"

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
