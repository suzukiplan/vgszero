/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/decimal.hpp"

void test(std::string (*func)(const char*), const char* input, const char* expect)
{
    if (func == oct2dec) { printf("oct2dec: "); }
    if (func == hex2dec) { printf("hex2dec: "); }
    if (func == bin2dec) { printf("bin2dec: "); }
    auto result = func(input);
    printf("%-12s ---> %s ", input, result.empty() ? "(empty)" : result.c_str());
    if (result == expect) {
        puts("OK");
    } else {
        puts("Unexpected!");
        throw -1;
    }
}

int main()
{
    try {
        test(oct2dec, "", "");
        test(oct2dec, "1", "1");
        test(oct2dec, "123456", "42798");
        test(oct2dec, "1234567", "342391");
        test(oct2dec, "12345678", "");
        test(oct2dec, "1a", "");
        test(oct2dec, "a1", "");
        test(hex2dec, "", "");
        test(hex2dec, "1", "1");
        test(hex2dec, "ffff", "");
        test(hex2dec, "FFFF", "65535");
        test(hex2dec, "EF", "239");
        test(hex2dec, "FG", "");
        test(bin2dec, "", "");
        test(bin2dec, "10101", "21");
        test(bin2dec, "01010", "10");
        test(bin2dec, "1", "1");
        test(bin2dec, "0", "0");
        test(bin2dec, "12", "");
    } catch (...) {
        return -1;
    }

    return 0;
}