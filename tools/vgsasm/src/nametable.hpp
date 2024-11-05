/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

std::string nametable_check(std::string name)
{
    if (!isalpha(name.c_str()[0])) {
        return "The first letter is not an alphabetic letter: " + name;
    } else if (nameTable.find(name) != nameTable.end()) {
        return "Reserved symbol name " + name + " was specified.";
    } else {
        return "";
    }
}

void nametable_add(std::string name, LineData* line)
{
    auto errmsg = nametable_check(name);
    if (!errmsg.empty()) {
        if (line) {
            line->error = true;
            line->errmsg = errmsg;
        }
    } else {
        nameTable[name] = line;
    }
}

void nametable_init()
{
    for (auto mnemonic : mnemonicTable) {
        nametable_add(mnemonic.first, nullptr);
    }
    for (auto operand : operandTable) {
        nametable_add(operand.first, nullptr);
    }
    nametable_add("SIZEOF", nullptr);
    nametable_add("OFFSET", nullptr);
    nametable_add("STRUCT", nullptr);
    nametable_add("ENUM", nullptr);
    nametable_add("DS.B", nullptr);
    nametable_add("DS.W", nullptr);
    nametable_add("ORG", nullptr);
}
