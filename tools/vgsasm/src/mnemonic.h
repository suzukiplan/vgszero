/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"
#include "operand.hpp"

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

bool mnemonic_format_check(LineData* line, int size, ...);
bool mnemonic_format_test(LineData* line, int size, ...);
bool mnemonic_format_begin(LineData* line, int size, ...);
bool mnemonic_range(LineData* line, int n, int min, int max);
bool mnemonic_is_reg16(Operand op);
void mnemonic_single(LineData* line, uint8_t code);
void mnemonic_single_ED(LineData* line, uint8_t code);
void mnemonic_parse(LineData* line);
void mnemonic_syntax_check(std::vector<LineData*>* lines);
