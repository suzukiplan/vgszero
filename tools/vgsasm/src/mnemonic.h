/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

class TempAddr
{
  public:
    LineData* line;
    std::string label;
    int midx;
    bool isRelative;
    TempAddr(LineData* line, std::string label, int midx, bool isRelative)
    {
        this->line = line;
        this->label = label;
        this->midx = midx;
        this->isRelative = isRelative;
    }
};

extern std::vector<TempAddr*> tempAddrs;

enum class Mnemonic {
    None = 0,
    LD,
    LDI,
    LDD,
    LDIR,
    LDDR,
    PUSH,
    POP,
    EX,
    EXX,
    CP,
    CPI,
    CPIR,
    CPD,
    CPDR,
    ADD,
    ADC,
    SUB,
    SBC,
    AND,
    OR,
    XOR,
    INC,
    DEC,
    DAA,
    CPL,
    NEG,
    CCF,
    SCF,
    NOP,
    HALT,
    RL,
    RLA,
    RLC,
    RLCA,
    RR,
    RRA,
    RRC,
    RRCA,
    SLA,
    SLL,
    SRA,
    SRL,
    RLD,
    RRD,
    BIT,
    SET,
    RES,
    JP,
    JR,
    DJNZ,
    CALL,
    RET,
    RETI,
    RETN,
    RST,
    OUT,
    OUTI,
    OTIR,
    OUTD,
    OTDR,
    IN,
    INI,
    INIR,
    IND,
    INDR,
    DI,
    EI,
    IM,
    DB,
    DW,
    MUL,  // VGS-Zero
    MULS, // VGS-Zero
    DIV,  // VGS-Zero
    DIVS, // VGS-Zero
    MOD,  // VGS-Zero
    ATN2, // VGS-Zero
    SIN,  // VGS-Zero
    COS,  // VGS-Zero
};

bool mnemonic_format_check(LineData* line, int size, ...);
bool mnemonic_format_test(LineData* line, int size, ...);
bool mnemonic_format_begin(LineData* line, int size, ...);
bool mnemonic_range(LineData* line, int n, int min, int max);
bool mnemonic_is_reg16(Operand op);
void mnemonic_single(LineData* line, uint8_t code);
void mnemonic_single_ED(LineData* line, uint8_t code);
bool operand_is_condition(Operand op);
bool operand_is_condition(std::string str);
void parse_operand(LineData* line);
void parse_mneoimonic(LineData* line);
void mnemonic_syntax_check(std::vector<LineData*>* lines);
