/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

bool checkNameTable(std::string name)
{
    if (mnemonicTable.find(name) != mnemonicTable.end() ||
        operandTable.find(name) != operandTable.end() ||
        name == "SIZEOF" ||
        name == "OFFSET" ||
        nameTable.find(name) != nameTable.end()) {
        return true;
    } else {
        return false;
    }
}

void addNameTable(std::string name, LineData* line)
{
    if (checkNameTable(name)) {
        line->error = true;
        line->errmsg = "Reserved symbol name " + name + " was specified.";
    } else {
        nameTable[name] = line;
    }
}
