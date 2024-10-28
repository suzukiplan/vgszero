/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

// 文字列リテラルを探索して無名ラベルに置換
void string_literal_extract(std::vector<LineData*>* lines)
{
    int count = 0;
    for (auto it = lines->begin(); it != lines->end(); it++) {
        auto line = *it;
        auto mne = Mnemonic::None;
        for (auto token = line->token.begin(); token != line->token.end(); token++) {
            if (token->first == TokenType::Label || token->first == TokenType::LabelAt) {
                break; // ラベル行なら変換しない
            } else if (token->first == TokenType::Mnemonic) {
                mne = mnemonicTable[token->second];
                if (mne == Mnemonic::DB || mne == Mnemonic::DW) {
                    break; // DB, DW なら変換しない
                }
            } else if (token->first == TokenType::String) {
                token->first = TokenType::LabelJump;
                // 定義済みの無名ラベルに同じ文字列があればそれを参照しておく
                bool found = false;
                for (auto n = nonameLabels.begin(); n != nonameLabels.end(); n++) {
                    if (n->second == token->second) {
                        token->second = n->first;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    // 新しい無名ラベルを作成してそれを参照
                    auto label = "$" + std::to_string(count++);
                    nonameLabels[label] = token->second;
                    token->second = label;
                }
            }
        }
    }
    // 無名ラベルとデータ行を展開
    for (int i = 0; i < count; i++) {
        // 無名ラベル行を追加
        auto newLabelLine = new LineData();
        auto label = "$" + std::to_string(i);
        newLabelLine->token.push_back(std::make_pair(TokenType::Label, label));
        lines->push_back(newLabelLine);
        labelTable[label] = newLabelLine;

        // データ行を追加
        auto newDataLine = new LineData();
        newDataLine->token.push_back(std::make_pair(TokenType::Mnemonic, "DB"));
        bool first = true;
        for (auto str = nonameLabels[label].c_str(); *str; str++) {
            if (!first) {
                newDataLine->token.push_back(std::make_pair(TokenType::Split, ","));
            }
            newDataLine->token.push_back(std::make_pair(TokenType::Numeric, std::to_string(*str)));
            first = false;
        }
        if (!first) {
            newDataLine->token.push_back(std::make_pair(TokenType::Split, ","));
        }
        newDataLine->token.push_back(std::make_pair(TokenType::Numeric, "0"));
        lines->push_back(newDataLine);
    }
}
