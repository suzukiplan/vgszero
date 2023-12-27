#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

class Binary
{
  public:
    void* data;
    size_t size;
    Binary(void* data_, size_t size_)
    {
        this->data = data_;
        this->size = size_;
    }
};

static Binary* loadBinary(const char* path)
{
    FILE* fp = fopen(path, "rb");
    if (!fp) {
        printf("File open failed: %s\n", path);
        exit(-1);
    }
    fseek(fp, 0, SEEK_END);
    size_t size = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    void* result = malloc(size);
    if (size != fread(result, 1, size, fp)) {
        fclose(fp);
        free(result);
        printf("File read failed: %s\n", path);
        exit(-1);
    }
    fclose(fp);
    return new Binary(result, size);
}

int main(int argc, char* argv[])
{
    const char* output = nullptr;
    Binary* game = nullptr;
    Binary* bgm = nullptr;
    Binary* se = nullptr;
    bool error = false;

    for (int i = 1; i < argc; i++) {
        if ('-' != argv[i][0]) {
            error = true;
            break;
        }
        if (argc <= i + 1) {
            error = true;
            break;
        }
        switch (tolower(argv[i][1])) {
            case 'o': output = argv[++i]; break;
            case 'r': game = loadBinary(argv[++i]); break;
            case 'b': bgm = loadBinary(argv[++i]); break;
            case 's': se = loadBinary(argv[++i]); break;
            default: error = true;
        }
    }

    if (error || !output || !game) {
        puts("makepkg  -o /path/to/output.pkg");
        puts("         -r /path/to/game.rom");
        puts("        [-b /path/to/bgm.dat]");
        puts("        [-s /path/to/se.dat]");
        return 1;
    }

    FILE* fp = fopen(output, "wb");
    if (!fp) {
        printf("File open failed: %s\n", output);
        return -1;
    }
    size_t totalSize = 0;

    // write eyecatch
    if (8 != fwrite("VGS0PKG", 1, 8, fp)) {
        fclose(fp);
        printf("File write failed: %s\n", output);
        return -1;
    }
    totalSize += 8;

    // write game.rom chunk
    int size = (int)game->size;
    if (4 != fwrite(&size, 1, 4, fp)) {
        puts("File write error");
        exit(-1);
    }
    totalSize += 4;
    if (size != fwrite(game->data, 1, size, fp)) {
        puts("File write error");
        exit(-1);
    }
    totalSize += size;

    // write bgm chunk
    size = bgm ? (int)bgm->size : 0;
    if (4 != fwrite(&size, 1, 4, fp)) {
        puts("File write error");
        exit(-1);
    }
    totalSize += 4;
    if (0 < size) {
        if (size != fwrite(bgm->data, 1, size, fp)) {
            puts("File write error");
            exit(-1);
        }
        totalSize += size;
    }

    // write se chunk
    size = se ? (int)se->size : 0;
    if (4 != fwrite(&size, 1, 4, fp)) {
        puts("File write error");
        exit(-1);
    }
    totalSize += 4;
    if (0 < size) {
        if (size != fwrite(se->data, 1, size, fp)) {
            puts("File write error");
            exit(-1);
        }
        totalSize += size;
    }

    // total size check
    fclose(fp);
    if (16 * 1024 * 1024 < totalSize) {
        puts("Error: total size is over than 16MB");
        exit(-1);
    }

    return 0;
}