#include <stdio.h>
#include <stdlib.h>

#include "vgsmml.h"

int vgsftv(const void* notes, size_t notesSize, void** ftv, size_t* ftvSize);

int main(int argc, char* argv[])
{
    FILE* fp;
    struct VgsBgmData* bgm;
    struct VgsMmlErrorInfo err;
    void* ftv;
    size_t ftvSize;

    if (argc < 3) {
        puts("usage: vgs2mml mml-file bgm-file");
        return 1;
    }
    bgm = vgsmml_compile_from_file(argv[1], &err);
    if (NULL == bgm) {
        if (err.line) {
            printf("error(%d) line=%d: %s\n", err.code, err.line, err.message);
        } else {
            printf("error(%d) %s\n", err.code, err.message);
        }
        return 2;
    }
    printf("bgm-size: %ld bytes\n", bgm->size);

    vgsftv(bgm->data, bgm->size, &ftv, &ftvSize);

    fp = fopen(argv[2], "wb");
    if (NULL == fp) {
        puts("file open error.");
        return 3;
    }
    if (ftvSize != fwrite(ftv, 1, ftvSize, fp)) {
        puts("file write error.");
        fclose(fp);
        return 4;
    }
    fclose(fp);
    vgsmml_free_bgm_data(bgm);
    free(ftv);
    return 0;
}
