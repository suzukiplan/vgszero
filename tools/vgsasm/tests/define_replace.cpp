/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/define.hpp"
#include "../src/nametable.hpp"

void add_line(const char* text)
{
    auto line = new LineData("", -1, text);
    define_parse(line);
}

void test_normal(const char* text, int n, ...)
{
    auto line = new LineData("", -1, text);
    define_replace(line);
    printf("N: %-12s ---> ", text);
    line->printDebug();
    if (line->error) {
        puts(("Unexpected error: " + line->errmsg).c_str());
        throw -1;
    }
    if (n != line->token.size()) {
        printf("Unexpected token number: %d, %d\n", n, (int)line->token.size());
        throw -1;
    }
    va_list arg;
    va_start(arg, n);
    for (int i = 0; i < n; i++) {
        auto first = va_arg(arg, TokenType);
        auto second = va_arg(arg, const char*);
        if (line->token[i].first != first) {
            printf("Unexpected TokenType #%d\n", i + 1);
            throw -1;
        }
        if (line->token[i].second != second) {
            printf("Unexpected Token#%d: %s != %s\n", i + 1, line->token[i].second.c_str(), second);
            throw -1;
        }
    }
}

void test_error(const char* text, const char* errmsg)
{
    auto line = new LineData("", -1, text);
    define_replace(line);
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
        add_line("#define A AAA");
        test_normal("A B C", 3, TokenType::Other, "AAA", TokenType::Other, "B", TokenType::Other, "C");
        add_line("#define B BB BB");
        test_normal("A B C", 4, TokenType::Other, "AAA", TokenType::Other, "BB", TokenType::Other, "BB", TokenType::Other, "C");
        test_normal("X B C", 4, TokenType::Other, "X", TokenType::Other, "BB", TokenType::Other, "BB", TokenType::Other, "C");
        add_line("#define C A");
        test_error("A B C", "Detect circular references in #define definition names.");
        add_line("#define D.A FOO");
        test_normal("D.A", 1, TokenType::Other, "FOO");
        test_error("D.B", "`B` was not defined at `D`");
    } catch (...) {
        return -1;
    }
    return 0;
}
