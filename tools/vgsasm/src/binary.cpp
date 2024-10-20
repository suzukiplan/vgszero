#include "common.h"

void parse_binary(LineData* line)
{
    for (auto it = line->token.begin(); it != line->token.end(); it++) {
        if (it->second == "#BINARY") {
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

            fseek(fp, 0, SEEK_END);
            auto size = ftell(fp);
            fseek(fp, 0, SEEK_SET);

            if (size < 0) {
                line->error = true;
                line->errmsg = "I/O error";
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
                    line->errmsg = "I/O error";
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
