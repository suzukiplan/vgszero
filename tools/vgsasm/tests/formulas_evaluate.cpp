/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/formulas.hpp"
#include "../src/numeric.hpp"

void test_normal(const char* text, int n, ...)
{
    auto line = new LineData("", -1, text);
    numeric_parse(line);
    operand_parse(line);
    formulas_evaluate(line);
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
    operand_parse(line);
    formulas_evaluate(line);
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
        test_normal("1+2+3", 1, TokenType::Numeric, "6");
        test_normal("1+2+3*4", 1, TokenType::Numeric, "15");
        test_normal("1+2+3*4/2", 1, TokenType::Numeric, "9");
        test_normal("(1+2+3)*4/2", 1, TokenType::Numeric, "12");
        test_normal("(1+2+3)*(4/2+1)", 1, TokenType::Numeric, "18");
        test_normal("(1+(2+3))+4", 1, TokenType::Numeric, "10");
        test_normal("A+1", 3, TokenType::Operand, "A", TokenType::Plus, "+", TokenType::Numeric, "1");
        test_normal("AA+1", 3, TokenType::Other, "AA", TokenType::Plus, "+", TokenType::Numeric, "1");
        test_normal("A-1", 3, TokenType::Operand, "A", TokenType::Minus, "-", TokenType::Numeric, "1");
        test_normal("AA-1", 3, TokenType::Other, "AA", TokenType::Minus, "-", TokenType::Numeric, "1");
        test_error("A*1", "Illegal expression with no specification of the number of operations to be performed.");
        test_error("A/1", "Illegal expression with no specification of the number of operations to be performed.");
        test_error("1+A", "Illegal expression with no specification of the number of operations to be performed.");
        test_error("1*A", "Illegal expression with no specification of the number of operations to be performed.");
        test_error("1/0", "Division by zero.");
        test_error("1/(5-5)", "Division by zero.");
        test_error("3/(1*2/3)", "Division by zero.");
    } catch (...) {
        return -1;
    }
    return 0;
}
