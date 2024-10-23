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
    ArrayBegin,       // [ 配列起点
    ArrayEnd,         // ] 配列終点
    BracketBegin,     // ( カッコ
    BracketEnd,       // ) カッコ
    AddressBegin,     // ( カッコ（※特定箇所のみ）
    AddressEnd,       // ) カッコ (※特定箇所のみ)
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
    int start;
    int size;
    std::vector<StructField*> fields;

    Struct(LineData* line, std::string name)
    {
        this->line = line;
        this->name = name;
        this->start = 0;
        this->size = 0;
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

// tables (defined in main.cpp)
extern std::map<std::string, LineData*> nameTable;
extern std::vector<std::string> includeFiles;
extern std::map<std::string, std::vector<std::pair<TokenType, std::string>>> defineTable;
extern std::map<std::string, LineData*> labelTable;
extern std::map<std::string, Mnemonic> mnemonicTable;
extern std::map<std::string, Operand> operandTable;
extern std::map<std::string, Struct*> structTable;
extern std::map<std::string, Macro*> macroTable;

extern int arrayCount;
extern int bracketCount;
extern int scopeCount;
extern LineData* lastScopeBegin;

void addNameTable(std::string name, LineData* line);                                  // main.cpp
bool checkNameTable(std::string name);                                                // main.cpp
void trim_string(char* src);                                                          // main.cpp
std::vector<std::string> split_token(std::string str, char del);                      // main.cpp
void parse_binary(LineData* line);                                                    // prep.cpp
void init_define();                                                                   // prep.cpp
bool parse_define(LineData* line);                                                    // prep.cpp
void replace_define(LineData* line);                                                  // prep.cpp
void parse_org(LineData* line);                                                       // prep.cpp
void setpc_org(std::vector<LineData*>* lines);                                        // prep.cpp
void bracket_to_address(LineData* line);                                              // prep.cpp
bool bracket_eliminate(LineData* line);                                               // prep.cpp
std::vector<LineData*> readFile(const char* filePath);                                // file.cpp
std::string evaluate_formulas(std::vector<std::pair<TokenType, std::string>>* token); // formulas.cpp
void evaluate_formulas(LineData* line);                                               // formulas.cpp
void evaluate_formulas_array(LineData* line);                                         // formulas.cpp
void replace_assignment(LineData* line);                                              // formulas.cpp
std::string oct2dec(const char* hex);                                                 // formulas.cpp
std::string hex2dec(const char* hex);                                                 // formulas.cpp
std::string bin2dec(const char* bin);                                                 // formulas.cpp
void parse_numeric(LineData* line);                                                   // formulas.cpp
void parse_numeric_minus(LineData* line);                                             // formulas.cpp
void parse_numeric_plus(LineData* line);                                              // formulas.cpp
void split_increment(std::vector<LineData*>* lines);                                  // formulas.cpp
LineData* parse_label(LineData* line);                                                // label.cpp
void parse_label_jump(LineData* line);                                                // label.cpp
void extract_anonymous_label(std::vector<LineData*>* lines);                          // label.cpp
void extract_string_literal(std::vector<LineData*>* lines);                           // label.cpp
void parse_mneoimonic(LineData* line);                                                // mnemonic.cpp
void mnemonic_syntax_check(std::vector<LineData*>* lines);                            // mnemonic.cpp
void parse_operand(LineData* line);                                                   // mnemonic.cpp
bool operand_is_condition(Operand op);                                                // mnemonic.cpp
bool operand_is_condition(std::string str);                                           // mnemonic.cpp
void parse_sizeof(LineData* line);                                                    // struct.cpp
void replace_sizeof(LineData* line);                                                  // struct.cpp
void parse_offset(LineData* line);                                                    // struct.cpp
void replace_offset(LineData* line);                                                  // struct.cpp
void parse_struct(LineData* line);                                                    // struct.cpp
bool struct_syntax_check(std::vector<LineData*>* lines);                              // struct.cpp
bool struct_check_size();                                                             // struct.cpp
void parse_struct_name(LineData* line);                                               // struct.cpp
void parse_struct_array(LineData* line);                                              // struct.cpp
void replace_struct(LineData* line);                                                  // struct.cpp
void parse_macro(LineData* line);                                                     // macro.cpp
void macro_syntax_check(std::vector<LineData*>* lines);                               // macro.cpp
void extract_macro_call(std::vector<LineData*>* lines);                               // macro.cpp
