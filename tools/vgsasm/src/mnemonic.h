#pragma once
#include "common.h"

class TempAddr
{
  public:
    LineData* line;
    std::string label;
    int midx;
    bool isRelative;
    TempAddr(LineData* line, std::string label, int midx, bool isRelative)
    {
        this->line = line;
        this->label = label;
        this->midx = midx;
        this->isRelative = isRelative;
    }
};

extern std::vector<TempAddr*> tempAddrs;

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

#define ML_PUSH_BC line->machine.push_back(0xC5)
#define ML_PUSH_DE line->machine.push_back(0xD5)
#define ML_PUSH_HL line->machine.push_back(0xE5)
#define ML_PUSH_AF line->machine.push_back(0xF5)
#define ML_PUSH_IX                 \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0xE5)
#define ML_PUSH_IY                 \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0xE5)
#define ML_POP_BC line->machine.push_back(0xC1)
#define ML_POP_DE line->machine.push_back(0xD1)
#define ML_POP_HL line->machine.push_back(0xE1)
#define ML_POP_AF line->machine.push_back(0xF1)
#define ML_POP_IX                  \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0xE1)
#define ML_POP_IY                  \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0xE1)

#define ML_LD_A_n(X)               \
    line->machine.push_back(0x3E); \
    line->machine.push_back(X)
#define ML_LD_B_n(X)               \
    line->machine.push_back(0x06); \
    line->machine.push_back(X)
#define ML_LD_C_n(X)               \
    line->machine.push_back(0x0E); \
    line->machine.push_back(X)
#define ML_LD_D_n(X)               \
    line->machine.push_back(0x16); \
    line->machine.push_back(X)
#define ML_LD_E_n(X)               \
    line->machine.push_back(0x1E); \
    line->machine.push_back(X)
#define ML_LD_H_n(X)               \
    line->machine.push_back(0x26); \
    line->machine.push_back(X)
#define ML_LD_L_n(X)               \
    line->machine.push_back(0x2E); \
    line->machine.push_back(X)

#define ML_ADD_HL_BC line->machine.push_back(0x09)
#define ML_ADD_HL_DE line->machine.push_back(0x19)
#define ML_ADD_HL_HL line->machine.push_back(0x29)
#define ML_ADD_HL_SP line->machine.push_back(0x39)

#define ML_ADD_IX_BC               \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x09)
#define ML_ADD_IX_DE               \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x19)
#define ML_ADD_IX_IX               \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x29)
#define ML_ADD_IX_SP               \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x39)

#define ML_ADD_IY_BC               \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x09)
#define ML_ADD_IY_DE               \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x19)
#define ML_ADD_IY_IY               \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x29)
#define ML_ADD_IY_SP               \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x39)

#define ML_INC_A line->machine.push_back(0x3C)
#define ML_INC_B line->machine.push_back(0x04)
#define ML_INC_C line->machine.push_back(0x0C)
#define ML_INC_D line->machine.push_back(0x14)
#define ML_INC_E line->machine.push_back(0x1C)
#define ML_INC_H line->machine.push_back(0x24)
#define ML_INC_L line->machine.push_back(0x2C)
#define ML_INC_BC line->machine.push_back(0x03)
#define ML_INC_DE line->machine.push_back(0x13)
#define ML_INC_HL line->machine.push_back(0x23)
#define ML_INC_ADDR_HL line->machine.push_back(0x34)
#define ML_INC_SP line->machine.push_back(0x33)
#define ML_INC_IXH                 \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x24)
#define ML_INC_IXL                 \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x2C)
#define ML_INC_IYH                 \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x24)
#define ML_INC_IYL                 \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x2C)
#define ML_INC_IX                  \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x23)
#define ML_INC_IY                  \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x23)
#define ML_INC_ADDR_IX(D)          \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x34); \
    line->machine.push_back(D)
#define ML_INC_ADDR_IY(D)          \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x34); \
    line->machine.push_back(D)

#define ML_DEC_A line->machine.push_back(0x3D)
#define ML_DEC_B line->machine.push_back(0x05)
#define ML_DEC_C line->machine.push_back(0x0D)
#define ML_DEC_D line->machine.push_back(0x15)
#define ML_DEC_E line->machine.push_back(0x1D)
#define ML_DEC_H line->machine.push_back(0x25)
#define ML_DEC_L line->machine.push_back(0x2D)
#define ML_DEC_BC line->machine.push_back(0x0B)
#define ML_DEC_DE line->machine.push_back(0x1B)
#define ML_DEC_HL line->machine.push_back(0x2B)
#define ML_DEC_ADDR_HL line->machine.push_back(0x35)
#define ML_DEC_SP line->machine.push_back(0x3B)
#define ML_DEC_IXH                 \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x25)
#define ML_DEC_IXL                 \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x2D)
#define ML_DEC_IYH                 \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x25)
#define ML_DEC_IYL                 \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x2D)
#define ML_DEC_IX                  \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x2B)
#define ML_DEC_IY                  \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x2B)
#define ML_DEC_ADDR_IX(D)          \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x35); \
    line->machine.push_back(D)
#define ML_DEC_ADDR_IY(D)          \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x35); \
    line->machine.push_back(D)

#define ML_IN_A(N)                 \
    line->machine.push_back(0xDB); \
    line->machine.push_back(N)
#define ML_OUT_A(N)                \
    line->machine.push_back(0xD3); \
    line->machine.push_back(N)

#define ML_LD_A_B line->machine.push_back(0x78)
#define ML_LD_A_C line->machine.push_back(0x79)
#define ML_LD_A_D line->machine.push_back(0x7A)
#define ML_LD_A_E line->machine.push_back(0x7B)
#define ML_LD_A_H line->machine.push_back(0x7C)
#define ML_LD_A_L line->machine.push_back(0x7D)
#define ML_LD_A_A line->machine.push_back(0x7F)
#define ML_LD_A_IXH                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x7C)
#define ML_LD_A_IXL                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x7D)
#define ML_LD_A_IYH                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x7C)
#define ML_LD_A_IYL                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x7D)

#define ML_LD_B_B line->machine.push_back(0x40)
#define ML_LD_B_C line->machine.push_back(0x41)
#define ML_LD_B_D line->machine.push_back(0x42)
#define ML_LD_B_E line->machine.push_back(0x43)
#define ML_LD_B_H line->machine.push_back(0x44)
#define ML_LD_B_L line->machine.push_back(0x45)
#define ML_LD_B_A line->machine.push_back(0x47)
#define ML_LD_B_IXH                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x44)
#define ML_LD_B_IXL                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x45)
#define ML_LD_B_IYH                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x44)
#define ML_LD_B_IYL                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x45)

#define ML_LD_C_B line->machine.push_back(0x48)
#define ML_LD_C_C line->machine.push_back(0x49)
#define ML_LD_C_D line->machine.push_back(0x4A)
#define ML_LD_C_E line->machine.push_back(0x4B)
#define ML_LD_C_H line->machine.push_back(0x4C)
#define ML_LD_C_L line->machine.push_back(0x4D)
#define ML_LD_C_A line->machine.push_back(0x4F)
#define ML_LD_C_IXH                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x4C)
#define ML_LD_C_IXL                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x4D)
#define ML_LD_C_IYH                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x4C)
#define ML_LD_C_IYL                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x4D)

#define ML_LD_D_B line->machine.push_back(0x50)
#define ML_LD_D_C line->machine.push_back(0x51)
#define ML_LD_D_D line->machine.push_back(0x52)
#define ML_LD_D_E line->machine.push_back(0x53)
#define ML_LD_D_H line->machine.push_back(0x54)
#define ML_LD_D_L line->machine.push_back(0x55)
#define ML_LD_D_A line->machine.push_back(0x57)
#define ML_LD_D_IXH                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x54)
#define ML_LD_D_IXL                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x55)
#define ML_LD_D_IYH                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x54)
#define ML_LD_D_IYL                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x55)

#define ML_LD_E_B line->machine.push_back(0x58)
#define ML_LD_E_C line->machine.push_back(0x59)
#define ML_LD_E_D line->machine.push_back(0x5A)
#define ML_LD_E_E line->machine.push_back(0x5B)
#define ML_LD_E_H line->machine.push_back(0x5C)
#define ML_LD_E_L line->machine.push_back(0x5D)
#define ML_LD_E_A line->machine.push_back(0x5F)
#define ML_LD_E_IXH                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x4C)
#define ML_LD_E_IXL                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x4D)
#define ML_LD_E_IYH                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x4C)
#define ML_LD_E_IYL                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x4D)

#define ML_LD_H_B line->machine.push_back(0x60)
#define ML_LD_H_C line->machine.push_back(0x61)
#define ML_LD_H_D line->machine.push_back(0x62)
#define ML_LD_H_E line->machine.push_back(0x63)
#define ML_LD_H_H line->machine.push_back(0x64)
#define ML_LD_H_L line->machine.push_back(0x65)
#define ML_LD_H_A line->machine.push_back(0x67)

#define ML_LD_L_B line->machine.push_back(0x68)
#define ML_LD_L_C line->machine.push_back(0x69)
#define ML_LD_L_D line->machine.push_back(0x6A)
#define ML_LD_L_E line->machine.push_back(0x6B)
#define ML_LD_L_H line->machine.push_back(0x6C)
#define ML_LD_L_L line->machine.push_back(0x6D)
#define ML_LD_L_A line->machine.push_back(0x6F)

#define ML_LD_IXH_A                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x67);
#define ML_LD_IXL_A                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x6F);
#define ML_LD_IYH_A                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x67);
#define ML_LD_IYL_A                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x6F);

#define ML_LD_IXH_B                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x60);
#define ML_LD_IXL_B                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x68);
#define ML_LD_IYH_B                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x60);
#define ML_LD_IYL_B                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x68);

#define ML_LD_IXH_C                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x61);
#define ML_LD_IXL_C                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x69);
#define ML_LD_IYH_C                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x61);
#define ML_LD_IYL_C                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x69);

#define ML_LD_IXH_D                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x62);
#define ML_LD_IXL_D                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x6A);
#define ML_LD_IYH_D                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x62);
#define ML_LD_IYL_D                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x6A);

#define ML_LD_IXH_E                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x63);
#define ML_LD_IXL_E                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x6B);
#define ML_LD_IYH_E                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x63);
#define ML_LD_IYL_E                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x6B);

#define ML_LD_IXH_IXH              \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x64);
#define ML_LD_IXL_IXH              \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x6C);
#define ML_LD_IYH_IYH              \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x64);
#define ML_LD_IYL_IYH              \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x6C);

#define ML_LD_IXH_IXL              \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x65);
#define ML_LD_IXL_IXL              \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0x6D);
#define ML_LD_IYH_IYL              \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x65);
#define ML_LD_IYL_IYL              \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0x6D);

#define ML_LD_A_HL line->machine.push_back(0x7E)
#define ML_LD_B_HL line->machine.push_back(0x46)
#define ML_LD_C_HL line->machine.push_back(0x4E)
#define ML_LD_D_HL line->machine.push_back(0x56)
#define ML_LD_E_HL line->machine.push_back(0x5E)
#define ML_LD_H_HL line->machine.push_back(0x66)
#define ML_LD_L_HL line->machine.push_back(0x6E)
#define ML_LD_IXH_HL \
    ML_PUSH_AF;      \
    ML_LD_A_HL;      \
    ML_LD_IXH_A;     \
    ML_POP_AF
#define ML_LD_IXL_HL \
    ML_PUSH_AF;      \
    ML_LD_A_HL;      \
    ML_LD_IXL_A;     \
    ML_POP_AF
#define ML_LD_IYH_HL \
    ML_PUSH_AF;      \
    ML_LD_A_HL;      \
    ML_LD_IYH_A;     \
    ML_POP_AF
#define ML_LD_IYL_HL \
    ML_PUSH_AF;      \
    ML_LD_A_HL;      \
    ML_LD_IYL_A;     \
    ML_POP_AF

#define ML_LD_BC_BC \
    ML_LD_B_B;      \
    ML_LD_C_C
#define ML_LD_BC_DE \
    ML_LD_B_D;      \
    ML_LD_C_E
#define ML_LD_BC_HL \
    ML_LD_B_H;      \
    ML_LD_C_L
#define ML_LD_BC_IX \
    ML_LD_B_IXH;    \
    ML_LD_C_IXL
#define ML_LD_BC_IY \
    ML_LD_B_IYH;    \
    ML_LD_C_IYL

#define ML_LD_DE_BC \
    ML_LD_D_B;      \
    ML_LD_E_C
#define ML_LD_DE_DE \
    ML_LD_D_D;      \
    ML_LD_E_E
#define ML_LD_DE_HL \
    ML_LD_D_H;      \
    ML_LD_E_L
#define ML_LD_DE_IX \
    ML_LD_D_IXH;    \
    ML_LD_E_IXL
#define ML_LD_DE_IY \
    ML_LD_D_IYH;    \
    ML_LD_E_IYL

#define ML_LD_HL_BC \
    ML_LD_H_B;      \
    ML_LD_L_C
#define ML_LD_HL_DE \
    ML_LD_H_D;      \
    ML_LD_L_E
#define ML_LD_HL_HL \
    ML_LD_H_H;      \
    ML_LD_L_L
#define ML_LD_HL_IX \
    ML_PUSH_IX;     \
    ML_POP_HL
#define ML_LD_HL_IY \
    ML_PUSH_IY;     \
    ML_POP_HL

#define ML_LD_IX_BC \
    ML_LD_IXH_B;    \
    ML_LD_IXL_C
#define ML_LD_IX_DE \
    ML_LD_IXH_D;    \
    ML_LD_IXL_E
#define ML_LD_IX_HL \
    ML_PUSH_HL;     \
    ML_POP_IX
#define ML_LD_IX_IX \
    ML_LD_IXH_IXH;  \
    ML_LD_IXL_IXL
#define ML_LD_IX_IY \
    ML_PUSH_IY;     \
    ML_POP_IX

#define ML_LD_IY_BC \
    ML_LD_IYH_B;    \
    ML_LD_IYL_C
#define ML_LD_IY_DE \
    ML_LD_IYH_D;    \
    ML_LD_IYL_E
#define ML_LD_IY_HL \
    ML_PUSH_HL;     \
    ML_POP_IY
#define ML_LD_IY_IX \
    ML_PUSH_IX;     \
    ML_POP_IY
#define ML_LD_IY_IY \
    ML_LD_IYH_IYH;  \
    ML_LD_IYL_IYL

#define ML_LD_SP_HL line->machine.push_back(0xF9)
#define ML_LD_SP_IX                \
    line->machine.push_back(0xDD); \
    line->machine.push_back(0xF9)
#define ML_LD_SP_IY                \
    line->machine.push_back(0xFD); \
    line->machine.push_back(0xF9)

#define ML_LD_HL_A line->machine.push_back(0x77)
#define ML_LD_HL_B line->machine.push_back(0x70)
#define ML_LD_HL_C line->machine.push_back(0x71)
#define ML_LD_HL_D line->machine.push_back(0x72)
#define ML_LD_HL_E line->machine.push_back(0x73)
#define ML_LD_HL_H line->machine.push_back(0x74)
#define ML_LD_HL_L line->machine.push_back(0x75)
#define ML_LD_HL_IXH \
    ML_PUSH_AF;      \
    ML_LD_A_IXH;     \
    ML_LD_HL_A;      \
    ML_POP_AF
#define ML_LD_HL_IXL \
    ML_PUSH_AF;      \
    ML_LD_A_IXL;     \
    ML_LD_HL_A;      \
    ML_POP_AF
#define ML_LD_HL_IYH \
    ML_PUSH_AF;      \
    ML_LD_A_IYH;     \
    ML_LD_HL_A;      \
    ML_POP_AF
#define ML_LD_HL_IYL \
    ML_PUSH_AF;      \
    ML_LD_A_IYL;     \
    ML_LD_HL_A;      \
    ML_POP_AF

#define ML_RET line->machine.push_back(0xC9)
#define ML_RET_NZ line->machine.push_back(0xC0)
#define ML_RET_Z line->machine.push_back(0xC8)
#define ML_RET_NC line->machine.push_back(0xD0)
#define ML_RET_C line->machine.push_back(0xD8)
#define ML_RET_PO line->machine.push_back(0xE0)
#define ML_RET_PE line->machine.push_back(0xE8)
#define ML_RET_P line->machine.push_back(0xF0)
#define ML_RET_M line->machine.push_back(0xF8)
#define ML_RETI                    \
    line->machine.push_back(0xED); \
    line->machine.push_back(0x4D)
#define ML_RETN                    \
    line->machine.push_back(0xED); \
    line->machine.push_back(0x45)

#define ML_ADD_A_A line->machine.push_back(0x87)
#define ML_ADD_A_B line->machine.push_back(0x80)
#define ML_ADD_A_C line->machine.push_back(0x81)
#define ML_ADD_A_D line->machine.push_back(0x82)
#define ML_ADD_A_E line->machine.push_back(0x83)
#define ML_ADD_A_H line->machine.push_back(0x84)
#define ML_ADD_A_L line->machine.push_back(0x85)

#define ML_JR_NZ(E)                \
    line->machine.push_back(0x20); \
    line->machine.push_back(E)
#define ML_JR_Z(E)                 \
    line->machine.push_back(0x28); \
    line->machine.push_back(E)
#define ML_JR_NC(E)                \
    line->machine.push_back(0x30); \
    line->machine.push_back(E)
#define ML_JR_C(E)                 \
    line->machine.push_back(0x38); \
    line->machine.push_back(E)

#define ML_LD_NN_A(NN)                  \
    line->machine.push_back(0x32);      \
    line->machine.push_back(NN & 0xFF); \
    line->machine.push_back((NN & 0xFF00) >> 8)
#define ML_LD_NN_BC(NN)                 \
    line->machine.push_back(0xED);      \
    line->machine.push_back(0x43);      \
    line->machine.push_back(NN & 0xFF); \
    line->machine.push_back((NN & 0xFF00) >> 8)
#define ML_LD_NN_DE(NN)                 \
    line->machine.push_back(0xED);      \
    line->machine.push_back(0x53);      \
    line->machine.push_back(NN & 0xFF); \
    line->machine.push_back((NN & 0xFF00) >> 8)
#define ML_LD_NN_HL(NN)                 \
    line->machine.push_back(0x22);      \
    line->machine.push_back(NN & 0xFF); \
    line->machine.push_back((NN & 0xFF00) >> 8)
#define ML_LD_NN_SP(NN)                 \
    line->machine.push_back(0xED);      \
    line->machine.push_back(0x73);      \
    line->machine.push_back(NN & 0xFF); \
    line->machine.push_back((NN & 0xFF00) >> 8)
#define ML_LD_NN_IX(NN)                 \
    line->machine.push_back(0xDD);      \
    line->machine.push_back(0x22);      \
    line->machine.push_back(NN & 0xFF); \
    line->machine.push_back((NN & 0xFF00) >> 8)
#define ML_LD_NN_IY(NN)                 \
    line->machine.push_back(0xFD);      \
    line->machine.push_back(0x22);      \
    line->machine.push_back(NN & 0xFF); \
    line->machine.push_back((NN & 0xFF00) >> 8)

#define ML_ADD_HL line->machine.push_back(0x86)
#define ML_ADC_HL line->machine.push_back(0x8E)
#define ML_SUB_HL line->machine.push_back(0x96)
#define ML_SBC_HL line->machine.push_back(0x9E)
#define ML_AND_HL line->machine.push_back(0xA6)
#define ML_OR_HL line->machine.push_back(0xB6)
#define ML_XOR_HL line->machine.push_back(0xAE)
#define ML_CP_HL line->machine.push_back(0xBE)

bool mnemonic_format_check(LineData* line, int size, ...);
bool mnemonic_format_test(LineData* line, int size, ...);
bool mnemonic_format_begin(LineData* line, int size, ...);
bool mnemonic_range(LineData* line, int n, int min, int max);
bool mnemonic_is_reg16(Operand op);
void mnemonic_single(LineData* line, uint8_t code);
void mnemonic_single_ED(LineData* line, uint8_t code);
