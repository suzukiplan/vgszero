/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"
#include "mnemonic.h"

static bool isReg8(Operand op)
{
    return op == Operand::A ||
           op == Operand::B ||
           op == Operand::C ||
           op == Operand::D ||
           op == Operand::E ||
           op == Operand::H ||
           op == Operand::L ||
           op == Operand::IXH ||
           op == Operand::IXL ||
           op == Operand::IYH ||
           op == Operand::IYL;
}

static bool isIX(Operand op)
{
    return op == Operand::IXH ||
           op == Operand::IXL ||
           op == Operand::IX;
}

static bool isIY(Operand op)
{
    return op == Operand::IYH ||
           op == Operand::IYL ||
           op == Operand::IY;
}

static uint8_t getBitReg8(Operand op)
{
    switch (op) {
        case Operand::B: return 0x00;
        case Operand::C: return 0x01;
        case Operand::D: return 0x02;
        case Operand::E: return 0x03;
        case Operand::H: return 0x04;
        case Operand::L: return 0x05;
        case Operand::A: return 0x07;
        case Operand::IXH: return 0x04;
        case Operand::IXL: return 0x05;
        case Operand::IYH: return 0x04;
        case Operand::IYL: return 0x05;
    }
    return 0xFF;
}

void mnemonic_LD(LineData* line)
{
    if (line->isAssignmnt && mnemonic_format_begin(line, 3, TokenType::Numeric, TokenType::Split)) {
        // 代入式のLDでは左辺が数値の場合アドレスに自動変換する
        line->token.insert(line->token.begin() + 1, std::make_pair(TokenType::AddressBegin, "("));
        line->token.insert(line->token.begin() + 3, std::make_pair(TokenType::AddressEnd, ")"));
    }

    if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Operand)) {
        // LD r, r'
        auto op1 = operandTable[line->token[1].second];
        auto op2 = operandTable[line->token[3].second];
        if (isReg8(op1) && isReg8(op2)) {
            if (op1 == Operand::IXH || op1 == Operand::IXL || op1 == Operand::IYH || op1 == Operand::IYL) {
                bool isOp1IX = op1 == Operand::IXH || op1 == Operand::IXL;
                bool isOp1IY = op1 == Operand::IYH || op1 == Operand::IYL;
                if (op2 == Operand::H || op2 == Operand::L) {
                    line->error = true;
                    if (isOp1IX) {
                        line->errmsg = "LD {IXH|IXL},{H|L} is not supported.";
                    } else {
                        line->errmsg = "LD {IYH|IYL},{H|L} is not supported.";
                    }
                    return;
                }
                bool isOp2IX = op2 == Operand::IXH || op2 == Operand::IXL;
                bool isOp2IY = op2 == Operand::IYH || op2 == Operand::IYL;
                if (isOp1IX && isOp2IY) {
                    line->error = true;
                    line->errmsg = "LD {IXH|IXL},{IYH|IYL} is not supported.";
                    return;
                }
                if (isOp1IY && isOp2IX) {
                    line->error = true;
                    line->errmsg = "LD {IYH|IYL},{IXH|IXL} is not supported.";
                    return;
                }
                line->machine.push_back(isOp1IX ? 0xDD : 0xFD);
                uint8_t code = 0x60;
                code |= getBitReg8(op1) << 3;
                code |= getBitReg8(op2);
                line->machine.push_back(code);
            } else {
                if (op2 == Operand::IXH || op2 == Operand::IXL) {
                    if (op1 == Operand::H || op1 == Operand::L) {
                        line->error = true;
                        line->errmsg = "LD {H|L},{IXH|IXL} is not supported.";
                        return;
                    }
                    line->machine.push_back(0xDD);
                } else if (op2 == Operand::IYH || op2 == Operand::IYL) {
                    if (op1 == Operand::H || op1 == Operand::L) {
                        // LD {H|L}, {IYH,IYL}
                        line->error = true;
                        line->errmsg = "LD {H|L},{IYH|IYL} is not supported.";
                        return;
                    }
                    line->machine.push_back(0xFD);
                }
                uint8_t code = 0x40;
                code |= getBitReg8(op1) << 3;
                code |= getBitReg8(op2);
                line->machine.push_back(code);
            }
            return;
        } else {
            switch (op1) {
                case Operand::BC:
                    switch (op2) {
                        case Operand::BC: ML_LD_BC_BC; return;
                        case Operand::DE: ML_LD_BC_DE; return;
                        case Operand::HL: ML_LD_BC_HL; return;
                        case Operand::IX: ML_LD_BC_IX; return;
                        case Operand::IY: ML_LD_BC_IY; return;
                    }
                    break;
                case Operand::DE:
                    switch (op2) {
                        case Operand::BC: ML_LD_DE_BC; return;
                        case Operand::DE: ML_LD_DE_DE; return;
                        case Operand::HL: ML_LD_DE_HL; return;
                        case Operand::IX: ML_LD_DE_IX; return;
                        case Operand::IY: ML_LD_DE_IY; return;
                    }
                    break;
                case Operand::HL:
                    switch (op2) {
                        case Operand::BC: ML_LD_HL_BC; return;
                        case Operand::DE: ML_LD_HL_DE; return;
                        case Operand::HL: ML_LD_HL_HL; return;
                        case Operand::IX: ML_LD_HL_IX; return;
                        case Operand::IY: ML_LD_HL_IY; return;
                    }
                    break;
                case Operand::IX:
                    switch (op2) {
                        case Operand::BC: ML_LD_IX_BC; return;
                        case Operand::DE: ML_LD_IX_DE; return;
                        case Operand::HL: ML_LD_IX_HL; return;
                        case Operand::IX: ML_LD_IX_IX; return;
                        case Operand::IY: ML_LD_IX_IY; return;
                    }
                    break;
                case Operand::IY:
                    switch (op2) {
                        case Operand::BC: ML_LD_IY_BC; return;
                        case Operand::DE: ML_LD_IY_DE; return;
                        case Operand::HL: ML_LD_IY_HL; return;
                        case Operand::IX: ML_LD_IY_IX; return;
                        case Operand::IY: ML_LD_IY_IY; return;
                    }
                    break;
                case Operand::SP:
                    switch (op2) {
                        case Operand::HL: ML_LD_SP_HL; return;
                        case Operand::IX: ML_LD_SP_IX; return;
                        case Operand::IY: ML_LD_SP_IY; return;
                    }
            }
        }
    } else if (mnemonic_format_test(line, 6, TokenType::Operand, TokenType::Split, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd) &&
               operandTable[line->token[4].second] == Operand::HL) {
        // LD r, (HL)
        switch (operandTable[line->token[1].second]) {
            case Operand::A: ML_LD_A_HL; return;
            case Operand::B: ML_LD_B_HL; return;
            case Operand::C: ML_LD_C_HL; return;
            case Operand::D: ML_LD_D_HL; return;
            case Operand::E: ML_LD_E_HL; return;
            case Operand::F: break; // this opcode $76 is HALT
            case Operand::H: ML_LD_H_HL; return;
            case Operand::L: ML_LD_L_HL; return;
            case Operand::IXH: ML_LD_IXH_HL; return;
            case Operand::IXL: ML_LD_IXL_HL; return;
            case Operand::IYH: ML_LD_IYH_HL; return;
            case Operand::IYL: ML_LD_IYL_HL; return;
            case Operand::BC:
                ML_LD_C_HL;
                ML_INC_HL;
                ML_LD_B_HL;
                ML_DEC_HL;
                return;
            case Operand::DE:
                ML_LD_E_HL;
                ML_INC_HL;
                ML_LD_D_HL;
                ML_DEC_HL;
                return;
            case Operand::IX:
                ML_PUSH_AF;
                ML_LD_A_HL;
                ML_LD_IXL_A;
                ML_INC_HL;
                ML_LD_A_HL;
                ML_LD_IXH_A;
                ML_DEC_HL;
                ML_POP_AF;
                return;
            case Operand::IY:
                ML_PUSH_AF;
                ML_LD_A_HL;
                ML_LD_IYL_A;
                ML_INC_HL;
                ML_LD_A_HL;
                ML_LD_IYH_A;
                ML_DEC_HL;
                ML_POP_AF;
                return;
        }
    } else if (mnemonic_format_test(line, 6, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd, TokenType::Split, TokenType::Operand) &&
               operandTable[line->token[2].second] == Operand::HL) {
        // LD (HL), r
        switch (operandTable[line->token[5].second]) {
            case Operand::A: ML_LD_HL_A; return;
            case Operand::B: ML_LD_HL_B; return;
            case Operand::C: ML_LD_HL_C; return;
            case Operand::D: ML_LD_HL_D; return;
            case Operand::E: ML_LD_HL_E; return;
            case Operand::F: break; // this opcode $76 is HALT
            case Operand::H: ML_LD_HL_H; return;
            case Operand::L: ML_LD_HL_L; return;
            case Operand::IXH: ML_LD_HL_IXH; return;
            case Operand::IXL: ML_LD_HL_IXL; return;
            case Operand::IYH: ML_LD_HL_IYH; return;
            case Operand::IYL: ML_LD_HL_IYL; return;
            // register pair support
            case Operand::BC:
                ML_LD_HL_C;
                ML_INC_HL;
                ML_LD_HL_B;
                ML_DEC_HL;
                return;
            case Operand::DE:
                ML_LD_HL_E;
                ML_INC_HL;
                ML_LD_HL_D;
                ML_DEC_HL;
                return;
            case Operand::IX:
                ML_PUSH_AF;
                ML_LD_A_IXL;
                ML_LD_HL_A;
                ML_INC_HL;
                ML_LD_A_IXH;
                ML_LD_HL_A;
                ML_DEC_HL;
                ML_POP_AF;
                return;
            case Operand::IY:
                ML_PUSH_AF;
                ML_LD_A_IYL;
                ML_LD_HL_A;
                ML_INC_HL;
                ML_LD_A_IYH;
                ML_LD_HL_A;
                ML_DEC_HL;
                ML_POP_AF;
                return;
        }
    } else if (mnemonic_format_test(line, 6, TokenType::Operand, TokenType::Split, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd) &&
               operandTable[line->token[1].second] == Operand::A &&
               (operandTable[line->token[4].second] == Operand::BC ||
                operandTable[line->token[4].second] == Operand::DE)) {
        switch (operandTable[line->token[4].second]) {
            case Operand::BC: line->machine.push_back(0x0A); return;
            case Operand::DE: line->machine.push_back(0x1A); return;
        }
    } else if (mnemonic_format_test(line, 6, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd, TokenType::Split, TokenType::Operand) &&
               operandTable[line->token[5].second] == Operand::A &&
               (operandTable[line->token[2].second] == Operand::BC ||
                operandTable[line->token[2].second] == Operand::DE)) {
        switch (operandTable[line->token[2].second]) {
            case Operand::BC: line->machine.push_back(0x02); return;
            case Operand::DE: line->machine.push_back(0x12); return;
        }
    } else if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::Numeric)) {
        // `LD r, n` or `LD rr, nn`
        auto op = operandTable[line->token[1].second];
        auto n = atoi(line->token[3].second.c_str());
        if (isReg8(op)) {
            auto b = getBitReg8(op);
            b <<= 3;
            if (mnemonic_range(line, n, -128, 255)) {
                if (isIX(op)) {
                    line->machine.push_back(0xDD);
                } else if (isIY(op)) {
                    line->machine.push_back(0xFD);
                }
                line->machine.push_back(0x06 | b);
                line->machine.push_back(n & 0xFF);
                return;
            }
        } else {
            if (mnemonic_range(line, n, -32768, 65535)) {
                uint8_t code = 0x00;
                switch (op) {
                    case Operand::BC: code = 0x01; break;
                    case Operand::DE: code = 0x11; break;
                    case Operand::HL: code = 0x21; break;
                    case Operand::SP: code = 0x31; break;
                    case Operand::IX: code = 0x21; break;
                    case Operand::IY: code = 0x21; break;
                }
                if (code) {
                    if (isIX(op)) {
                        line->machine.push_back(0xDD);
                    } else if (isIY(op)) {
                        line->machine.push_back(0xFD);
                    }
                    line->machine.push_back(code);
                    line->machine.push_back(n & 0x00FF);
                    line->machine.push_back((n & 0xFF00) >> 8);
                    return;
                }
            }
        }
    } else if (mnemonic_format_test(line, 4, TokenType::Operand, TokenType::Split, TokenType::LabelJump)) {
        auto op = operandTable[line->token[1].second];
        uint8_t code = 0x00;
        switch (op) {
            case Operand::BC: code = 0x01; break;
            case Operand::DE: code = 0x11; break;
            case Operand::HL: code = 0x21; break;
            case Operand::SP: code = 0x31; break;
            case Operand::IX: code = 0x21; break;
            case Operand::IY: code = 0x21; break;
        }
        if (code) {
            if (isIX(op)) {
                line->machine.push_back(0xDD);
            } else if (isIY(op)) {
                line->machine.push_back(0xFD);
            }
            line->machine.push_back(code);
            tempAddrs.push_back(new TempAddr(line, line->token[3].second, line->machine.size(), false));
            line->machine.push_back(0x00);
            line->machine.push_back(0x00);
            return;
        }
    } else if (mnemonic_format_test(line, 6, TokenType::Operand, TokenType::Split, TokenType::AddressBegin, TokenType::Numeric, TokenType::AddressEnd)) {
        // LD r, (nn)
        auto op = operandTable[line->token[1].second];
        uint16_t code = 0x00;
        switch (op) {
            case Operand::A: code = 0x3A; break;
            case Operand::HL: code = 0x2A; break;
            case Operand::BC: code = 0xED4B; break;
            case Operand::DE: code = 0xED5B; break;
            case Operand::SP: code = 0xED7B; break;
            case Operand::IX: code = 0xDD2A; break;
            case Operand::IY: code = 0xFD2A; break;
        }
        auto addr = atoi(line->token[4].second.c_str());
        if (mnemonic_range(line, addr, 0x0000, 0xFFFF)) {
            switch (op) {
                case Operand::B:
                    ML_PUSH_AF;
                    ML_LD_A_NN(addr);
                    ML_LD_B_A;
                    ML_POP_AF;
                    return;
                case Operand::C:
                    ML_PUSH_AF;
                    ML_LD_A_NN(addr);
                    ML_LD_C_A;
                    ML_POP_AF;
                    return;
                case Operand::D:
                    ML_PUSH_AF;
                    ML_LD_A_NN(addr);
                    ML_LD_D_A;
                    ML_POP_AF;
                    return;
                case Operand::E:
                    ML_PUSH_AF;
                    ML_LD_A_NN(addr);
                    ML_LD_E_A;
                    ML_POP_AF;
                    return;
                case Operand::H:
                    ML_PUSH_AF;
                    ML_LD_A_NN(addr);
                    ML_LD_H_A;
                    ML_POP_AF;
                    return;
                case Operand::L:
                    ML_PUSH_AF;
                    ML_LD_A_NN(addr);
                    ML_LD_L_A;
                    ML_POP_AF;
                    return;
                case Operand::IXH:
                    ML_PUSH_AF;
                    ML_LD_A_NN(addr);
                    ML_LD_IXH_A;
                    ML_POP_AF;
                    return;
                case Operand::IXL:
                    ML_PUSH_AF;
                    ML_LD_A_NN(addr);
                    ML_LD_IXL_A;
                    ML_POP_AF;
                    return;
                case Operand::IYH:
                    ML_PUSH_AF;
                    ML_LD_A_NN(addr);
                    ML_LD_IYH_A;
                    ML_POP_AF;
                    return;
                case Operand::IYL:
                    ML_PUSH_AF;
                    ML_LD_A_NN(addr);
                    ML_LD_IYL_A;
                    ML_POP_AF;
                    return;
            }
            if (code) {
                if (0x100 <= code) { line->machine.push_back((code & 0xFF00) >> 8); }
                line->machine.push_back(code & 0xFF);
                line->machine.push_back(addr & 0x00FF);
                line->machine.push_back((addr & 0xFF00) >> 8);
                return;
            }
        }
    } else if (mnemonic_format_test(line, 6, TokenType::Operand, TokenType::Split, TokenType::AddressBegin, TokenType::LabelJump, TokenType::AddressEnd)) {
        // LD r, (nn)
        auto op = operandTable[line->token[1].second];
        uint16_t code = 0x00;
        switch (op) {
            case Operand::A: code = 0x3A; break;
            case Operand::HL: code = 0x2A; break;
            case Operand::BC: code = 0xED4B; break;
            case Operand::DE: code = 0xED5B; break;
            case Operand::SP: code = 0xED7B; break;
            case Operand::IX: code = 0xDD2A; break;
            case Operand::IY: code = 0xFD2A; break;
        }
        if (code) {
            if (0x100 <= code) { line->machine.push_back((code & 0xFF00) >> 8); }
            line->machine.push_back(code & 0xFF);
            tempAddrs.push_back(new TempAddr(line, line->token[4].second, line->machine.size(), false));
            line->machine.push_back(0x00);
            line->machine.push_back(0x00);
            return;
        } else {
            switch (op) {
                case Operand::B:
                    ML_PUSH_AF;
                    line->machine.push_back(0x3A);
                    tempAddrs.push_back(new TempAddr(line, line->token[4].second, line->machine.size(), false));
                    line->machine.push_back(0);
                    line->machine.push_back(0);
                    ML_LD_B_A;
                    ML_POP_AF;
                    return;
                case Operand::C:
                    ML_PUSH_AF;
                    line->machine.push_back(0x3A);
                    tempAddrs.push_back(new TempAddr(line, line->token[4].second, line->machine.size(), false));
                    line->machine.push_back(0);
                    line->machine.push_back(0);
                    ML_LD_C_A;
                    ML_POP_AF;
                    return;
                case Operand::D:
                    ML_PUSH_AF;
                    line->machine.push_back(0x3A);
                    tempAddrs.push_back(new TempAddr(line, line->token[4].second, line->machine.size(), false));
                    line->machine.push_back(0);
                    line->machine.push_back(0);
                    ML_LD_D_A;
                    ML_POP_AF;
                    return;
                case Operand::E:
                    ML_PUSH_AF;
                    line->machine.push_back(0x3A);
                    tempAddrs.push_back(new TempAddr(line, line->token[4].second, line->machine.size(), false));
                    line->machine.push_back(0);
                    line->machine.push_back(0);
                    ML_LD_E_A;
                    ML_POP_AF;
                    return;
                case Operand::H:
                    ML_PUSH_AF;
                    line->machine.push_back(0x3A);
                    tempAddrs.push_back(new TempAddr(line, line->token[4].second, line->machine.size(), false));
                    line->machine.push_back(0);
                    line->machine.push_back(0);
                    ML_LD_H_A;
                    ML_POP_AF;
                    return;
                case Operand::L:
                    ML_PUSH_AF;
                    line->machine.push_back(0x3A);
                    tempAddrs.push_back(new TempAddr(line, line->token[4].second, line->machine.size(), false));
                    line->machine.push_back(0);
                    line->machine.push_back(0);
                    ML_LD_L_A;
                    ML_POP_AF;
                    return;
                case Operand::IXH:
                    ML_PUSH_AF;
                    line->machine.push_back(0x3A);
                    tempAddrs.push_back(new TempAddr(line, line->token[4].second, line->machine.size(), false));
                    line->machine.push_back(0);
                    line->machine.push_back(0);
                    ML_LD_IXH_A;
                    ML_POP_AF;
                    return;
                case Operand::IXL:
                    ML_PUSH_AF;
                    line->machine.push_back(0x3A);
                    tempAddrs.push_back(new TempAddr(line, line->token[4].second, line->machine.size(), false));
                    line->machine.push_back(0);
                    line->machine.push_back(0);
                    ML_LD_IXL_A;
                    ML_POP_AF;
                    return;
                case Operand::IYH:
                    ML_PUSH_AF;
                    line->machine.push_back(0x3A);
                    tempAddrs.push_back(new TempAddr(line, line->token[4].second, line->machine.size(), false));
                    line->machine.push_back(0);
                    line->machine.push_back(0);
                    ML_LD_IYH_A;
                    ML_POP_AF;
                    return;
                case Operand::IYL:
                    ML_PUSH_AF;
                    line->machine.push_back(0x3A);
                    tempAddrs.push_back(new TempAddr(line, line->token[4].second, line->machine.size(), false));
                    line->machine.push_back(0);
                    line->machine.push_back(0);
                    ML_LD_IYL_A;
                    ML_POP_AF;
                    return;
            }
        }
    } else if (mnemonic_format_test(line, 6, TokenType::AddressBegin, TokenType::Numeric, TokenType::AddressEnd, TokenType::Split, TokenType::Operand)) {
        // LD (nn),r
        auto addr = atoi(line->token[2].second.c_str());
        if (mnemonic_range(line, addr, 0x0000, 0xFFFF)) {
            switch (operandTable[line->token[5].second]) {
                case Operand::A: ML_LD_NN_A(addr); return;
                case Operand::BC: ML_LD_NN_BC(addr); return;
                case Operand::DE: ML_LD_NN_DE(addr); return;
                case Operand::HL: ML_LD_NN_HL(addr); return;
                case Operand::SP: ML_LD_NN_SP(addr); return;
                case Operand::IX: ML_LD_NN_IX(addr); return;
                case Operand::IY: ML_LD_NN_IY(addr); return;
                case Operand::B:
                    ML_PUSH_AF;
                    ML_LD_A_B;
                    ML_LD_NN_A(addr);
                    ML_POP_AF;
                    return;
                case Operand::C:
                    ML_PUSH_AF;
                    ML_LD_A_C;
                    ML_LD_NN_A(addr);
                    ML_POP_AF;
                    return;
                case Operand::D:
                    ML_PUSH_AF;
                    ML_LD_A_D;
                    ML_LD_NN_A(addr);
                    ML_POP_AF;
                    return;
                case Operand::E:
                    ML_PUSH_AF;
                    ML_LD_A_E;
                    ML_LD_NN_A(addr);
                    ML_POP_AF;
                    return;
                case Operand::H:
                    ML_PUSH_AF;
                    ML_LD_A_H;
                    ML_LD_NN_A(addr);
                    ML_POP_AF;
                    return;
                case Operand::L:
                    ML_PUSH_AF;
                    ML_LD_A_L;
                    ML_LD_NN_A(addr);
                    ML_POP_AF;
                    return;
                case Operand::IXH:
                    ML_PUSH_AF;
                    ML_LD_A_IXH;
                    ML_LD_NN_A(addr);
                    ML_POP_AF;
                    return;
                case Operand::IXL:
                    ML_PUSH_AF;
                    ML_LD_A_IXL;
                    ML_LD_NN_A(addr);
                    ML_POP_AF;
                    return;
                case Operand::IYH:
                    ML_PUSH_AF;
                    ML_LD_A_IYH;
                    ML_LD_NN_A(addr);
                    ML_POP_AF;
                    return;
                case Operand::IYL:
                    ML_PUSH_AF;
                    ML_LD_A_IYL;
                    ML_LD_NN_A(addr);
                    ML_POP_AF;
                    return;
            }
        }
    } else if (mnemonic_format_test(line, 6, TokenType::AddressBegin, TokenType::LabelJump, TokenType::AddressEnd, TokenType::Split, TokenType::Operand)) {
        switch (operandTable[line->token[5].second]) {
            case Operand::A:
                ML_LD_NN_A(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                return;
            case Operand::BC:
                ML_LD_NN_BC(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                return;
            case Operand::DE:
                ML_LD_NN_DE(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                return;
            case Operand::HL:
                ML_LD_NN_HL(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                return;
            case Operand::SP:
                ML_LD_NN_SP(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                return;
            case Operand::IX:
                ML_LD_NN_IX(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                return;
            case Operand::IY:
                ML_LD_NN_IY(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                return;
            case Operand::B:
                ML_PUSH_AF;
                ML_LD_A_B;
                ML_LD_NN_A(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                ML_POP_AF;
                return;
            case Operand::C:
                ML_PUSH_AF;
                ML_LD_A_C;
                ML_LD_NN_A(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                ML_POP_AF;
                return;
            case Operand::D:
                ML_PUSH_AF;
                ML_LD_A_D;
                ML_LD_NN_A(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                ML_POP_AF;
                return;
            case Operand::E:
                ML_PUSH_AF;
                ML_LD_A_E;
                ML_LD_NN_A(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                ML_POP_AF;
                return;
            case Operand::H:
                ML_PUSH_AF;
                ML_LD_A_H;
                ML_LD_NN_A(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                ML_POP_AF;
                return;
            case Operand::L:
                ML_PUSH_AF;
                ML_LD_A_L;
                ML_LD_NN_A(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                ML_POP_AF;
                return;
            case Operand::IXH:
                ML_PUSH_AF;
                ML_LD_A_IXH;
                ML_LD_NN_A(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                ML_POP_AF;
                return;
            case Operand::IXL:
                ML_PUSH_AF;
                ML_LD_A_IXL;
                ML_LD_NN_A(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                ML_POP_AF;
                return;
            case Operand::IYH:
                ML_PUSH_AF;
                ML_LD_A_IYH;
                ML_LD_NN_A(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                ML_POP_AF;
                return;
            case Operand::IYL:
                ML_PUSH_AF;
                ML_LD_A_IYL;
                ML_LD_NN_A(0);
                tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
                ML_POP_AF;
                return;
        }
    } else if (mnemonic_format_test(line, 6, TokenType::AddressBegin, TokenType::Numeric, TokenType::AddressEnd, TokenType::Split, TokenType::Numeric)) {
        // LD (nn),n
        auto addr = atoi(line->token[2].second.c_str());
        auto n = atoi(line->token[5].second.c_str());
        if (mnemonic_range(line, addr, 0x0000, 0xFFFF) && mnemonic_range(line, n, -128, 255)) {
            ML_PUSH_AF;
            ML_LD_A_n(n);
            ML_LD_NN_A(addr);
            ML_POP_AF;
            return;
        }
    } else if (mnemonic_format_test(line, 6, TokenType::AddressBegin, TokenType::LabelJump, TokenType::AddressEnd, TokenType::Split, TokenType::Numeric)) {
        // LD (LABEL),n
        auto n = atoi(line->token[5].second.c_str());
        if (mnemonic_range(line, n, -128, 255)) {
            ML_PUSH_AF;
            ML_LD_A_n(n);
            ML_LD_NN_A(0);
            tempAddrs.push_back(new TempAddr(line, line->token[2].second, line->machine.size() - 2, false));
            ML_POP_AF;
            return;
        }
    } else if (mnemonic_format_test(line, 8, TokenType::Operand, TokenType::Split, TokenType::AddressBegin, TokenType::Operand, TokenType::PlusOrMinus, TokenType::Numeric, TokenType::AddressEnd)) {
        // LD r, ({IX|IY}{+|-}d)
        auto op1 = operandTable[line->token[1].second];
        auto op2 = operandTable[line->token[4].second];
        auto isPlus = line->token[5].first == TokenType::Plus;
        auto d = atoi(line->token[6].second.c_str());
        if (op2 == Operand::IX || op2 == Operand::IY) {
            if (!isPlus) { d = 0 - d; }
            if (isReg8(op1) && !isIX(op1) && !isIY(op1) && mnemonic_range(line, d, -128, 127)) {
                line->machine.push_back(isIX(op2) ? 0xDD : 0xFD);
                line->machine.push_back(0x46 | (getBitReg8(op1) << 3));
                line->machine.push_back(d);
                return;
            } else {
                if (op2 == Operand::IX) {
                    switch (op1) {
                        case Operand::BC:
                            ML_LD_C_IX(d);
                            ML_LD_B_IX(d + 1);
                            return;
                        case Operand::DE:
                            ML_LD_E_IX(d);
                            ML_LD_D_IX(d + 1);
                            return;
                        case Operand::HL:
                            ML_LD_L_IX(d);
                            ML_LD_H_IX(d + 1);
                            return;
                    }
                } else {
                    switch (op1) {
                        case Operand::BC:
                            ML_LD_C_IY(d);
                            ML_LD_B_IY(d + 1);
                            return;
                        case Operand::DE:
                            ML_LD_E_IY(d);
                            ML_LD_D_IY(d + 1);
                            return;
                        case Operand::HL:
                            ML_LD_L_IY(d);
                            ML_LD_H_IY(d + 1);
                            return;
                    }
                }
            }
        }
    } else if (mnemonic_format_test(line, 6, TokenType::Operand, TokenType::Split, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd)) {
        // LD r, ({IX|IY})
        auto op1 = operandTable[line->token[1].second];
        auto op2 = operandTable[line->token[4].second];
        if (op2 == Operand::IX || op2 == Operand::IY) {
            if (isReg8(op1) && !isIX(op1) && !isIY(op1)) {
                line->machine.push_back(isIX(op2) ? 0xDD : 0xFD);
                line->machine.push_back(0x46 | (getBitReg8(op1) << 3));
                line->machine.push_back(0x00);
                return;
            } else {
                if (op2 == Operand::IX) {
                    switch (op1) {
                        case Operand::BC:
                            ML_LD_C_IX(0);
                            ML_LD_B_IX(1);
                            return;
                        case Operand::DE:
                            ML_LD_E_IX(0);
                            ML_LD_D_IX(1);
                            return;
                        case Operand::HL:
                            ML_LD_L_IX(0);
                            ML_LD_H_IX(1);
                            return;
                    }
                } else {
                    switch (op1) {
                        case Operand::BC:
                            ML_LD_C_IY(0);
                            ML_LD_B_IY(1);
                            return;
                        case Operand::DE:
                            ML_LD_E_IY(0);
                            ML_LD_D_IY(1);
                            return;
                        case Operand::HL:
                            ML_LD_L_IY(0);
                            ML_LD_H_IY(1);
                            return;
                    }
                }
            }
        }
    } else if (mnemonic_format_test(line, 8, TokenType::AddressBegin, TokenType::Operand, TokenType::PlusOrMinus, TokenType::Numeric, TokenType::AddressEnd, TokenType::Split, TokenType::Operand)) {
        // LD ({IX|IY}{+|-}d), r
        auto op1 = operandTable[line->token[7].second];
        auto op2 = operandTable[line->token[2].second];
        auto isPlus = line->token[3].first == TokenType::Plus;
        auto d = atoi(line->token[4].second.c_str());
        if (op2 == Operand::IX || op2 == Operand::IY) {
            if (!isPlus) { d = 0 - d; }
            if (isReg8(op1) && !isIX(op1) && !isIY(op1) && mnemonic_range(line, d, -128, 127)) {
                line->machine.push_back(isIX(op2) ? 0xDD : 0xFD);
                line->machine.push_back(0x70 | getBitReg8(op1));
                line->machine.push_back(d);
                return;
            } else {
                if (op2 == Operand::IX) {
                    switch (op1) {
                        case Operand::BC:
                            ML_LD_IX_C(d);
                            ML_LD_IX_B(d + 1);
                            return;
                        case Operand::DE:
                            ML_LD_IX_E(d);
                            ML_LD_IX_D(d + 1);
                            return;
                        case Operand::HL:
                            ML_LD_IX_L(d);
                            ML_LD_IX_H(d + 1);
                            return;
                    }
                } else {
                    switch (op1) {
                        case Operand::BC:
                            ML_LD_IY_C(d);
                            ML_LD_IY_B(d + 1);
                            return;
                        case Operand::DE:
                            ML_LD_IY_E(d);
                            ML_LD_IY_D(d + 1);
                            return;
                        case Operand::HL:
                            ML_LD_IY_L(d);
                            ML_LD_IY_H(d + 1);
                            return;
                    }
                }
            }
        }
    } else if (mnemonic_format_test(line, 6, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd, TokenType::Split, TokenType::Operand)) {
        // LD ({IX|IY}), r
        auto op1 = operandTable[line->token[5].second];
        auto op2 = operandTable[line->token[2].second];
        if (op2 == Operand::IX || op2 == Operand::IY) {
            if (isReg8(op1) && !isIX(op1) && !isIY(op1)) {
                line->machine.push_back(isIX(op2) ? 0xDD : 0xFD);
                line->machine.push_back(0x70 | getBitReg8(op1));
                line->machine.push_back(0x00);
                return;
            } else {
                if (op2 == Operand::IX) {
                    switch (op1) {
                        case Operand::BC:
                            ML_LD_IX_C(0);
                            ML_LD_IX_B(1);
                            return;
                        case Operand::DE:
                            ML_LD_IX_E(0);
                            ML_LD_IX_D(1);
                            return;
                        case Operand::HL:
                            ML_LD_IX_L(0);
                            ML_LD_IX_H(1);
                            return;
                    }
                } else {
                    switch (op1) {
                        case Operand::BC:
                            ML_LD_IY_C(0);
                            ML_LD_IY_B(1);
                            return;
                        case Operand::DE:
                            ML_LD_IY_E(0);
                            ML_LD_IY_D(1);
                            return;
                        case Operand::HL:
                            ML_LD_IY_L(0);
                            ML_LD_IY_H(1);
                            return;
                    }
                }
            }
        }
    } else if (mnemonic_format_test(line, 8, TokenType::AddressBegin, TokenType::Operand, TokenType::PlusOrMinus, TokenType::Numeric, TokenType::AddressEnd, TokenType::Split, TokenType::Numeric)) {
        // LD ({IX|IY}{+|-}d), n
        auto op = operandTable[line->token[2].second];
        auto isPlus = line->token[3].first == TokenType::Plus;
        auto d = atoi(line->token[4].second.c_str());
        auto n = atoi(line->token[7].second.c_str());
        if (op == Operand::IX || op == Operand::IY) {
            if (!isPlus) { d = 0 - d; }
            if (mnemonic_range(line, d, -128, 127) && mnemonic_range(line, n, -128, 255)) {
                line->machine.push_back(isIX(op) ? 0xDD : 0xFD);
                line->machine.push_back(0x36);
                line->machine.push_back(d & 0xFF);
                line->machine.push_back(n & 0xFF);
                return;
            }
        }
    } else if (mnemonic_format_test(line, 6, TokenType::AddressBegin, TokenType::Operand, TokenType::AddressEnd, TokenType::Split, TokenType::Numeric)) {
        // LD ({BC|DE|IX|IY}), n
        auto op = operandTable[line->token[2].second];
        auto n = atoi(line->token[5].second.c_str());
        if (mnemonic_range(line, n, -128, 255)) {
            switch (op) {
                case Operand::IX:
                case Operand::IY:
                    line->machine.push_back(isIX(op) ? 0xDD : 0xFD);
                    line->machine.push_back(0x36);
                    line->machine.push_back(0x00);
                    line->machine.push_back(n & 0xFF);
                    return;
                case Operand::HL:
                    line->machine.push_back(0x36);
                    line->machine.push_back(n);
                    return;
                case Operand::BC:
                    ML_PUSH_HL;
                    ML_LD_H_B;
                    ML_LD_L_C;
                    line->machine.push_back(0x36);
                    line->machine.push_back(n);
                    ML_POP_HL;
                    return;
                case Operand::DE:
                    ML_PUSH_HL;
                    ML_LD_H_D;
                    ML_LD_L_E;
                    line->machine.push_back(0x36);
                    line->machine.push_back(n);
                    ML_POP_HL;
                    return;
            }
        }
    }
    if (!line->error) {
        line->error = true;
        line->errmsg = "Illegal LD instruction.";
    }
}
