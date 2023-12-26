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

    // TODO: wav -> pcm raw data convert

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