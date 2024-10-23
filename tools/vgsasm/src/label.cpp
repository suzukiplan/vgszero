#include "common.h"
#include "mnemonic.h"

static std::string lastLabel = "";
static std::map<std::string, std::string> nonameLabels;

LineData* parse_label(LineData* line)
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
    auto cp = label.c_str();
    if (*cp == '$') {
        line->error = true;
        line->errmsg = "Label names beginning with `$` are not allowed.";
        return nullptr;
    }

    addNameTable(label, line);
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

void parse_label_jump(LineData* line)
{
    static auto lastLabel = line->token.end();
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::Label) {
            lastLabel = it;
        } else if (it->first == TokenType::Other) {
            std::string labelName;
            bool errorIfNotFound;
            auto str = it->second.c_str();
            if ('@' == str[0]) {
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
            }
        }
    }
}

void extract_anonymous_label(std::vector<LineData*>* lines)
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
                auto jump = atoi(token->second.c_str());
                auto label = (isPlus ? "$@+" : "$@-") + std::to_string(jump) + "#" + std::to_string(count++);
                token->second = label;
                for (int i = 0; i < jump; i++) {
                    if (it == lines->begin() || it == lines->end()) {
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

// 文字列リテラルを探索して無名ラベルに置換
void extract_string_literal(std::vector<LineData*>* lines)
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
        newLabelLine->token.push_back(std::make_pair(TokenType::Label, label + ":"));
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
