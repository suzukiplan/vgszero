/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/define.hpp"
#include "../src/nametable.hpp"

void test_normal(const char* text, std::string name, int n, ...)
{
    auto line = new LineData("", -1, text);
    define_parse(line);
    printf("N: %-12s ---> ", text);
    line->printDebug();
    if (line->error) {
        puts(("Unexpected error: " + line->errmsg).c_str());
        throw -1;
    }
    for (auto t : line->token) {
        if (t.first != TokenType::Delete) {
            puts(("Token not deleted: " + t.second).c_str());
            throw -1;
        }
    }
    if (defineTable.end() == defineTable.find(name)) {
        puts(("Not defined: " + name).c_str());
        throw -1;
    }
    auto def = defineTable[name];
    if (n != def.size()) {
        printf("Unexpected define number: %d, %d\n", n, (int)def.size());
        throw -1;
    }
    va_list arg;
    va_start(arg, n);
    for (int i = 0; i < n; i++) {
        auto first = va_arg(arg, TokenType);
        auto second = va_arg(arg, const char*);
        if (def[i].first != first) {
            printf("Unexpected TokenType #%d\n", i + 1);
            throw -1;
        }
        if (def[i].second != second) {
            printf("Unexpected Token#%d: %s != %s\n", i + 1, line->token[i].second.c_str(), second);
            throw -1;
        }
    }
}

void test_error(const char* text, const char* errmsg)
{
    auto line = new LineData("", -1, text);
    define_parse(line);
    printf("E: %-12s ---> ", text);
    if (!line->error) {
        puts("Error expect bad not...");
        throw -1;
    }
    if (line->errmsg != errmsg) {
        puts(("Unexpected error: " + line->errmsg).c_str());
        throw -1;
    }
    printf(" %s\n", errmsg);
}

int main()
{
    try {
        test_normal("#define AA", "AA", 1, TokenType::Delete, "");
        test_normal("#define BB C", "BB", 1, TokenType::Other, "C");
        test_error("#define AA B", "Duplicate definition name AA in #define.");
        test_error("#define CC CC", "CC is included in #define CC.");
        test_error("#define", "No definition name specified in #define.");
        test_error("A #define", "#define must appear at the beginning of the line.");
        test_error("#define #DEFINE", "Multiple #defines cannot be defined on a single line.");
    } catch (...) {
        return -1;
    }
    return 0;
}
