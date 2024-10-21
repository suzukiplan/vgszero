#include "common.h"

std::vector<LineData*> readFile(const char* filePath)
{
    try {
        int lineNumber = 0;
        std::ifstream ifs(filePath, std::ios::binary);
        ifs.seekg(0, std::ios::end);
        auto sz = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::vector<char> buf(sz);
        ifs.read(buf.data(), sz);
        auto str = std::string(buf.data(), sz);
        int first = 0;
        int last = str.find_first_of('\n');
        std::vector<LineData*> result;
        while (first < str.size()) {
            std::string subStr(str, first, last - first);
            auto line = new LineData(filePath, ++lineNumber, subStr);

            // #include は他のプリプロセッサよりも優先して先に展開
            if (2 == line->token.size() && line->token[0].second == "#INCLUDE" && TokenType::String == line->token[1].first) {
                auto path = line->token[1].second.c_str();
                bool alreadyIncluded = false;
                for (auto includeFile : includeFiles) {
                    if (0 == strcmp(includeFile.c_str(), path)) {
                        alreadyIncluded = true;
                    }
                }
                if (!alreadyIncluded) {
                    includeFiles.push_back(path);
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
                    auto iLines = readFile(path);
                    if (iLines.empty()) {
                        line->error = true;
                        line->errmsg = "Cannot open \"";
                        line->errmsg += path;
                        line->errmsg += "\"";
                        result.push_back(line);
                    } else {
                        for (auto includeLine : iLines) {
                            result.push_back(includeLine);
                        }
                    }
                }
            } else {
                if (line->token.empty()) {
                    delete line;
                } else {
                    result.push_back(line);
                }
            }

            first = last + 1;
            last = str.find_first_of('\n', first);
            if (last == std::string::npos) {
                last = str.size();
            }
        }

        // スコープが閉じられているかチェック
        if (0 != scopeCount) {
            lastScopeBegin->error = true;
            lastScopeBegin->errmsg = "The scope `{` specified in this line is not closed with `}`.";
        }

        return result;
    } catch (...) {
        std::vector<LineData*> empty;
        return empty;
    }
}
