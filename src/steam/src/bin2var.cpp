#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if (argc < 4) {
        fprintf(stderr, "bin2var /path/to/binary.rom /path/to/gamepkg.c /path/to/gamepkg.h\n");
        return -1;
    }
    FILE* fp = fopen(argv[1], "rb");
    if (!fp) {
        fprintf(stderr, "file open error: %s\n", argv[1]);
        return -1;
    }
    FILE* fpC = fopen(argv[2], "wt");
    if (!fpC) {
        fprintf(stderr, "file open error: %s\n", argv[2]);
        fclose(fp);
        return -1;
    }
    FILE* fpH = fopen(argv[3], "wt");
    if (!fpH) {
        fprintf(stderr, "file open error: %s\n", argv[3]);
        fclose(fpC);
        fclose(fp);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    int size = (int)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    fprintf(fpH, "#pragma once\n");
    fprintf(fpH, "extern \"C\" {\n");
    fprintf(fpH, "    extern const unsigned char gamepkg[%d];\n", size);
    fprintf(fpH, "};\n");
    fclose(fpH);
    fprintf(fpC, "const unsigned char gamepkg[%d] = {\n", size);
    bool firstLine = true;
    while (1) {
        unsigned char buf[16];
        int readSize = (int)fread(buf, 1, sizeof(buf), fp);
        if (readSize < 1) {
            fprintf(fpC, "\n");
            break;
        }
        if (firstLine) {
            firstLine = false;
        } else {
            fprintf(fpC, ",\n");
        }
        fprintf(fpC, "    ");
        for (int i = 0; i < readSize; i++) {
            if (i) {
                fprintf(fpC, ", 0x%02X", buf[i]);
            } else {
                fprintf(fpC, "0x%02X", buf[i]);
            }
        }
    }
    fprintf(fpC, "};\n");
    fclose(fp);
    return 0;
}
