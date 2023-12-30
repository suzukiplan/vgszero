#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char* makeDump(int addr, char* data, int size)
{
    static char result[1024];
    char tmp[80];
    char ascii[80];
    sprintf(result, "%04X: ", addr);
    memset(ascii, 0, sizeof(ascii));
    for (int i = 0; i < size; i++) {
        if (i) {
            sprintf(tmp, ", %02X", (int)((unsigned char)(data[i])));
        } else {
            sprintf(tmp, "%02X", (int)((unsigned char)(data[i])));
        }
        strcat(result, tmp);
        ascii[i] = isprint(data[i]) ? data[i] : '.';
    }
    strcat(result, "  ");
    strcat(result, ascii);
    return result;
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        puts("usage: chkdif file1 file2");
        return 1;
    }
    FILE* f1 = fopen(argv[1], "rb");
    FILE* f2 = fopen(argv[2], "rb");
    if (!f1 || !f2) {
        if (f1) {
            fclose(f1);
        } else {
            printf("File not found: %s", argv[1]);
        }
        if (f2) {
            fclose(f2);
        } else {
            printf("File not found: %s", argv[2]);
        }
        return -1;
    }


    int addr = 0;
    bool error = true;
    do {
        char d1[16];
        char d2[16];
        int s1 = (int)fread(d1, 1, 16, f1);
        int s2 = (int)fread(d2, 1, 16, f2);
        if (s1 < 1 && s2 < 1) {
            error = false;
            break;
        }
        char t1[1024];
        char t2[1024];
        strcpy(t1, makeDump(addr, d1, s1));
        strcpy(t2, makeDump(addr, d2, s2));

        if (0 != strcmp(t1, t2)) {
            puts("Detect difference");
            printf("File1: %s\n", t1);
            printf("File2: %s\n", t2);
            break;
        }
    } while(1);
    fclose(f1);
    fclose(f2);
    return error ? -1 : 0;
}