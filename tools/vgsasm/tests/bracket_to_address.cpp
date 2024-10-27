/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/mnemonic.hpp"
#include "../src/bracket.hpp"

void test_normal(const char* text, int n, ...)
{
    auto line = new LineData("", -1, text);
    mnemonic_parse(line);
    bracket_to_address(line);
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
        test_normal("LD (X), Y", 6, TokenType::Mnemonic, "LD", TokenType::AddressBegin, "(", TokenType::Other, "X", TokenType::AddressEnd, ")", TokenType::Split, ",", TokenType::Other, "Y");
        test_normal("LD (X+Z), Y", 8, TokenType::Mnemonic, "LD", TokenType::AddressBegin, "(", TokenType::Other, "X", TokenType::Plus, "+", TokenType::Other, "Z", TokenType::AddressEnd, ")", TokenType::Split, ",", TokenType::Other, "Y");
        test_normal("LD (X)+Z, Y", 8, TokenType::Mnemonic, "LD", TokenType::BracketBegin, "(", TokenType::Other, "X", TokenType::BracketEnd, ")", TokenType::Plus, "+", TokenType::Other, "Z", TokenType::Split, ",", TokenType::Other, "Y");
        test_normal("LD Z+(X), Y", 8, TokenType::Mnemonic, "LD", TokenType::Other, "Z", TokenType::Plus, "+", TokenType::BracketBegin, "(", TokenType::Other, "X", TokenType::BracketEnd, ")", TokenType::Split, ",", TokenType::Other, "Y");
        test_normal("LD X, (Y)", 6, TokenType::Mnemonic, "LD", TokenType::Other, "X", TokenType::Split, ",", TokenType::AddressBegin, "(", TokenType::Other, "Y", TokenType::AddressEnd, ")");
        test_normal("LD X, (Y+Z)", 8, TokenType::Mnemonic, "LD", TokenType::Other, "X", TokenType::Split, ",", TokenType::AddressBegin, "(", TokenType::Other, "Y", TokenType::Plus, "+", TokenType::Other, "Z", TokenType::AddressEnd, ")");
        test_normal("LD X, (Y)+Z", 8, TokenType::Mnemonic, "LD", TokenType::Other, "X", TokenType::Split, ",", TokenType::BracketBegin, "(", TokenType::Other, "Y", TokenType::BracketEnd, ")", TokenType::Plus, "+", TokenType::Other, "Z");
        test_normal("LD X, Z+(Y)", 8, TokenType::Mnemonic, "LD", TokenType::Other, "X", TokenType::Split, ",", TokenType::Other, "Z", TokenType::Plus, "+", TokenType::BracketBegin, "(", TokenType::Other, "Y", TokenType::BracketEnd, ")");
        test_normal("()", 2, TokenType::BracketBegin, "(", TokenType::BracketEnd, ")");
        test_normal("()()", 4, TokenType::BracketBegin, "(", TokenType::BracketEnd, ")", TokenType::BracketBegin, "(", TokenType::BracketEnd, ")");
        test_normal("(())", 4, TokenType::BracketBegin, "(", TokenType::BracketBegin, "(", TokenType::BracketEnd, ")", TokenType::BracketEnd, ")");
        test_normal("LD ((X)), Y", 8, TokenType::Mnemonic, "LD", TokenType::AddressBegin, "(", TokenType::BracketBegin, "(", TokenType::Other, "X", TokenType::BracketEnd, ")", TokenType::AddressEnd, ")", TokenType::Split, ",", TokenType::Other, "Y");
    } catch (...) {
        return -1;
    }
    return 0;
}
