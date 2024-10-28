/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/numeric.hpp"
#include "../src/clear.hpp"

void test_normal(const char* text, int n, ...)
{
    auto line = new LineData("", -1, text);
    numeric_parse(line);
    numeric_parse_minus(line);
    numeric_parse_plus(line);
    std::vector<LineData*> lines;
    lines.push_back(line);
    clear_delete_token(&lines);
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
    numeric_parse(line);
    numeric_parse_minus(line);
    numeric_parse_plus(line);
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
        test_normal("1,2,3", 5, TokenType::Numeric, "1", TokenType::Split, ",", TokenType::Numeric, "2", TokenType::Split, ",", TokenType::Numeric, "3");
        test_normal("0+1,+2,+3", 7, TokenType::Numeric, "0", TokenType::Plus, "+", TokenType::Numeric, "1", TokenType::Split, ",", TokenType::Numeric, "2", TokenType::Split, ",", TokenType::Numeric, "3");
        test_normal("0-1,-2,-3", 7, TokenType::Numeric, "0", TokenType::Minus, "-", TokenType::Numeric, "1", TokenType::Split, ",", TokenType::Numeric, "-2", TokenType::Split, ",", TokenType::Numeric, "-3");
        test_normal("$00ff", 1, TokenType::Numeric, "255");
        test_normal("0xffff", 1, TokenType::Numeric, "65535");
        test_error("$00fg", "Invalid hexadecimal number: $00FG");
        test_normal("%10101", 1, TokenType::Numeric, "21");
        test_normal("0b01010", 1, TokenType::Numeric, "10");
        test_error("%0102", "Invalid binary number: %0102");
    } catch (...) {
        return -1;
    }
    return 0;
}
