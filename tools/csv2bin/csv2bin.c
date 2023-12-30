#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char* argv[])
{
    char buf[65536];
    FILE* in;
    FILE* out;
    size_t size = 0;

    if (argc < 3) {
        puts("usage: csv2bin input.csv output.bin");
        return 1;
    }

    if (NULL == (in = fopen(argv[1], "rt"))) {
        puts("cannot open csv file");
        return 1;
    }

    if (NULL == (out = fopen(argv[2], "wb"))) {
        puts("cannot open output binary file");
        fclose(in);
        return 1;
    }

    while (fgets(buf, sizeof(buf), in)) {
        char* cp = buf;
        unsigned char bin[1];
        while (*cp) {
            while (*cp && !isdigit(*cp)) cp++;
            if (!*cp) break;
            bin[0] = (unsigned char)atoi(cp);
            fwrite(bin, 1, 1, out);
            size++;
            while (isdigit(*cp)) cp++;
        }
    }
    printf("wrote %ld byte(s)\n", size);
    fclose(out);
    fclose(in);
    return 0;
}