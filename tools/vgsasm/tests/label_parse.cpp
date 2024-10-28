/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/label.hpp"

void test_normal(const char* text, int n, ...)
{
    auto line = new LineData("", -1, text);
    auto newLine = label_parse(line);
    printf("N: %-12s ---> ", text);
    line->printDebug();
    if (newLine) {
        printf("               ");
        newLine->printDebug();
    }
    if (line->error) {
        puts(("Unexpected error: " + line->errmsg).c_str());
        throw -1;
    }
    if (n != line->token.size() + (newLine ? newLine->token.size() : 0)) {
        printf("Unexpected token number: %d, %d\n", n, (int)line->token.size());
        throw -1;
    }
    va_list arg;
    va_start(arg, n);
    for (int i = 0, ii = 0; i < n; i++, ii++) {
        auto first = va_arg(arg, TokenType);
        auto second = va_arg(arg, const char*);
        if (line->token.begin() + ii == line->token.end()) {
            ii = 0;
            line = newLine;
        }
        if (line->token[ii].first != first) {
            printf("Unexpected TokenType #%d\n", i + 1);
            throw -1;
        }
        if (line->token[ii].second != second) {
            printf("Unexpected Token#%d: %s != %s\n", i + 1, line->token[i].second.c_str(), second);
            throw -1;
        }
    }
}

void test_error(const char* text, const char* errmsg)
{
    auto line = new LineData("", -1, text);
    label_parse(line);
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
        test_normal(".label1", 1, TokenType::Label, "LABEL1");
        test_normal("@label", 1, TokenType::LabelAt, "LABEL@LABEL1");
        test_normal("label2:", 1, TokenType::Label, "LABEL2");
        test_normal("@label", 1, TokenType::LabelAt, "LABEL@LABEL2");
        test_error("label1:", "Duplicate label: LABEL1");
        test_error("0label1:", "The first letter is not an alphabetic letter: 0LABEL1");
        test_normal("label3: FOO", 2, TokenType::Label, "LABEL3", TokenType::Other, "FOO");
    } catch (...) {
        return -1;
    }
    return 0;
}
