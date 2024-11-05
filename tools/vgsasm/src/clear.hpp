/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

void clear_delete_token(std::vector<LineData*>* lines)
{
    for (auto it1 = lines->begin(); it1 != lines->end();) {
        auto token = &(*it1)->token;
        for (auto it2 = token->begin(); it2 != token->end();) {
            if (it2->first == TokenType::Delete) {
                token->erase(it2);
            } else {
                it2++;
            }
        }
        if (token->empty()) {
            lines->erase(it1);
        } else {
            it1++;
        }
    }
}
