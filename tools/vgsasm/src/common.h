/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <list>

enum class TokenType {
    None,             // トークンが無い
    Delete,           // 削除マーク
    Mnemonic,         // ニーモニック
    Operand,          // オペランド
    Org,              // 開始アドレス
    Struct,           // 構造体
    StructName,       // 構造体名
    StructArray,      // 構造体配列
    StructArrayField, // 構造体配列のフィールド指定
    SizeOf,           // 構造体サイズ
    Offset,           // 構造体フィールドのオフセット
    String,           // 文字列 "str" or 'str'
    Label,            // ラベル（HOGE: or .HOGE）
    LabelAt,          // @HOGE 形式のラベル
    LabelJump,        // ジャンプ先ラベル
    Numeric,          // 数値
    Split,            // , カンマ
    Plus,             // + プラス
    Minus,            // - マイナス
    PlusOrMinus,      // + or - (比較用)
    Div,              // / 割り算
    Mul,              // * 掛け算
    Modulo,           // % 剰余残
    ArrayBegin,       // [ 配列起点
    ArrayEnd,         // ] 配列終点
    BracketBegin,     // ( カッコ
    BracketEnd,       // ) カッコ
    AddressBegin,     // ( カッコ（※特定箇所のみ）
    AddressEnd,       // ) カッコ (※特定箇所のみ)
    ArgumentBegin,    // ( カッコ（※特定箇所のみ）
    ArgumentEnd,      // ) カッコ (※特定箇所のみ)
    ScopeBegin,       // { スコープ起点
    ScopeEnd,         // } スコープ終点
    And,              // & アンパーサンド
    Or,               // |
    Caret,            // ^
    Inc,              // インクリメント
    Dec,              // デクリメント
    Binary,           // #binary
    Macro,            // #macro
    MacroCaller,      // マクロ呼び出し
    Equal,            // = 代入
    EqualPlus,        // += 加算代入
    EqualMinus,       // -= 減算代入
    EqualAnd,         // &= 論理積代入
    EqualOr,          // |= 論理和代入
    EqualXor,         // ^= 排他的論理和代入
    EqualShiftLeft,   // <<= 左論理シフト代入
    EqualShiftRight,  // >>= 右論理シフト代入
    EqualMul,         // *= 乗算代入
    EqualDiv,         // /= 除算代入
    EqualMod,         // %= 剰余残代入
    ArrowLeft,        // <-
    ArrowRight,       // ->
    Other             // その他 (構文解析の仮定で最終的にはなくなる)
};

enum class Mnemonic;

class LineData
{
  public:
    bool error;
    std::string errmsg;
    std::string path;
    int lineNumber;
    int programCounter;
    bool programCounterInit;
    bool isAssignmnt;
    std::string text;
    std::vector<std::pair<TokenType, std::string>> token;
    std::vector<uint8_t> machine;
    LineData(std::string text) { LineData(nullptr, 0, text); }
    LineData(const char* path, int lineNumber, std::string text);
    LineData(LineData* from);
    LineData();
    void printDebug();
};

class StructField
{
  public:
    LineData* line;
    std::string name;
    std::vector<std::pair<TokenType, std::string>> token;
    int size;
    int count;
    int address;

    StructField(LineData* line, std::string name)
    {
        this->line = line;
        this->name = name;
        this->size = 0;
        this->count = 0;
        this->address = 0;
    }
};

class Struct
{
  public:
    LineData* line;
    std::string name;
    std::string after;
    int start;
    int size;
    int afterArray = 0;
    std::vector<StructField*> fields;

    Struct(LineData* line, std::string name)
    {
        this->line = line;
        this->name = name;
        this->after = "";
        this->start = 0;
        this->size = 0;
        this->afterArray = 0;
    }
};

class Macro
{
  public:
    std::string name;
    std::vector<std::string> args;
    std::vector<LineData*> lines;
    LineData* refer;
    std::vector<Macro*> caller;

    Macro(std::string name, LineData* refer)
    {
        this->name = name;
        this->refer = refer;
        this->args.clear();
        this->lines.clear();
    }
};

enum class Mnemonic {
    None = 0,
    LD,
    LDI,
    LDD,
    LDIR,
    LDDR,
    PUSH,
    POP,
    EX,
    EXX,
    CP,
    CPI,
    CPIR,
    CPD,
    CPDR,
    ADD,
    ADC,
    SUB,
    SBC,
    AND,
    OR,
    XOR,
    INC,
    DEC,
    DAA,
    CPL,
    NEG,
    CCF,
    SCF,
    NOP,
    HALT,
    RL,
    RLA,
    RLC,
    RLCA,
    RR,
    RRA,
    RRC,
    RRCA,
    SLA,
    SLL,
    SRA,
    SRL,
    RLD,
    RRD,
    BIT,
    SET,
    RES,
    JP,
    JR,
    DJNZ,
    CALL,
    RET,
    RETI,
    RETN,
    RST,
    OUT,
    OUTI,
    OTIR,
    OUTD,
    OTDR,
    IN,
    INI,
    INIR,
    IND,
    INDR,
    DI,
    EI,
    IM,
    DB,
    DW,
    MUL,  // VGS-Zero
    MULS, // VGS-Zero
    DIV,  // VGS-Zero
    DIVS, // VGS-Zero
    MOD,  // VGS-Zero
    ATN2, // VGS-Zero
    SIN,  // VGS-Zero
    COS,  // VGS-Zero
};

enum class Operand {
    A,
    B,
    C,
    D,
    E,
    F,
    H,
    L,
    IXH,
    IXL,
    IYH,
    IYL,
    AF,
    AF_dash,
    BC,
    DE,
    HL,
    IX,
    IY,
    SP,
    NZ,
    Z,
    NC,
    PO,
    PE,
    P,
    M,
};

// tables (defined in main.hpp)
extern std::map<std::string, LineData*> nameTable;
extern std::vector<std::string> includeFiles;
extern std::map<std::string, std::vector<std::pair<TokenType, std::string>>> defineTable;
extern std::map<std::string, LineData*> labelTable;
extern std::map<std::string, Mnemonic> mnemonicTable;
extern std::map<std::string, Operand> operandTable;
extern std::map<std::string, Struct*> structTable;
extern std::vector<std::string> structNameList;
extern std::map<std::string, Macro*> macroTable;

extern int arrayCount;
extern int bracketCount;
extern int scopeCount;
extern LineData* lastScopeBegin;

void addNameTable(std::string name, LineData* line);             // main.cpp
bool checkNameTable(std::string name);                           // main.cpp
void trim_string(char* src);                                     // main.cpp
std::vector<std::string> split_token(std::string str, char del); // main.cpp
std::string oct2dec(const char* hex);                            // formulas.hpp
std::string hex2dec(const char* hex);                            // formulas.hpp
std::string bin2dec(const char* bin);                            // formulas.hpp
