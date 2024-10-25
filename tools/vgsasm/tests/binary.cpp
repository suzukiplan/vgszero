/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.hpp"
#include "../src/numeric.hpp"
#include "../src/binary.hpp"

void test_normal(const char* text, int size, uint8_t firstByte, int n, ...)
{
    auto line = new LineData("", -1, text);
    parse_numeric(line);
    parse_binary(line);
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
    if (size != line->machine.size()) {
        printf("Unexpected file size: %d, %d\n", (int)line->machine.size(), size);
        throw -1;
    }
    if (0 < size && firstByte != line->machine[0]) {
        printf("Unexpected first byte: %d, %d\n", (int)line->machine[0], (int)firstByte);
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
    parse_numeric(line);
    parse_binary(line);
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
    uint8_t buf[4096];
    for (int i = 0; i < sizeof(buf); i++) { buf[i] = i & 0xFF; }
    FILE* fp = fopen("data.bin", "wb");
    fwrite(buf, 1, sizeof(buf), fp);
    fclose(fp);

    try {
        test_normal("#binary \"data.bin\"", 4096, 0, 2, TokenType::Binary, "data.bin", TokenType::Delete, "data.bin");
        test_normal("#binary \"data.bin\", 0", 4096, 0, 4, TokenType::Binary, "data.bin", TokenType::Delete, "data.bin", TokenType::Delete, ",", TokenType::Delete, "0");
        test_normal("#binary \"data.bin\", 96", 4000, 96, 4, TokenType::Binary, "data.bin", TokenType::Delete, "data.bin", TokenType::Delete, ",", TokenType::Delete, "96");
        test_normal("#binary \"data.bin\", 4000", 96, 4000 & 0xFF, 4, TokenType::Binary, "data.bin", TokenType::Delete, "data.bin", TokenType::Delete, ",", TokenType::Delete, "4000");
        test_normal("#binary \"data.bin\", 4096", 0, 0, 4, TokenType::Binary, "data.bin", TokenType::Delete, "data.bin", TokenType::Delete, ",", TokenType::Delete, "4096");
        test_error("#binary \"data.bin\", 4097", "#binary specified file size is smaller than the offset value.");
        test_error("#binary \"data.bin\", -1", "Unexpected token: -");
        test_error("#binary \"data.bin\",", "#binary offset value is not specified.");
        test_normal("#binary \"data.bin\", 0, 100", 100, 0, 6, TokenType::Binary, "data.bin", TokenType::Delete, "data.bin", TokenType::Delete, ",", TokenType::Delete, "0", TokenType::Delete, ",", TokenType::Delete, "100");
        test_normal("#binary \"data.bin\", 100, 100", 100, 100, 6, TokenType::Binary, "data.bin", TokenType::Delete, "data.bin", TokenType::Delete, ",", TokenType::Delete, "100", TokenType::Delete, ",", TokenType::Delete, "100");
        test_normal("#binary \"data.bin\", 96, 4000", 4000, 96, 6, TokenType::Binary, "data.bin", TokenType::Delete, "data.bin", TokenType::Delete, ",", TokenType::Delete, "96", TokenType::Delete, ",", TokenType::Delete, "4000");
        test_error("#binary \"data.bin\", 96, 4001", "Cannot read 4001 bytes.");
        test_error("#binary \"data.bin\", 0, -1", "Unexpected token: -");
        test_error("#binary \"data.bin\", 0,", "#binary size value is not specified.");
        test_error("#binary", "#binary syntax error.");
        test_error("#binary data.bin", "#binary syntax error.");
        test_error("hoge #binary \"data.bin\"", "#binary must appear at the beginning of the line.");
        test_error("#binary \"file_not_found\"", "#binary file open error: file_not_found");
    } catch (...) {
        return -1;
    }
    return 0;
}
