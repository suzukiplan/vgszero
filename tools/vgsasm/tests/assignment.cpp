/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/assignment.hpp"

void test_normal(const char* text, int n, ...)
{
    auto line = new LineData("", -1, text);
    replace_assignment(line);
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
    replace_assignment(line);
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
        test_normal("A = B", 4, TokenType::Other, "LD", TokenType::Other, "A", TokenType::Split, ",", TokenType::Other, "B");
        test_normal("A += B", 4, TokenType::Other, "ADD", TokenType::Other, "A", TokenType::Split, ",", TokenType::Other, "B");
        test_normal("A -= B", 4, TokenType::Other, "SUB", TokenType::Other, "A", TokenType::Split, ",", TokenType::Other, "B");
        test_normal("A *= B", 4, TokenType::Other, "MUL", TokenType::Other, "A", TokenType::Split, ",", TokenType::Other, "B");
        test_normal("A /= B", 4, TokenType::Other, "DIV", TokenType::Other, "A", TokenType::Split, ",", TokenType::Other, "B");
        test_normal("A %= B", 4, TokenType::Other, "MOD", TokenType::Other, "A", TokenType::Split, ",", TokenType::Other, "B");
        test_normal("A &= B", 4, TokenType::Other, "AND", TokenType::Other, "A", TokenType::Split, ",", TokenType::Other, "B");
        test_normal("A |= B", 4, TokenType::Other, "OR", TokenType::Other, "A", TokenType::Split, ",", TokenType::Other, "B");
        test_normal("A ^= B", 4, TokenType::Other, "XOR", TokenType::Other, "A", TokenType::Split, ",", TokenType::Other, "B");
        test_normal("A <<= B", 4, TokenType::Other, "SL", TokenType::Other, "A", TokenType::Split, ",", TokenType::Other, "B");
        test_normal("A >>= B", 4, TokenType::Other, "SR", TokenType::Other, "A", TokenType::Split, ",", TokenType::Other, "B");
        test_error("A =", "Illegal assignment expression without left or right side.");
        test_error("= B", "Illegal assignment expression without left or right side.");
        test_error("=", "Illegal assignment expression without left or right side.");
        test_error("A = B += C", "Multiple assignments are described.");
        test_normal("AB", 1, TokenType::Other, "AB");
    } catch (...) {
        return -1;
    }
    return 0;
}
