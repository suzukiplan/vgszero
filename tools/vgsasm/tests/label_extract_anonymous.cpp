/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/label.hpp"

std::vector<LineData*> lines;
int lineNumber;

void clear()
{
    nameTable.clear();
    labelTable.clear();
    lines.clear();
    lineNumber = 0;
}

void add_line(const char* text)
{
    auto line = new LineData("", -1, text);
    label_parse(line);
    label_parse_jump(line);
    lines.push_back(line);
}

void test_normal(int checkLineNumber, int n, ...)
{
    label_extract_anonymous(&lines);
    int ln = 1;
    LineData* line = nullptr;
    for (auto l : lines) {
        if (1 == ln) {
            printf("N: [%c] ", ln == checkLineNumber ? 'x' : ' ');
        } else {
            printf("   [%c] ", ln == checkLineNumber ? 'x' : ' ');
        }
        if (ln == checkLineNumber) {
            line = l;
        }
        l->printDebug();
        if (l->error) {
            puts(("Unexpected error: " + l->errmsg).c_str());
            throw -1;
        }
        ln++;
    }
    if (!line) {
        puts("Check line not exist!");
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
    label_extract_anonymous(&lines);
    int ln = 1;
    for (auto line : lines) {
        if (1 == ln) {
            printf("E: [%c] ", line->error ? 'x' : ' ');
        } else {
            printf("   [%c] ", line->error ? 'x' : ' ');
        }
        ln++;
        line->printDebug();
    }
    for (auto line : lines) {
        if (line->error) {
            if (line->errmsg != errmsg) {
                puts(("Unexpected error: " + line->errmsg).c_str());
                throw -1;
            } else {
                puts(errmsg);
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
        clear();
        add_line("JR @+0");
        test_normal(2, 4, TokenType::Other, "JR", TokenType::Delete, "@", TokenType::Delete, "+", TokenType::LabelJump, "$@+0#0");

        clear();
        add_line("NOP");
        add_line("JR @-1");
        add_line("NOP");
        test_normal(3, 4, TokenType::Other, "JR", TokenType::Delete, "@", TokenType::Delete, "-", TokenType::LabelJump, "$@-1#0");

        clear();
        add_line("NOP");
        add_line("JR @+1");
        add_line("NOP");
        test_normal(2, 4, TokenType::Other, "JR", TokenType::Delete, "@", TokenType::Delete, "+", TokenType::LabelJump, "$@+1#0");

        clear();
        add_line("NOP");
        add_line("JR @+1");
        add_line("LABEL:");
        test_error("Anonymous labels cannot cross another label: LABEL");

        clear();
        add_line("NOP");
        add_line(".LABEL_UP");
        add_line("JR @-1");
        add_line(".LABEL_DOWN");
        add_line("NOP");
        test_error("Anonymous labels cannot cross another label: LABEL_UP");

        clear();
        add_line("JR @");
        test_error("Anonymous label syntax error. (no token)");

        clear();
        add_line("JR @*");
        test_error("Anonymous label syntax error. (invalid token: *)");

        clear();
        add_line("JR @+");
        test_error("Anonymous label syntax error. (no numeric specified)");

        clear();
        add_line("JR @+A");
        test_error("Unexpected symbol specified: A");

        clear();
        add_line("JR @+0 B");
        test_error("Unexpected symbol specified: B");

        clear();
        add_line("JR @+2");
        add_line("struct foo $C000");
        add_line("{");
        add_line("}");
        test_error("Anonymous labels cannot cross scopes.");

        clear();
        add_line("JR @+2");
        add_line("#binary");
        add_line("{");
        add_line("}");
        test_error("Anonymous labels cannot cross #BINARY.");
    } catch (...) {
        return -1;
    }
    return 0;
}
