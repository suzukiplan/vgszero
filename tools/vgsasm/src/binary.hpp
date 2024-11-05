/**
 * Z80 Assembler for VGS-Zero
 * Copyright (c) 2024, Yoji Suzuki.
 * License under GPLv3: https://github.com/suzukiplan/vgsasm/blob/master/LICENSE.txt
 */
#pragma once
#include "common.h"

void binary_parse(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->second == "#BINARY") {
            if (it != line->token.begin()) {
                line->error = true;
                line->errmsg = "#binary must appear at the beginning of the line.";
                return;
            }
            it->first = TokenType::Binary;
            if (it + 1 == line->token.end() || (it + 1)->first != TokenType::String) {
                line->error = true;
                line->errmsg = "#binary syntax error.";
                return;
            }
            it->second = (it + 1)->second;
            it++;
            it->first = TokenType::Delete;
            auto path = it->second.c_str();

            int offset = 0;
            int size = 0;
            bool sizeSpecified = false;
            if (it + 1 != line->token.end() && (it + 1)->first == TokenType::Split &&
                it + 2 != line->token.end() && (it + 2)->first == TokenType::Numeric) {
                offset = atoi((it + 2)->second.c_str());
                if (offset < 0) {
                    line->error = true;
                    line->errmsg = "#binary invalid offset: " + std::to_string(offset);
                    return;
                }
                (it + 1)->first = TokenType::Delete;
                (it + 2)->first = TokenType::Delete;
                it += 2;
                if (it + 1 != line->token.end() && (it + 1)->first == TokenType::Split &&
                    it + 2 != line->token.end() && (it + 2)->first == TokenType::Numeric) {
                    size = atoi((it + 2)->second.c_str());
                    sizeSpecified = true;
                    if (size < 1) {
                        line->error = true;
                        line->errmsg = "#binary invalid size: " + std::to_string(size);
                        return;
                    }
                    (it + 1)->first = TokenType::Delete;
                    (it + 2)->first = TokenType::Delete;
                    it += 2;
                } else if (it + 1 != line->token.end()) {
                    if ((it + 1)->first == TokenType::Split) {
                        if (it + 2 != line->token.end()) {
                            line->error = true;
                            line->errmsg = "Unexpected token: " + (it + 2)->second;
                        } else {
                            line->error = true;
                            line->errmsg = "#binary size value is not specified.";
                        }
                    } else {
                        line->error = true;
                        line->errmsg = "Unexpected token: " + (it + 1)->second;
                    }
                    return;
                }
            } else if (it + 1 != line->token.end()) {
                if ((it + 1)->first == TokenType::Split) {
                    if (it + 2 != line->token.end()) {
                        line->error = true;
                        line->errmsg = "Unexpected token: " + (it + 2)->second;
                    } else {
                        line->error = true;
                        line->errmsg = "#binary offset value is not specified.";
                    }
                } else {
                    line->error = true;
                    line->errmsg = "Unexpected token: " + (it + 1)->second;
                }
                return;
            }

            char basePath[4096];
            if ('/' != path[0]) {
                strcpy(basePath, line->path.c_str());
                char* cp = strrchr(basePath, '/');
                if (cp) {
                    cp++;
                    *cp = 0;
                    strcat(basePath, path);
                } else {
                    strcpy(basePath, path);
                }
                path = basePath;
            }

            auto fp = fopen(path, "rb");
            if (!fp) {
                line->error = true;
                line->errmsg = "#binary file open error: ";
                line->errmsg += path;
                return;
            }

            if (!sizeSpecified) {
                fseek(fp, 0, SEEK_END);
                size = ftell(fp);
                size -= offset;
            }

            if (fseek(fp, offset, SEEK_SET) < 0) {
                fclose(fp);
                line->error = true;
                line->errmsg = "#binary seek failed: " + std::to_string(offset);
                return;
            }

            if (size < 0) {
                line->error = true;
                line->errmsg = "#binary specified file size is smaller than the offset value.";
                fclose(fp);
                return;
            }

            if (0 < size) {
                auto buf = (uint8_t*)malloc(size);
                if (!buf) {
                    line->error = true;
                    line->errmsg = "No memory.";
                    fclose(fp);
                    return;
                }
                if (size != fread(buf, 1, size, fp)) {
                    line->error = true;
                    line->errmsg = "Cannot read " + std::to_string(size) + " bytes.";
                    free(buf);
                    fclose(fp);
                    return;
                }
                for (int i = 0; i < (int)size; i++) {
                    line->machine.push_back(buf[i]);
                }
                free(buf);
            }

            fclose(fp);
        }
    }
}
