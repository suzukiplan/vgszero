/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#include "common.h"

#include "assignment.hpp"
#include "binary.hpp"
#include "bracket.hpp"
#include "clear.hpp"
#include "decimal.hpp"
#include "define.hpp"
#include "enum.hpp"
#include "file.hpp"
#include "formulas.hpp"
#include "ifdef.hpp"
#include "increment.hpp"
#include "label.hpp"
#include "literal.hpp"
#include "macro.hpp"
#include "mnemonic.hpp"
#include "nametable.hpp"
#include "numeric.hpp"
#include "offset.hpp"
#include "operand.hpp"
#include "org.hpp"
#include "sizeof.hpp"
#include "string.hpp"
#include "struct.hpp"
#include "tables.hpp"

int errorCount = 0;
bool showLineDebug = false;

uint8_t bin[0x10000];
size_t binStart;
size_t binSize;

static int check_error(LineData* line)
{
    if (line->error) {
        printf("Error: %s (%d) %s\n", line->path.c_str(), line->lineNumber, line->errmsg.c_str());
        if (showLineDebug) { line->printDebug(); }
        return -1;
    }
    return 0;
}

static int check_error(std::vector<LineData*> lines)
{
    int ret = 0;
    for (auto line : lines) {
        ret |= check_error(line);
    }
    return ret;
}

static int assemble(std::vector<LineData*> lines)
{
    bool error = false;

    // #define にプリセット定義を展開
    define_init();

    // #define のテーブル作成
    bool searchDefine = true;
    while (searchDefine && !error) {
        searchDefine = false;
        for (auto it = lines.begin(); it != lines.end(); it++) {
            if (define_parse(*it)) {
                searchDefine = true;
                lines.erase(it);
                break;
            } else {
                error = check_error(*it) ? true : error;
            }
        }
    }
    if (error) {
        return -1;
    }

    // #ifdef
    parse_ifdef(&lines);
    if (check_error(lines)) {
        return -1;
    }
    clear_delete_token(&lines);

    // enum の展開
    enum_extract(&lines);
    if (check_error(lines)) {
        return -1;
    }
    clear_delete_token(&lines);

    // #define の展開
    for (auto it = lines.begin(); it != lines.end(); it++) {
        define_replace(*it);
        error = check_error(*it) ? true : error;
    }
    if (error) {
        return -1;
    }

    // ラベルをパース
    for (auto line = lines.begin(); line != lines.end(); line++) {
        // Other -> Label or LabelAt
        auto newLine = label_parse(*line);
        if (newLine) {
            lines.insert(line + 1, newLine);
            line = lines.begin();
        }
    }
    if (check_error(lines)) {
        return -1;
    }

    // 匿名ラベルを展開
    label_extract_anonymous(&lines);
    if (check_error(lines)) {
        return -1;
    }
    clear_delete_token(&lines);

    // 基本構文解析
    for (auto line = lines.begin(); line != lines.end(); line++) {
        assignment_replace(*line);  // X Equal* Y = {LD|ADD|SUB|AND|OR|XOR} X, Y
        mnemonic_parse(*line);      // Other -> Mnemonic
        operand_parse(*line);       // Other -> Operand
        struct_parse(*line);        // Other -> Struct
        bracket_to_address(*line);  // Braket -> Address
        numeric_parse(*line);       // Other -> Numeric
        numeric_parse_minus(*line); // Split, Minus, Numeric -> Split, -Numeric
        numeric_parse_plus(*line);  // Split, Plus, Numeric -> Split, Numeric
        sizeof_parse(*line);        // Other -> Sizeof
        offset_parse(*line);        // Other -> Offset
        binary_parse(*line);        // Other -> Binary
        macro_parse(*line);         // Other -> Macro
        error = check_error(*line) ? true : error;
    }
    if (error) {
        return -1;
    }

    // マクロ呼び出し箇所をパース
    macro_parse_caller(&lines);
    if (check_error(lines)) {
        return -1;
    }

    // インクリメント、デクリメント演算子を展開
    increment_split(&lines);
    if (check_error(lines)) {
        return -1;
    }

    // 文字列リテラルを無名ラベルの参照に変換し、末尾に無名ラベル+DBを展開
    string_literal_extract(&lines);
    if (check_error(lines)) {
        return -1;
    }

    // 演算を実行（1回目）
    for (auto line = lines.begin(); line != lines.end(); line++) {
        formulas_evaluate(*line);
        while (bracket_eliminate(*line)) {
            formulas_evaluate(*line);
        }
        error = check_error(*line) ? true : error;
    }
    if (error) {
        return -1;
    }

    // struct 構文解析
    while (struct_syntax_check(&lines)) {
        if (check_error(lines)) {
            return -1;
        }
    }
    if (check_error(lines)) {
        return -1;
    }
    int retryCount = 0;
    while (struct_check_size()) {
        if (check_error(lines)) {
            return -1;
        }
        retryCount++;
        if (structTable.size() <= retryCount) {
            // 循環参照が発生している構造体をエラーにする
            for (auto it = structTable.begin(); it != structTable.end(); it++) {
                if (0 == it->second->size) {
                    it->second->line->error = true;
                    it->second->line->errmsg = "Detects circular references between structures";
                    break;
                }
            }
        }
    }
    if (check_error(lines)) {
        return -1;
    }
    clear_delete_token(&lines);

    // 残存パース処理
    for (auto line : lines) {
        struct_parse_name(line); // 構造体トークンをパース
        struct_replace(line);    // 構造体 -> 数値
        sizeof_replace(line);    // sizeof -> 数値
        offset_replace(line);    // offset -> 数値
        label_parse_jump(line);  // Other -> LabelJump
        org_parse(line);         // Other -> org
    }
    if (check_error(lines)) {
        return -1;
    }
    clear_delete_token(&lines);

    // 展開された全ての数値計算を実行
    for (auto line : lines) {
        formulas_evaluate(line);
    }
    if (check_error(lines)) {
        return -1;
    }
    clear_delete_token(&lines);

    // 構造体配列を数値に置換
    for (auto line : lines) {
        struct_parse_array(line);
    }
    if (check_error(lines)) {
        return -1;
    }
    clear_delete_token(&lines);

    // 展開された全ての数値計算を実行
    for (auto line : lines) {
        formulas_evaluate(line);
    }
    if (check_error(lines)) {
        return -1;
    }
    clear_delete_token(&lines);

    // マクロの構文解析（マクロテーブルを作成）
    macro_syntax_check(&lines);
    if (check_error(lines)) {
        return -1;
    }
    clear_delete_token(&lines);

    // マクロ呼び出し箇所を展開
    extract_macro_call(&lines);
    if (check_error(lines)) {
        return -1;
    }

    // org があれば org の次の行のプログラムカウンタを初期化
    org_setpc(&lines);
    if (check_error(lines)) {
        return -1;
    }
    clear_delete_token(&lines);

    // ニーモニック書式チェック
    mnemonic_syntax_check(&lines);
    if (check_error(lines)) {
        return -1;
    }
    clear_delete_token(&lines);

    // この時点で Other が残っていたらエラーにする
    for (auto line = lines.begin(); line != lines.end(); line++) {
        for (auto token : (*line)->token) {
            if (token.first == TokenType::Other) {
                (*line)->error = true;
                (*line)->errmsg = "Invalid symbol: " + token.second;
            }
        }
    }
    if (check_error(lines)) {
        return -1;
    }

    // struct解析結果を出力（デバッグ）
#if 0
    for (auto s : structTable) {
        auto name = s.first.c_str();
        auto start = s.second->start;
        auto size = s.second->size;
        printf("Struct: %s (0x%X) size = %d\n", name, start, size);
        for (auto f : s.second->fields) {
            printf(" - %s (size=%d, count=%d addr=0x%X)\n", f->name.c_str(), f->size, f->count, f->address);
        }
    }
#endif

    // 解析結果を出力（デバッグ）
    if (showLineDebug) {
        for (auto line : lines) {
            printf("%16s:%04d", line->path.c_str(), line->lineNumber);
            line->printDebug();
        }
    }

    // バイナリ出力
    binSize = 0;
    if (!lines.empty()) {
        memset(bin, 0xFF, sizeof(bin));
        binStart = lines[0]->programCounter;
        std::string prevSource = "";
        int prevLine = 0;
        bool setaddr[0x10000];
        memset(setaddr, 0, sizeof(setaddr));
        for (auto line : lines) {
            if (0x10000 < line->programCounter + line->machine.size()) {
                printf("Error: %s (%d) Program size exceeds 64KB.\n", line->path.c_str(), line->lineNumber);
                return -1;
            }
            for (int n = 0; n < line->machine.size(); n++) {
                if (setaddr[line->programCounter + n]) {
                    printf("Error: %s (%d) Program size exceeds next org address.\n", prevSource.c_str(), prevLine);
                    return -1;
                }
                bin[line->programCounter + n] = line->machine[n];
                setaddr[line->programCounter + n] = true;
            }
            if (!line->machine.empty()) {
                binSize = line->programCounter + line->machine.size() - binStart;
                prevSource = line->path;
                prevLine = line->lineNumber;
            }
        }
    }

    return 0;
}

static int assemble(const char* path)
{
    auto lines = readFile(path);
    if (lines.empty()) {
        printf("Error: Cannot open \"%s\".\n", path);
        return -1;
    }
    return assemble(lines);
}

int main(int argc, char* argv[])
{
    nametable_init();
    char in[1024];
    char out[1024];
    in[0] = 0;
    out[0] = 0;
    int binarySize = 0;
    bool error = false;
    for (int i = 1; i < argc; i++) {
        if ('-' == argv[i][0]) {
            switch (tolower(argv[i][1])) {
                case 'o':
                    i++;
                    if (argc <= i) {
                        error = true;
                        break;
                    }
                    strcpy(out, argv[i]);
                    break;
                case 'b':
                    i++;
                    if (argc <= i) {
                        error = true;
                        break;
                    }
                    if (argv[i][0] == '0' && toupper(argv[i][1]) == 'X') {
                        auto str = hex2dec(&argv[i][2]);
                        binarySize = atoi(str.c_str());
                    } else if (argv[i][0] == '$') {
                        auto str = hex2dec(&argv[i][1]);
                        binarySize = atoi(str.c_str());
                    } else {
                        binarySize = atoi(argv[i]);
                    }
                    break;
                case 'd':
                    i++;
                    if (argc <= i) {
                        error = true;
                        break;
                    } else {
                        for (int j = 0; argv[i][j]; j++) {
                            argv[i][j] = toupper(argv[i][j]);
                        }
                        if (nameTable.find(argv[i]) != nameTable.end()) {
                            printf("Duplicate or unspecified name specified: %s\n", argv[i]);
                            error = true;
                            break;
                        }
                        nametable_add(argv[i], nullptr);
                        defineTable[argv[i]].push_back(std::make_pair(TokenType::None, ""));
                    }
                    break;
                case 'v':
                    showLineDebug = true;
                    break;
                default:
                    error = true;
                    break;
            }
        } else {
            if (in[0]) {
                error = true;
                break;
            } else {
                strcpy(in, argv[i]);
            }
        }
    }

    if (error || !in[0]) {
        puts("usage: vgsasm [-o /path/to/output.bin]");
        puts("              [-b binary_size]");
        puts("              [-d name]");
        puts("              [-v]");
        puts("              /path/to/input.asm");
        return 1;
    }

    if (!out[0]) {
        const char* file = strrchr(in, '/');
        if (file) {
            file++;
        } else {
            file = in;
        }
        strcpy(out, file);
        char* cp = strrchr(out, '.');
        if (cp) {
            *cp = 0;
        }
        strcat(out, ".bin");
    }
    if (assemble(in)) {
        return -1;
    }

    if (binSize < 1) {
        puts("No binary data.");
        return -1;
    } else if (binarySize && (binarySize <= binSize || 0x10000 <= binStart + binarySize)) {
        puts("Binary size overflow.");
        return -1;
    } else {
        FILE* fp = fopen(out, "wb");
        if (!fp) {
            puts("File open error.");
            return -1;
        }
        binSize = binarySize != 0 ? binarySize : binSize;
        if (binSize != fwrite(&bin[binStart], 1, binSize, fp)) {
            puts("File write error.");
            fclose(fp);
            return -1;
        }
        fclose(fp);
    }

    return 0;
}
