#include "common.h"
#include "mnemonic.h"

void mnemonic_bit_op(LineData* line, Mnemonic mne);                  // mnemonic_bit.cpp
void mnemonic_calc8(LineData* line, uint8_t code);                   // mnemonic_calc.cpp
void mnemonic_calc16(LineData* line, uint8_t code);                  // mnemonic_calc.cpp
void mnemonic_calcOH(LineData* line, uint8_t code8, uint8_t code16); // mnemonic_calc.cpp
void mnemonic_CALL(LineData* line);                                  // mnemonic_call.cpp
void mnemonic_RET(LineData* line);                                   // mnemonic_call.cpp
void mnemonic_RST(LineData* line);                                   // mnemonic_call.cpp
void mnemonic_EX(LineData* line);                                    // mnemonic_ex.cpp
void mnemonic_INC(LineData* line);                                   // mnemonic_incdec.cpp
void mnemonic_DEC(LineData* line);                                   // mnemonic_incdec.cpp
void mnemonic_IN(LineData* line);                                    // mnemonic_io.cpp
void mnemonic_OUT(LineData* line);                                   // mnemonic_io.cpp
void mnemonic_JP(LineData* line);                                    // mnemonic_jump.cpp
void mnemonic_JR(LineData* line);                                    // mnemonic_jump.cpp
void mnemonic_DJNZ(LineData* line);                                  // mnemonic_jump.cpp
void mnemonic_LD(LineData* line);                                    // mnemonic_load.cpp
void mnemonic_shift(LineData* line, uint8_t code);                   // mnemonic_shift.cpp
void mnemonic_PUSH(LineData* line);                                  // mnemonic_stack.cpp
void mnemonic_POP(LineData* line);                                   // mnemonic_stack.cpp
void mnemonic_DB(LineData* line);                                    // mnemonic_data.cpp
void mnemonic_DW(LineData* line);                                    // mnemonic_data.cpp
void mnemonic_MUL(LineData* line);                                   // mnemonic_vgs.cpp
void mnemonic_MULS(LineData* line);                                  // mnemonic_vgs.cpp
void mnemonic_DIV(LineData* line);                                   // mnemonic_vgs.cpp
void mnemonic_DIVS(LineData* line);                                  // mnemonic_vgs.cpp
void mnemonic_MOD(LineData* line);                                   // mnemonic_vgs.cpp
void mnemonic_ATN2(LineData* line);                                  // mnemonic_vgs.cpp
void mnemonic_SIN(LineData* line);                                   // mnemonic_vgs.cpp
void mnemonic_COS(LineData* line);                                   // mnemonic_vgs.cpp

std::vector<TempAddr*> tempAddrs;

std::map<std::string, Mnemonic> mnemonicTable = {
    {"LD", Mnemonic::LD},
    {"LDI", Mnemonic::LDI},
    {"LDIR", Mnemonic::LDIR},
    {"LDD", Mnemonic::LDD},
    {"LDDR", Mnemonic::LDDR},
    {"PUSH", Mnemonic::PUSH},
    {"POP", Mnemonic::POP},
    {"EX", Mnemonic::EX},
    {"EXX", Mnemonic::EXX},
    {"CP", Mnemonic::CP},
    {"CPI", Mnemonic::CPI},
    {"CPIR", Mnemonic::CPIR},
    {"CPD", Mnemonic::CPD},
    {"CPDR", Mnemonic::CPDR},
    {"ADD", Mnemonic::ADD},
    {"ADC", Mnemonic::ADC},
    {"SUB", Mnemonic::SUB},
    {"SBC", Mnemonic::SBC},
    {"AND", Mnemonic::AND},
    {"OR", Mnemonic::OR},
    {"XOR", Mnemonic::XOR},
    {"EOR", Mnemonic::XOR},
    {"INC", Mnemonic::INC},
    {"DEC", Mnemonic::DEC},
    {"DAA", Mnemonic::DAA},
    {"CPL", Mnemonic::CPL},
    {"NEG", Mnemonic::NEG},
    {"CCF", Mnemonic::CCF},
    {"SCF", Mnemonic::SCF},
    {"NOP", Mnemonic::NOP},
    {"HALT", Mnemonic::HALT},
    {"RL", Mnemonic::RL},
    {"RLA", Mnemonic::RLA},
    {"RLC", Mnemonic::RLC},
    {"RLCA", Mnemonic::RLCA},
    {"RR", Mnemonic::RR},
    {"RRA", Mnemonic::RRA},
    {"RRC", Mnemonic::RRC},
    {"RRCA", Mnemonic::RRCA},
    {"SLA", Mnemonic::SLA},
    {"SL", Mnemonic::SLA},
    {"SLL", Mnemonic::SLL},
    {"SRA", Mnemonic::SRA},
    {"SRL", Mnemonic::SRL},
    {"SR", Mnemonic::SRL},
    {"RLD", Mnemonic::RLD},
    {"RRD", Mnemonic::RRD},
    {"BIT", Mnemonic::BIT},
    {"SET", Mnemonic::SET},
    {"RES", Mnemonic::RES},
    {"JP", Mnemonic::JP},
    {"JR", Mnemonic::JR},
    {"DJNZ", Mnemonic::DJNZ},
    {"CALL", Mnemonic::CALL},
    {"RST", Mnemonic::RST},
    {"RET", Mnemonic::RET},
    {"RETI", Mnemonic::RETI},
    {"RETN", Mnemonic::RETN},
    {"OUT", Mnemonic::OUT},
    {"OUTI", Mnemonic::OUTI},
    {"OUTIR", Mnemonic::OTIR},
    {"OTIR", Mnemonic::OTIR},
    {"OUTD", Mnemonic::OUTD},
    {"OUTDR", Mnemonic::OTDR},
    {"OTDR", Mnemonic::OTDR},
    {"IN", Mnemonic::IN},
    {"INI", Mnemonic::INI},
    {"INIR", Mnemonic::INIR},
    {"IND", Mnemonic::IND},
    {"INDR", Mnemonic::INDR},
    {"DI", Mnemonic::DI},
    {"EI", Mnemonic::EI},
    {"IM", Mnemonic::IM},
    {"DB", Mnemonic::DB},
    {"DEFB", Mnemonic::DB},
    {"DW", Mnemonic::DW},
    {"DEFW", Mnemonic::DW},
    {"BYTE", Mnemonic::DB},
    {"WORD", Mnemonic::DW},
    {"MUL", Mnemonic::MUL},
    {"MULS", Mnemonic::MULS},
    {"DIV", Mnemonic::DIV},
    {"DIVS", Mnemonic::DIVS},
    {"MOD", Mnemonic::MOD},
    {"ATN2", Mnemonic::ATN2},
    {"SIN", Mnemonic::SIN},
    {"COS", Mnemonic::COS},
};

static bool parseMneimonicSkipScope = false;

void parse_mneoimonic(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        // スコープ内ではニーモニック解析をスキップ
        if (parseMneimonicSkipScope) {
            if (it->first == TokenType::ScopeEnd) {
                parseMneimonicSkipScope = false;
            }
            continue;
        }

        // スコープ開始を検出したらスキップ
        if (it->first == TokenType::ScopeBegin) {
            parseMneimonicSkipScope = true;
            continue;
        }

        // Other ならチェック
        if (it->first == TokenType::Other) {
            auto m = mnemonicTable.find(it->second);
            if (m != mnemonicTable.end()) {
                if (it == line->token.begin()) {
                    it->first = TokenType::Mnemonic;
                } else {
                    line->error = true;
                    line->errmsg = "A mnemonic specified position was incorrect.";
                    return;
                }
            }
        }
    }
}

bool mnemonic_format_check(LineData* line, int size, ...)
{
    if (size < 1 || line->token.size() != size) {
        line->error = true;
        line->errmsg = "Excessive or insufficient number of operands.";
        return false;
    }
    if (1 == size) {
        return true;
    }
    va_list arg;
    va_start(arg, size);
    bool error = false;
    for (auto it = line->token.begin() + 1; it != line->token.end(); it++) {
        auto expect = va_arg(arg, TokenType);
        if (!error) {
            if (expect == TokenType::PlusOrMinus) {
                error = it->first != TokenType::Plus && it->first != TokenType::Minus;
            } else {
                error = it->first != expect;
            }
            if (error) {
                line->error = true;
                line->errmsg = "Unexpected operand: " + it->second;
            }
        }
    }
    va_end(arg);
    return !line->error;
}

bool mnemonic_format_test(LineData* line, int size, ...)
{
    if (size < 1 || line->token.size() != size) {
        return false;
    }
    if (1 == size) {
        return true;
    }
    va_list arg;
    va_start(arg, size);
    bool error = false;
    for (auto it = line->token.begin() + 1; it != line->token.end(); it++) {
        auto expect = va_arg(arg, TokenType);
        if (!error) {
            if (expect == TokenType::PlusOrMinus) {
                error = it->first != TokenType::Plus && it->first != TokenType::Minus;
            } else {
                error = it->first != expect;
            }
        }
    }
    va_end(arg);
    return !error;
}

bool mnemonic_format_begin(LineData* line, int size, ...)
{
    if (size < 1 || line->token.size() < size) {
        return false;
    }
    if (1 == size) {
        return true;
    }
    va_list arg;
    va_start(arg, size);
    bool error = false;
    for (int n = 1; n < size; n++) {
        auto it = line->token.begin() + n;
        auto expect = va_arg(arg, TokenType);
        if (!error) {
            if (expect == TokenType::PlusOrMinus) {
                error = it->first != TokenType::Plus && it->first != TokenType::Minus;
            } else {
                error = it->first != expect;
            }
        }
    }
    va_end(arg);
    return !error;
}

bool mnemonic_range(LineData* line, int n, int min, int max)
{
    if (n < min || max < n) {
        line->error = true;
        line->errmsg = "Numerical range incorrect: " + std::to_string(n);
        return false;
    }
    return true;
}

bool mnemonic_is_reg16(Operand op)
{
    switch (op) {
        case Operand::AF: return true;
        case Operand::BC: return true;
        case Operand::DE: return true;
        case Operand::HL: return true;
        case Operand::IX: return true;
        case Operand::IY: return true;
        case Operand::SP: return true;
        default: return false;
    }
}

void mnemonic_single(LineData* line, uint8_t code)
{
    if (mnemonic_format_check(line, 1)) {
        line->machine.push_back(code);
    }
}

void mnemonic_single_ED(LineData* line, uint8_t code)
{
    if (mnemonic_format_check(line, 1)) {
        line->machine.push_back(0xED);
        line->machine.push_back(code);
    }
}

void mnemonic_IM(LineData* line)
{
    if (mnemonic_format_check(line, 2, TokenType::Numeric)) {
        line->machine.push_back(0xED);
        switch (atoi(line->token[1].second.c_str())) {
            case 0: line->machine.push_back(0x46); break;
            case 1: line->machine.push_back(0x56); break;
            case 2: line->machine.push_back(0x5E); break;
            default:
                line->error = true;
                line->errmsg = "Unsupported interrupt mode: " + line->token[1].second;
        }
    }
}

static void setpc(LineData* prev, LineData* cur)
{
    if (cur->programCounterInit) {
        return;
    }
    if (!cur->programCounterInit) {
        cur->programCounterInit = true;
        if (prev) {
            cur->programCounter = prev->programCounter + prev->machine.size();
        } else {
            cur->programCounter = 0;
        }
    }
}

static void mnemonic_syntax_check_exec(std::vector<LineData*>* lines)
{
    LineData* prev = nullptr;
    for (auto line : *lines) {
        if (line->token.empty()) {
            setpc(prev, line);
            prev = line;
            continue;
        }
        if (line->token[0].first != TokenType::Mnemonic) {
            if (line->token[0].first != TokenType::Binary &&
                line->token[0].first != TokenType::Label &&
                line->token[0].first != TokenType::LabelAt) {
                line->error = true;
                line->errmsg = "Unexpected symbol: " + line->token[0].second;
            }
            setpc(prev, line);
            prev = line;
            continue;
        }
        auto m = mnemonicTable[line->token[0].second];
        switch (m) {
            case Mnemonic::LD: mnemonic_LD(line); break;
            case Mnemonic::IM: mnemonic_IM(line); break;
            case Mnemonic::PUSH: mnemonic_PUSH(line); break;
            case Mnemonic::POP: mnemonic_POP(line); break;
            case Mnemonic::EX: mnemonic_EX(line); break;
            case Mnemonic::NOP: mnemonic_single(line, 0x00); break;
            case Mnemonic::DI: mnemonic_single(line, 0xF3); break;
            case Mnemonic::EI: mnemonic_single(line, 0xFB); break;
            case Mnemonic::HALT: mnemonic_single(line, 0x76); break;
            case Mnemonic::EXX: mnemonic_single(line, 0xD9); break;
            case Mnemonic::DAA: mnemonic_single(line, 0x27); break;
            case Mnemonic::CPL: mnemonic_single(line, 0x2F); break;
            case Mnemonic::CCF: mnemonic_single(line, 0x3F); break;
            case Mnemonic::SCF: mnemonic_single(line, 0x37); break;
            case Mnemonic::NEG: mnemonic_single_ED(line, 0x44); break;
            case Mnemonic::LDI: mnemonic_single_ED(line, 0xA0); break;
            case Mnemonic::LDIR: mnemonic_single_ED(line, 0xB0); break;
            case Mnemonic::LDD: mnemonic_single_ED(line, 0xA8); break;
            case Mnemonic::LDDR: mnemonic_single_ED(line, 0xB8); break;
            case Mnemonic::CPI: mnemonic_single_ED(line, 0xA1); break;
            case Mnemonic::CPIR: mnemonic_single_ED(line, 0xB1); break;
            case Mnemonic::CPD: mnemonic_single_ED(line, 0xA9); break;
            case Mnemonic::CPDR: mnemonic_single_ED(line, 0xB9); break;
            case Mnemonic::OUTI: mnemonic_single_ED(line, 0xA3); break;
            case Mnemonic::OTIR: mnemonic_single_ED(line, 0xB3); break;
            case Mnemonic::OUTD: mnemonic_single_ED(line, 0xAB); break;
            case Mnemonic::OTDR: mnemonic_single_ED(line, 0xBB); break;
            case Mnemonic::INI: mnemonic_single_ED(line, 0xA2); break;
            case Mnemonic::INIR: mnemonic_single_ED(line, 0xB2); break;
            case Mnemonic::IND: mnemonic_single_ED(line, 0xAA); break;
            case Mnemonic::INDR: mnemonic_single_ED(line, 0xBA); break;
            case Mnemonic::AND: mnemonic_calc8(line, 0xA0); break;
            case Mnemonic::OR: mnemonic_calc8(line, 0xB0); break;
            case Mnemonic::XOR: mnemonic_calc8(line, 0xA8); break;
            case Mnemonic::CP: mnemonic_calc8(line, 0xB8); break;
            case Mnemonic::ADD: mnemonic_calcOH(line, 0x80, 0x09); break;
            case Mnemonic::ADC: mnemonic_calcOH(line, 0x88, 0x4A); break;
            case Mnemonic::SUB: mnemonic_calcOH(line, 0x90, 0x00); break;
            case Mnemonic::SBC: mnemonic_calcOH(line, 0x98, 0x42); break;
            case Mnemonic::BIT: mnemonic_bit_op(line, Mnemonic::BIT); break;
            case Mnemonic::SET: mnemonic_bit_op(line, Mnemonic::SET); break;
            case Mnemonic::RES: mnemonic_bit_op(line, Mnemonic::RES); break;
            case Mnemonic::INC: mnemonic_INC(line); break;
            case Mnemonic::DEC: mnemonic_DEC(line); break;
            case Mnemonic::RLCA: mnemonic_single(line, 0x07); break;
            case Mnemonic::RLA: mnemonic_single(line, 0x17); break;
            case Mnemonic::RRCA: mnemonic_single(line, 0x0F); break;
            case Mnemonic::RRA: mnemonic_single(line, 0x1F); break;
            case Mnemonic::RLC: mnemonic_shift(line, 0x00); break;
            case Mnemonic::RL: mnemonic_shift(line, 0x10); break;
            case Mnemonic::RRC: mnemonic_shift(line, 0x08); break;
            case Mnemonic::RR: mnemonic_shift(line, 0x18); break;
            case Mnemonic::SLA: mnemonic_shift(line, 0x20); break;
            case Mnemonic::SRA: mnemonic_shift(line, 0x28); break;
            case Mnemonic::SLL: mnemonic_shift(line, 0x30); break;
            case Mnemonic::SRL: mnemonic_shift(line, 0x38); break;
            case Mnemonic::RLD: mnemonic_single_ED(line, 0x6F); break;
            case Mnemonic::RRD: mnemonic_single_ED(line, 0x67); break;
            case Mnemonic::IN: mnemonic_IN(line); break;
            case Mnemonic::OUT: mnemonic_OUT(line); break;
            case Mnemonic::JP: mnemonic_JP(line); break;
            case Mnemonic::JR: mnemonic_JR(line); break;
            case Mnemonic::DJNZ: mnemonic_DJNZ(line); break;
            case Mnemonic::CALL: mnemonic_CALL(line); break;
            case Mnemonic::RET: mnemonic_RET(line); break;
            case Mnemonic::RETI: mnemonic_single_ED(line, 0x4D); break;
            case Mnemonic::RETN: mnemonic_single_ED(line, 0x45); break;
            case Mnemonic::RST: mnemonic_RST(line); break;
            case Mnemonic::DB: mnemonic_DB(line); break;
            case Mnemonic::DW: mnemonic_DW(line); break;
            case Mnemonic::MUL: mnemonic_MUL(line); break;
            case Mnemonic::MULS: mnemonic_MULS(line); break;
            case Mnemonic::DIV: mnemonic_DIV(line); break;
            case Mnemonic::DIVS: mnemonic_DIVS(line); break;
            case Mnemonic::MOD: mnemonic_MOD(line); break;
            case Mnemonic::ATN2: mnemonic_ATN2(line); break;
            case Mnemonic::SIN: mnemonic_SIN(line); break;
            case Mnemonic::COS: mnemonic_COS(line); break;
            default:
                printf("Not implemented: %s\n", line->token[0].second.c_str());
                exit(-1);
        }
        setpc(prev, line);
        prev = line;
    }
}

void mnemonic_syntax_check(std::vector<LineData*>* lines)
{
    mnemonic_syntax_check_exec(lines);
    if (!tempAddrs.empty()) {
        for (auto addr : tempAddrs) {
            auto label = labelTable[addr->label];
            auto pc = label->programCounter;
            if (!addr->isRelative) {
                addr->line->machine[addr->midx] = pc & 0x00FF;
                addr->line->machine[addr->midx + 1] = (pc & 0xFF00) >> 8;
            } else {
                auto e = pc - (addr->line->programCounter + 2);
                addr->line->machine[addr->midx] = e & 0xFF;
                if (e < -128 || 127 < e) {
                    addr->line->error = true;
                    addr->line->errmsg = "Relative jump destination address is too far away: ";
                    addr->line->errmsg += std::to_string(e);
                }
            }
            delete addr;
        }
        tempAddrs.clear();
    }
}
