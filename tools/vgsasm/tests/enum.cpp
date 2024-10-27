/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/define.hpp"
#include "../src/enum.hpp"

static std::vector<LineData*> lines;
static int lineNumber = 0;
extern LineData* lastScopeBegin;
extern int scopeCount;

void add_line(const char* text)
{
    auto line = new LineData("", ++lineNumber, text);
    lines.push_back(line);
}

void clear()
{
    nameTable.clear();
    defineTable.clear();
    lines.clear();
    lineNumber = 0;
    lastScopeBegin = nullptr;
    scopeCount = 0;
}

void test_normal(const char* text, int n, ...)
{
    enum_extract(&lines);
    for (auto l : lines) {
        if (l->error) {
            printf("N: %-12s ---> ", text);
            puts(("Unexpected error: " + l->errmsg).c_str());
            throw -1;
        }
        l->printDebug();
    }
    auto line = new LineData("", -1, text);
    define_replace(line);
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

void test_error(const char* errmsg)
{
    printf("E: ---> ");
    enum_extract(&lines);
    // for (auto l : lines) { l->printDebug(); }
    for (auto l : lines) {
        if (l->error) {
            if (l->errmsg != errmsg) {
                puts(("Unexpected error: " + l->errmsg).c_str());
                printf("%s: ", std::to_string(l->lineNumber).c_str());
                l->printDebug();
                for (auto ll : lines) {
                    ll->printDebug();
                }
                throw -1;
            } else {
                puts(l->errmsg.c_str());
                return;
            }
        }
    }
    puts("Error expect bad not...");
    throw -1;
}

int main()
{
    try {
        {
            clear();
            add_line("enum Foo {");
            add_line("hoge");
            add_line("hige");
            add_line("hage");
            add_line("}");
            test_normal("LD A, foo.hoge", 4, TokenType::Other, "LD", TokenType::Other, "A", TokenType::Split, ",", TokenType::Numeric, "0");
            test_normal("LD A, foo.hige", 4, TokenType::Other, "LD", TokenType::Other, "A", TokenType::Split, ",", TokenType::Numeric, "1");
            test_normal("LD A, foo.hage", 4, TokenType::Other, "LD", TokenType::Other, "A", TokenType::Split, ",", TokenType::Numeric, "2");
        }
        {
            clear();
            add_line("enum Foo");
            add_line("{");
            add_line("hoge = 10");
            add_line("hige");
            add_line("hage = 100");
            add_line("}");
            test_normal("LD A, foo.hoge", 4, TokenType::Other, "LD", TokenType::Other, "A", TokenType::Split, ",", TokenType::Numeric, "10");
            test_normal("LD A, foo.hige", 4, TokenType::Other, "LD", TokenType::Other, "A", TokenType::Split, ",", TokenType::Numeric, "11");
            test_normal("LD A, foo.hage", 4, TokenType::Other, "LD", TokenType::Other, "A", TokenType::Split, ",", TokenType::Numeric, "100");
        }
        {
            clear();
            add_line("enum Fuga {");
            add_line("hoge");
            add_line("hige");
            add_line("hage");
            add_line("}");
            add_line("enum Foo");
            add_line("{");
            add_line("hoge = 10");
            add_line("hige");
            add_line("hage = 100");
            add_line("}");
            test_normal("LD A, fuga.hoge", 4, TokenType::Other, "LD", TokenType::Other, "A", TokenType::Split, ",", TokenType::Numeric, "0");
            test_normal("LD A, fuga.hige", 4, TokenType::Other, "LD", TokenType::Other, "A", TokenType::Split, ",", TokenType::Numeric, "1");
            test_normal("LD A, fuga.hage", 4, TokenType::Other, "LD", TokenType::Other, "A", TokenType::Split, ",", TokenType::Numeric, "2");
            test_normal("LD A, foo.hoge", 4, TokenType::Other, "LD", TokenType::Other, "A", TokenType::Split, ",", TokenType::Numeric, "10");
            test_normal("LD A, foo.hige", 4, TokenType::Other, "LD", TokenType::Other, "A", TokenType::Split, ",", TokenType::Numeric, "11");
            test_normal("LD A, foo.hage", 4, TokenType::Other, "LD", TokenType::Other, "A", TokenType::Split, ",", TokenType::Numeric, "100");
        }
        {
            clear();
            add_line("foo enum");
            test_error("`enum` must be defined at the beginning of the line.");
        }
        {
            clear();
            add_line("enum");
            test_error("No enum name specified.");
        }
        {
            clear();
            add_line("enum ,");
            test_error("Invalid enum name: ,");
        }
        {
            clear();
            add_line("enum Fuga");
            test_error("Scope is not defined: FUGA");
        }
        {
            clear();
            add_line("enum Fuga");
            add_line("{");
            test_error("Scope was not closed.");
        }
        {
            clear();
            add_line("enum Fuga {");
            test_error("Scope was not closed.");
        }
        {
            clear();
            add_line("enum Fuga { hoge");
            test_error("Unexpected symbol: HOGE");
        }
        {
            clear();
            add_line("enum Fuga {}");
            test_error("Unexpected symbol: }");
        }
        {
            clear();
            add_line("enum Fuga {");
            add_line("} foo");
            test_error("Unexpected symbol: FOO");
        }
        {
            clear();
            add_line("enum Fuga {");
            add_line("hoge hoge");
            add_line("}");
            test_error("Illegal enum symbol number: 2");
        }
        {
            clear();
            add_line("enum Fuga {");
            add_line("hoge hoge hoge hoge");
            add_line("}");
            test_error("Illegal enum symbol number: 4");
        }
        {
            clear();
            add_line("enum Fuga {");
            add_line("hoge hige fuga");
            add_line("}");
            test_error("Unexpected symbol: HIGE");
        }
        {
            clear();
            add_line("enum Fuga {");
            add_line("hoge = fuga");
            add_line("}");
            test_error("Unexpected symbol: FUGA");
        }
    } catch (...) {
        return -1;
    }
    return 0;
}
