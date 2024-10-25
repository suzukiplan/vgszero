/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

std::string oct2dec(const char* oct)
{
    int result = 0;
    int len = 0;
    while (*oct) {
        if (isdigit(*oct) && *oct < '8') {
            result <<= 3;
            result += (*oct) - '0';
            len++;
        } else {
            return "";
        }
        oct++;
    }
    return 0 < len ? std::to_string(result) : "";
}

std::string hex2dec(const char* hex)
{
    int result = 0;
    int len = 0;
    while (*hex) {
        if (isdigit(*hex)) {
            result <<= 4;
            result += (*hex) - '0';
            len++;
        } else if ('A' <= *hex && *hex <= 'F') {
            result <<= 4;
            result += (*hex) - 'A' + 10;
            len++;
        } else {
            return "";
        }
        hex++;
    }
    return 0 < len ? std::to_string(result) : "";
}

std::string bin2dec(const char* bin)
{
    int result = 0;
    int len = 0;
    while (*bin) {
        if ('0' == *bin || '1' == *bin) {
            result <<= 1;
            result += (*bin) - '0';
            len++;
        } else {
            return "";
        }
        bin++;
    }
    return 0 < len ? std::to_string(result) : "";
}
