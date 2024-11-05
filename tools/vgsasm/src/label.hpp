/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"
#include "mnemonic.h"

static std::string lastLabel = "";
static std::map<std::string, std::string> nonameLabels;

LineData* label_parse(LineData* line)
{
    if (line->token.empty()) {
        return nullptr;
    }
    auto token = &line->token[0];
    if (token->first != TokenType::Other || token->second.empty()) {
        return nullptr;
    }

    auto str = token->second.c_str();
    std::string label = "";
    if (str[0] == '.') {
        token->first = TokenType::Label;
        label = str + 1;
        lastLabel = label;
        token->second = label;
    } else if (str[strlen(str) - 1] == ':') {
        token->first = TokenType::Label;
        label = token->second.substr(0, token->second.length() - 1);
        lastLabel = label;
        token->second = label;
    } else if (str[0] == '@') {
        token->first = TokenType::LabelAt;
        label = str + 1;
        label += "@" + lastLabel;
        token->second = label;
    }
    if (label.empty()) {
        return nullptr;
    }
    if (labelTable.find(label) != labelTable.end()) {
        line->error = true;
        line->errmsg = "Duplicate label: " + label;
        return nullptr;
    }

    // ラベルの禁則チェック
    nametable_add(label, line);
    if (line->error) {
        return nullptr;
    }
    token->second = label;
    labelTable[label] = line;

    // ラベル以降のトークンがあれば次の行に移す
    if (line->token.size() < 2) {
        return nullptr; // ない
    }
    auto newLine = new LineData(line);
    line->token.clear();
    line->token.push_back(newLine->token[0]);
    newLine->token.erase(newLine->token.begin());
    return newLine;
}

void label_parse_jump(LineData* line)
{
    static auto lastLabel = line->token.end();
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::Label) {
            lastLabel = it;
        } else if (it->first == TokenType::Other) {
            std::string labelName;
            bool errorIfNotFound;
            auto str = it->second.c_str();
            if ('@' == str[0] && 0 != str[1]) {
                if (lastLabel == line->token.end()) {
                    line->error = true;
                    line->errmsg = "Unknown label specified: " + it->second;
                    return;
                }
                labelName = &str[1];
                labelName += "@" + lastLabel->second;
                errorIfNotFound = true;
            } else {
                labelName = it->second;
                errorIfNotFound = false;
            }
            auto label = labelTable.find(labelName);
            if (label != labelTable.end()) {
                it->first = TokenType::LabelJump;
                it->second = labelName;
            } else if (errorIfNotFound) {
                line->error = true;
                line->errmsg = "Unknown label specified: " + it->second;
                return;
            } else {
                // @ が含まれる場合はチェック
                auto at = labelName.find("@");
                if (-1 != at && labelName != "@") {
                    auto atLeft = labelName.substr(0, at);
                    auto atRight = labelName.substr(at + 1);
                    if (labelTable.end() == labelTable.find(atRight)) {
                        line->error = true;
                        line->errmsg = "Label `" + atRight + "` is undefined.";
                        return;
                    } else {
                        line->error = true;
                        line->errmsg = "Label `@" + atLeft + "` is undefined in `" + atRight + "`.";
                        return;
                    }
                }
            }
        }
    }
}

void label_extract_anonymous(std::vector<LineData*>* lines)
{
    int count = 0;
    int lineNumber = 0;
    for (auto it = lines->begin(); it != lines->end(); it++) {
        lineNumber++;
        auto line = *it;
        for (auto token = line->token.begin(); token != line->token.end(); token++) {
            if (token->first == TokenType::Other && token->second == "@") {
                token->first = TokenType::Delete;
                token++;
                if (token == line->token.end()) {
                    line->error = true;
                    line->errmsg = "Anonymous label syntax error. (no token)";
                    break;
                }
                if (token->first != TokenType::Plus && token->first != TokenType::Minus) {
                    line->error = true;
                    line->errmsg = "Anonymous label syntax error. (invalid token: " + token->second + ")";
                    break;
                }
                auto isPlus = token->first == TokenType::Plus;
                token->first = TokenType::Delete;
                token++;
                if (token == line->token.end()) {
                    line->error = true;
                    line->errmsg = "Anonymous label syntax error. (no numeric specified)";
                    break;
                }
                token->first = TokenType::LabelJump;
                auto jumpNum = token->second.c_str();
                for (int i = 0; jumpNum[i]; i++) {
                    if (!isdigit(jumpNum[i])) {
                        line->error = true;
                        line->errmsg = "Unexpected symbol specified: " + token->second;
                        break;
                    }
                }
                if (line->error) {
                    break;
                }
                if (token + 1 != line->token.end()) {
                    line->error = true;
                    line->errmsg = "Unexpected symbol specified: " + (token + 1)->second;
                    break;
                }
                auto jump = atoi(jumpNum);
                auto label = (isPlus ? "$@+" : "$@-") + std::to_string(jump) + "#" + std::to_string(count++);
                token->second = label;
                for (int i = 0; i < jump; i++) {
                    if ((it == lines->begin() && !isPlus) || (it == lines->end() && isPlus)) {
                        line->error = true;
                        line->errmsg = "Anonymous label position ";
                        line->errmsg += isPlus ? "overflow." : "underflow";
                        break;
                    }
                    it += isPlus ? 1 : -1;
                    // 禁則条件やスキップ行をチェック
                    if (it != lines->begin() && it != lines->end()) {
                        if ((*it)->token.empty()) {
                            it += isPlus ? 1 : -1;
                        } else if ((*it)->token[0].first == TokenType::ScopeBegin || (*it)->token[0].first == TokenType::ScopeEnd) {
                            line->error = true;
                            line->errmsg = "Anonymous labels cannot cross scopes.";
                            break;
                        } else if ((*it)->token[0].first == TokenType::Label || (*it)->token[0].first == TokenType::LabelAt) {
                            line->error = true;
                            line->errmsg = "Anonymous labels cannot cross another label: " + (*it)->token[0].second;
                            break;
                        } else if ((*it)->token[0].second.c_str()[0] == '#') {
                            line->error = true;
                            line->errmsg = "Anonymous labels cannot cross " + (*it)->token[0].second + ".";
                            break;
                        }
                    }
                }
                if (!line->error) {
                    auto newLine = new LineData();
                    newLine->token.push_back(std::make_pair(TokenType::LabelAt, label));
                    lines->insert(it, newLine);
                    labelTable[label] = newLine;
                    it = lines->begin();
                }
            }
        }
    }
}
