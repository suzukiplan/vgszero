/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/clear.hpp"
#include "../src/operand.hpp"
#include "../src/increment.hpp"

std::vector<LineData*> lines;

void test_normal(const char* text, int n, ...)
{
    lines.clear();
    lines.push_back(new LineData("", -1, text));
    operand_parse(lines[0]);
    increment_split(&lines);
    clear_delete_token(&lines);
    printf("N: %-12s ---> ", text);
    int tokenSize = 0;
    bool first = true;
    for (auto line : lines) {
        if (!first) {
            printf("               ");
        }
        first = false;
        line->printDebug();
        if (line->error) {
            puts(("Unexpected error: " + line->errmsg).c_str());
            throw -1;
        }
        tokenSize += (int)line->token.size();
    }

    if (n != tokenSize) {
        printf("Unexpected token number: %d, %d\n", n, tokenSize);
        throw -1;
    }
    va_list arg;
    va_start(arg, n);
    auto line = lines.begin();
    for (int i = 0, ii = 0; i < n; i++, ii++) {
        if ((*line)->token.size() == ii) {
            ii = 0;
            line++;
        }
        auto first = va_arg(arg, TokenType);
        auto second = va_arg(arg, const char*);
        if ((*line)->token[ii].first != first) {
            printf("Unexpected TokenType #%d\n", i + 1);
            throw -1;
        }
        if ((*line)->token[ii].second != second) {
            printf("Unexpected Token#%d: %s != %s\n", i + 1, (*line)->token[i].second.c_str(), second);
            throw -1;
        }
    }
}

void test_error(const char* text, const char* errmsg)
{
    lines.clear();
    lines.push_back(new LineData("", -1, text));
    operand_parse(lines[0]);
    increment_split(&lines);
    printf("E: %-12s ---> ", text);
    for (auto line : lines) {
        if (line->error) {
            if (line->errmsg != errmsg) {
                puts(("Unexpected error: " + line->errmsg).c_str());
                throw -1;
            } else {
                printf(" %s\n", errmsg);
                return;
            }
        }
    }
    puts("Error not found!");
    throw -1;
}

int main()
{
    try {
        test_normal("LD (HL++), A", 8, TokenType::Other, "LD", TokenType::BracketBegin, "(", TokenType::Operand, "HL", TokenType::BracketEnd, ")", TokenType::Split, ",", TokenType::Operand, "A", TokenType::Mnemonic, "INC", TokenType::Operand, "HL");
        test_normal("LD (++HL), A", 8, TokenType::Mnemonic, "INC", TokenType::Operand, "HL", TokenType::Other, "LD", TokenType::BracketBegin, "(", TokenType::Operand, "HL", TokenType::BracketEnd, ")", TokenType::Split, ",", TokenType::Operand, "A");
        test_normal("LD (HL--), A", 8, TokenType::Other, "LD", TokenType::BracketBegin, "(", TokenType::Operand, "HL", TokenType::BracketEnd, ")", TokenType::Split, ",", TokenType::Operand, "A", TokenType::Mnemonic, "DEC", TokenType::Operand, "HL");
        test_normal("LD (--HL), A", 8, TokenType::Mnemonic, "DEC", TokenType::Operand, "HL", TokenType::Other, "LD", TokenType::BracketBegin, "(", TokenType::Operand, "HL", TokenType::BracketEnd, ")", TokenType::Split, ",", TokenType::Operand, "A");
        test_normal("LD (HL++), A++", 10, TokenType::Other, "LD", TokenType::BracketBegin, "(", TokenType::Operand, "HL", TokenType::BracketEnd, ")", TokenType::Split, ",", TokenType::Operand, "A", TokenType::Mnemonic, "INC", TokenType::Operand, "A", TokenType::Mnemonic, "INC", TokenType::Operand, "HL");
        test_normal("LD (++HL), ++A", 10, TokenType::Mnemonic, "INC", TokenType::Operand, "HL", TokenType::Mnemonic, "INC", TokenType::Operand, "A", TokenType::Other, "LD", TokenType::BracketBegin, "(", TokenType::Operand, "HL", TokenType::BracketEnd, ")", TokenType::Split, ",", TokenType::Operand, "A");
        test_normal("LD (++HL), A++", 10, TokenType::Mnemonic, "INC", TokenType::Operand, "HL", TokenType::Other, "LD", TokenType::BracketBegin, "(", TokenType::Operand, "HL", TokenType::BracketEnd, ")", TokenType::Split, ",", TokenType::Operand, "A", TokenType::Mnemonic, "INC", TokenType::Operand, "A");
        test_normal("LD (HL), ++A--", 10, TokenType::Mnemonic, "INC", TokenType::Operand, "A", TokenType::Other, "LD", TokenType::BracketBegin, "(", TokenType::Operand, "HL", TokenType::BracketEnd, ")", TokenType::Split, ",", TokenType::Operand, "A", TokenType::Mnemonic, "DEC", TokenType::Operand, "A");
        test_error("LD++", "`++` or `--` can only be specified before or after the register.");
        test_error("++", "`++` or `--` can only be specified before or after the register.");
        test_error("++--", "`++` or `--` can only be specified before or after the register.");
        test_error("IYH++", "`++` or `--` cannot be specified in the IYH or IYL registers.");
        test_error("IYL++", "`++` or `--` cannot be specified in the IYH or IYL registers.");
        test_error("A++ B--", "Illegal `++` or `--` sequence.");
    } catch (...) {
        return -1;
    }
    return 0;
}
