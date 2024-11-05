/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/numeric.hpp"
#include "../src/bracket.hpp"

void test_normal(const char* text, int n, ...)
{
    auto line = new LineData("", -1, text);
    numeric_parse(line);
    bracket_eliminate(line);
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

int main()
{
    try {
        test_normal("(123)", 1, TokenType::Numeric, "123");
        test_normal("(0x123)", 1, TokenType::Numeric, "291");
        test_normal("($123)", 1, TokenType::Numeric, "291");
        test_normal("(0b1010)", 1, TokenType::Numeric, "10");
        test_normal("(%1010)", 1, TokenType::Numeric, "10");
        test_normal("(X)", 3, TokenType::BracketBegin, "(", TokenType::Other, "X", TokenType::BracketEnd, ")");
        test_normal("(1)(2)(3)", 3, TokenType::Numeric, "1", TokenType::Numeric, "2", TokenType::Numeric, "3");
        test_normal("(1)(Z)(3)", 5, TokenType::Numeric, "1", TokenType::BracketBegin, "(", TokenType::Other, "Z", TokenType::BracketEnd, ")", TokenType::Numeric, "3");
        test_normal("(((1)))", 1, TokenType::Numeric, "1");
        test_normal("(1+2)", 5, TokenType::BracketBegin, "(", TokenType::Numeric, "1", TokenType::Plus, "+", TokenType::Numeric, "2", TokenType::BracketEnd, ")");
        test_normal("((1)+((2)))", 5, TokenType::BracketBegin, "(", TokenType::Numeric, "1", TokenType::Plus, "+", TokenType::Numeric, "2", TokenType::BracketEnd, ")");
    } catch (...) {
        return -1;
    }
    return 0;
}
