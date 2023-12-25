#include <stdio.h>
#include <string.h>

int writeFile(FILE* out, const char* inputFile, char* reason)
{
    char buf[8192];
    FILE* in = fopen(inputFile, "rb");
    long size;
    if (!in) {
        strcpy(reason, "cannot open");
        return -1;
    }
    fseek(in, 0, SEEK_END);
    size = ftell(in);
    fseek(in, 0, SEEK_SET);
    if (size < 1) {
        sprintf(reason, "invalid file size (%ld)", size);
        fclose(in);
        return -1;
    }
    while (0 < size) {
        memset(buf, 0, sizeof(buf));
        fread(buf, 1, 8192 < size ? 8192 : size, in);
        fwrite(buf, 1, sizeof(buf), out);
        size -= 8192;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    FILE* out;
    int i;
    char reason[80];
    if (argc < 3) {
        puts("usage: makerom output input1 input2 ... input256");
        return 1;
    }
    out = fopen(argv[1], "wb");
    if (!out) {
        printf("Cannot open file: %s\n", argv[1]);
        return 255;
    }
    for (i = 2; i < argc; i++) {
        if (writeFile(out, argv[i], reason)) {
            printf("Write file error: file = %s, error = %s\n", argv[1], reason);
            fclose(out);
            return 255;
        }
    }
    fclose(out);
    return 0;
}