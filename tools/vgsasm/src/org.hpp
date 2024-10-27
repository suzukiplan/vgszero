/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

static int lastOrg = -1;

void org_parse(LineData* line)
{
    if (line->token.empty()) {
        return;
    }
    if (line->token[0].second != "ORG") {
        return;
    }
    if (line->token.size() != 2 || line->token[1].first != TokenType::Numeric) {
        line->error = true;
        line->errmsg = "ORG formatting is incorrect.";
        return;
    }
    auto org = atoi(line->token[1].second.c_str());
    if (0xFFFF < org || org < 0) {
        line->error = true;
        line->errmsg = "org is not the correct address (0x0000 to 0xFFFF)";
        return;
    }
    if (lastOrg >= org) {
        line->error = true;
        line->errmsg = "Multiple orgs must be written in ascending order from the beginning of the line.";
        return;
    }
    lastOrg = org;

    line->token[0].first = TokenType::Org;
    line->token[0].second = line->token[1].second;
    line->token.erase(line->token.begin() + 1);
}

void org_setpc(std::vector<LineData*>* lines)
{
    auto prev = lines->end();
    for (auto it = lines->begin(); it != lines->end(); it++) {
        // 直前の行が ORG の行を探す
        if (prev != lines->end() && !(*prev)->token.empty() && (*prev)->token[0].first == TokenType::Org) {
            (*it)->programCounterInit = true;
            (*it)->programCounter = atoi((*prev)->token[0].second.c_str());
            lines->erase(prev);
            it = lines->begin();
        }
        prev = it;
    }
}
