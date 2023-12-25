#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

class Binary {
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
    if (argc < 3) {
        puts("makebgm bgm.dat song1.bgm [song2.bgm [song3.bgm...]]");
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

    // write count
    int size = (int)input.size();
    printf("Count of song: %d\n", size);
    if (sizeof(size) != fwrite(&size, 1, sizeof(size), fp)) {
        fclose(fp);
        printf("File write failed: %s\n", output);
        return -1;
    }

    // write size
    puts("Write size data.");
    for (Binary* song : input) {
        size = song->size;
        if (sizeof(size) != fwrite(&size, 1, sizeof(size), fp)) {
            fclose(fp);
            printf("File write failed: %s\n", output);
            return -1;
        }
    }

    // write data
    for (Binary* song : input) {
        printf("Write song data (%d bytes)\n", (int)song->size);
        if (song->size != fwrite(song->data, 1, song->size, fp)) {
            fclose(fp);
            printf("File write failed: %s\n", output);
            return -1;
        }
    }

    return 0;
}