/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

int lastOrg = -1;

void org_parse(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->second != "ORG") {
            continue;
        }
        if (it != line->token.begin()) {
            line->error = true;
            line->errmsg = "`org` must appear at the beginning of the line.";
            return;
        }
        it++;
        if (it == line->token.end()) {
            line->error = true;
            line->errmsg = "`org` address is not specified.";
            return;
        }
        if (it->first != TokenType::Numeric) {
            line->error = true;
            line->errmsg = "Unexpected symbol: " + it->second;
            return;
        }
        auto org = atoi(it->second.c_str());
        if (0xFFFF < org || org < 0) {
            line->error = true;
            line->errmsg = "`org` address is incorrect: " + it->second;
            return;
        }
        if (it + 1 != line->token.end()) {
            line->error = true;
            line->errmsg = "Unexpected symbol: " + (it + 1)->second;
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
        line->token.erase(it);
        return;
    }
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
