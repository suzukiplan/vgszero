#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

class Binary {
    public:
        unsigned char* data;
        size_t size;
        Binary(void* data_, size_t size_)
        {
            this->data = (unsigned char*)data_;
            this->size = size_;
        }
};

static Binary* loadBinary(const char* path)
{
    printf("Loading: %s\n", path);
    FILE* fp = fopen(path, "rb");
    if (!fp) {
        printf("File open failed: %s\n", path);
        exit(-1);
    }
    fseek(fp, 0, SEEK_END);
    size_t size = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* result = (char*)malloc(size);
    if (size != fread(result, 1, size, fp)) {
        fclose(fp);
        free(result);
        printf("File read failed: %s\n", path);
        exit(-1);
    }
    fclose(fp);

    if (0 != memcmp(result, "RIFF", 4)) {
        puts("Invalid format: RIFF not exist");
        exit(-1);
    }
    result += 4;
    size -= 4;

    int n;
    unsigned short s;

    memcpy(&n, result, 4);
    if (n != size - 4) {
        puts("Invalid format: SIZE");
        exit(-1);
    }
    result += 4;
    size -= 4;

    if (0 != memcmp(result, "WAVE", 4)) {
        puts("Invalid format: not WAVE format");
        exit(-1);
    }
    result += 4;
    size -= 4;

    // check chunks
    while (1) {
        if (0 == memcmp(result, "fmt ", 4)) {
            result += 4;
            size -= 4;
            memcpy(&n, result, 4);
            if (n != 16) {
                puts("Invalid format: not linear format");
                exit(-1);
            }
            result += 4;
            size -= 4;

            memcpy(&s, result, 2);
            if (s != 0x0001) {
                puts("Invalid format: unsupported compress type");
                exit(-1);
            }
            result += 2;
            size -= 2;

            short ch;
            memcpy(&ch, result, 2);
            result += 2;
            size -= 2;

            int rate;
            memcpy(&rate, result, 4);
            result += 4;
            size -= 4;

            int bps;
            memcpy(&bps, result, 4);
            result += 4;
            size -= 4;

            short bs;
            memcpy(&bs, result, 2);
            result += 2;
            size -= 2;

            short bits;
            memcpy(&bits, result, 2);
            result += 2;
            size -= 2;

            printf("- PCM Format: %dHz %dbits %dch (%d bytes/sec, %d bytes/sample)\n", rate, bits, ch, bps, bs);
            if (rate != 44100 || bits != 16 || ch != 2) {
                puts("Detected unsupported format (44100Hz/16bits/2ch only)");
                exit(-1);
            }
        } else if (0 == memcmp(result, "LIST", 4)) {
            result += 4;
            size -= 4;
            memcpy(&n, result, 4);
            result += 4 + n;
            size -= 4 + n;
        } else if (0 == memcmp(result, "data", 4)) {
            result += 4;
            size -= 4;
            break;
        } else {
            printf("Detected an invalid chunk: %c%c%c%c\n", result[0], result[1], result[2], result[3]);
            exit(-1);
        }
    }

    // validate data chunk size
    memcpy(&n, result, 4);
    if (n != size - 4) {
        puts("Invalid format: invalid sub chunk size");
        exit(-1);
    }
    printf("- PCM size: %d bytes\n", n);
    result += 4;
    size -= 4;

    return new Binary(result, size);
}


int main(int argc, char* argv[])
{
    if (argc < 3) {
        puts("makese se.dat se1.wav [se2.wav [se3.wav...]]");
        return 1;
    }
    const char* output = argv[1];
    std::vector<Binary*> input;
    for (int i = 2; i < argc; i++) {
        input.push_back(loadBinary(argv[i]));
    }
    FILE* fp = fopen(output, "wb");
    if (!fp) {
        printf("File open failed: %s\n", output);
        return -1;
    }

    // write eyecatch
    if (8 != fwrite("VGS0EFF", 1, 8, fp)) {
        fclose(fp);
        printf("File write failed: %s\n", output);
        return -1;
    }

    // write count
    int size = (int)input.size();
    printf("Count of sound-effect: %d\n", size);
    if (sizeof(size) != fwrite(&size, 1, sizeof(size), fp)) {
        fclose(fp);
        printf("File write failed: %s\n", output);
        return -1;
    }

    // write size
    puts("Write size data.");
    for (Binary* se : input) {
        size = se->size;
        if (sizeof(size) != fwrite(&size, 1, sizeof(size), fp)) {
            fclose(fp);
            printf("File write failed: %s\n", output);
            return -1;
        }
    }

    // write data
    for (Binary* se : input) {
        printf("Write sound-effect data (%d bytes)\n", (int)se->size);
        if (se->size != fwrite(se->data, 1, se->size, fp)) {
            fclose(fp);
            printf("File write failed: %s\n", output);
            return -1;
        }
    }

    return 0;
}