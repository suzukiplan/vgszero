#include "common.h"

void replace_assignment(LineData* line)
{
    std::vector<std::pair<TokenType, std::string>> left;
    std::vector<std::pair<TokenType, std::string>> right;
    bool isLeft = true;
    TokenType assignment = TokenType::None;
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->first == TokenType::Equal ||
            it->first == TokenType::EqualAnd ||
            it->first == TokenType::EqualMinus ||
            it->first == TokenType::EqualOr ||
            it->first == TokenType::EqualPlus ||
            it->first == TokenType::EqualXor) {
            if (assignment != TokenType::None) {
                line->error = true;
                line->errmsg = "Multiple assignments are described.";
                return;
            }
            assignment = it->first;
            isLeft = false;
            continue;
        }
        if (isLeft) {
            left.push_back(std::make_pair(it->first, it->second));
        } else {
            right.push_back(std::make_pair(it->first, it->second));
        }
    }
    if (assignment == TokenType::None) {
        return;
    }
    if (left.empty() || right.empty()) {
        line->error = true;
        line->errmsg = "Illegal assignment expression without left or right side.";
        return;
    }
    line->token.clear();
    std::pair<TokenType, std::string> mne;
    switch (assignment) {
        case TokenType::Equal:
            mne = std::make_pair(TokenType::Other, "LD");
            break;
        case TokenType::EqualAnd:
            mne = std::make_pair(TokenType::Other, "AND");
            break;
        case TokenType::EqualMinus:
            mne = std::make_pair(TokenType::Other, "SUB");
            break;
        case TokenType::EqualOr:
            mne = std::make_pair(TokenType::Other, "OR");
            break;
        case TokenType::EqualPlus:
            mne = std::make_pair(TokenType::Other, "ADD");
            break;
        case TokenType::EqualXor:
            mne = std::make_pair(TokenType::Other, "XOR");
            break;
        default:
            puts("logic error");
            exit(-1);
    }
    line->token.insert(line->token.begin(), mne);
    line->token.insert(line->token.end(), left.begin(), left.end());
    line->token.insert(line->token.end(), std::make_pair<TokenType, std::string>(TokenType::Split, ","));
    line->token.insert(line->token.end(), right.begin(), right.end());
    line->isAssignmnt = true;
}
