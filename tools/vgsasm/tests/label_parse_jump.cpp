/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/label.hpp"

void add(LineData* line)
{
    label_parse(line);
    label_parse_jump(line);
}

void test_normal(const char* text, int n, ...)
{
    auto line = new LineData("", -1, text);
    label_parse_jump(line);
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
    for (int i = 0, ii = 0; i < n; i++, ii++) {
        auto first = va_arg(arg, TokenType);
        auto second = va_arg(arg, const char*);
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
    label_parse_jump(line);
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
        add(new LineData("", -1, ".label1"));
        add(new LineData("", -1, "@label"));
        add(new LineData("", -1, ".label2"));
        add(new LineData("", -1, "@label"));
        test_normal("JR LABEL1", 2, TokenType::Other, "JR", TokenType::LabelJump, "LABEL1");
        test_normal("JR LABEL2", 2, TokenType::Other, "JR", TokenType::LabelJump, "LABEL2");
        test_normal("JR @LABEL", 2, TokenType::Other, "JR", TokenType::LabelJump, "LABEL@LABEL2");
        test_normal("JR LABEL@LABEL1", 2, TokenType::Other, "JR", TokenType::LabelJump, "LABEL@LABEL1");
        test_normal("JR LABEL@LABEL2", 2, TokenType::Other, "JR", TokenType::LabelJump, "LABEL@LABEL2");
        test_error("JR @Unknown", "Unknown label specified: @UNKNOWN");
        test_error("JR LABEL@Unknown", "Label `UNKNOWN` is undefined.");
        test_error("JR UNKNOWN@LABEL1", "Label `@UNKNOWN` is undefined in `LABEL1`.");
        test_normal("JR @", 2, TokenType::Other, "JR", TokenType::Other, "@");
    } catch (...) {
        return -1;
    }
    return 0;
}
