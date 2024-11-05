/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/nametable.hpp"

void test_normal(const char* text)
{
    auto line = new LineData("", -1, text);
    nametable_add(text, line);
    printf("N: %-12s ---> ", text);
    if (line->error) {
        puts(("Unexpected error: " + line->errmsg).c_str());
        throw -1;
    }
    puts("OK");
}

void test_error(const char* text, const char* errmsg)
{
    auto line = new LineData("", -1, text);
    nametable_add(text, line);
    printf("E: %-12s ---> ", text);
    if (line->error) {
        if (line->errmsg != errmsg) {
            puts(("Unexpected error: " + line->errmsg).c_str());
            throw -1;
        } else {
            puts(errmsg);
            return;
        }
    }
    puts("NG");
    throw -1;
}

int main()
{
    try {
        test_normal("A");
        test_error("0A", "The first letter is not an alphabetic letter: 0A");
        test_error("A", "Reserved symbol name A was specified.");
        test_normal("AA");
        test_normal("AAA");
    } catch (...) {
        return -1;
    }
    return 0;
}
