/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/offset.hpp"
#include "../src/clear.hpp"

void test_normal(const char* text, int n, ...)
{
    auto line = new LineData("", -1, text);
    offset_parse(line);
    std::vector<LineData*> lines;
    lines.push_back(line);
    clear_delete_token(&lines);
    offset_replace(line);
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
    offset_parse(line);
    std::vector<LineData*> lines;
    lines.push_back(line);
    clear_delete_token(&lines);
    offset_replace(line);
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
    Struct dummy(nullptr, "DUMMY");
    dummy.start = 0xC000;
    dummy.size = 3;
    StructField var1(nullptr, "VAR1");
    var1.address = 0xC000;
    var1.size = 1;
    var1.count = 1;
    StructField var2(nullptr, "VAR2");
    var2.address = 0xC001;
    var2.size = 1;
    var2.count = 1;
    StructField var3(nullptr, "VAR3");
    var3.address = 0xC002;
    var3.size = 1;
    var3.count = 1;
    dummy.fields.push_back(&var1);
    dummy.fields.push_back(&var2);
    dummy.fields.push_back(&var3);
    structTable["DUMMY"] = &dummy;

    try {
        test_normal("offset(dummy.var1)", 1, TokenType::Numeric, "0");
        test_normal("offset(dummy.var2)", 1, TokenType::Numeric, "1");
        test_normal("offset(dummy.var3)", 1, TokenType::Numeric, "2");
        test_error("offset(dummyy.var3)", "Undefined structure DUMMYY is specified in offset.");
        test_error("offset(dummy.var4)", "Field name `VAR4` is not defined in structure `DUMMY`.");
    } catch (...) {
        return -1;
    }
    return 0;
}
