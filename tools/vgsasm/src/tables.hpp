/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

std::map<std::string, LineData*> nameTable;
std::vector<std::string> includeFiles;
std::map<std::string, std::vector<std::pair<TokenType, std::string>>> defineTable;
std::map<std::string, LineData*> labelTable;
std::map<std::string, Struct*> structTable;
