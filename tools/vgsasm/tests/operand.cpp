/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/operand.hpp"

void test_normal(const char* text, int n, ...)
{
    auto line = new LineData("", -1, text);
    operand_parse(line);
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
        test_normal("LD (IX+10), A", 8,
                    TokenType::Other, "LD",
                    TokenType::BracketBegin, "(",
                    TokenType::Operand, "IX",
                    TokenType::Plus, "+",
                    TokenType::Other, "10",
                    TokenType::BracketEnd, ")",
                    TokenType::Split, ",",
                    TokenType::Operand, "A");
    } catch (...) {
        return -1;
    }
    return 0;
}
