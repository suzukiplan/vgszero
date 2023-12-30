/* 8bit Bitmap to FCS80 pattern table */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 情報ヘッダ */
struct DatHead {
    int isize;             /* 情報ヘッダサイズ */
    int width;             /* 幅 */
    int height;            /* 高さ */
    unsigned short planes; /* プレーン数 */
    unsigned short bits;   /* 色ビット数 */
    unsigned int ctype;    /* 圧縮形式 */
    unsigned int gsize;    /* 画像データサイズ */
    int xppm;              /* X方向解像度 */
    int yppm;              /* Y方向解像度 */
    unsigned int cnum;     /* 使用色数 */
    unsigned int inum;     /* 重要色数 */
};

int main(int argc, char* argv[])
{
    FILE* fpR = NULL;
    FILE* fpW = NULL;
    int rc = 0;
    char fh[14];
    unsigned int pal256[256];
    unsigned int pal16[16];
    struct DatHead dh;
    unsigned char bh, bl;
    unsigned char mh[4];
    int i, j, k, y, x, a;
    char bmp[16384];
    char chr[8192];

    /* 引数チェック */
    rc++;
    if (argc < 3) {
        fprintf(stderr, "usage: bmp2chr input.bmp output.chr [palette.c]\n");
        goto ENDPROC;
    }

    /* 読み込みファイルをオープン */
    rc++;
    if (NULL == (fpR = fopen(argv[1], "rb"))) {
        fprintf(stderr, "ERROR: Could not open: %s\n", argv[1]);
        goto ENDPROC;
    }

    /* ファイルヘッダを読み込む */
    rc++;
    if (sizeof(fh) != fread(fh, 1, sizeof(fh), fpR)) {
        fprintf(stderr, "ERROR: Invalid file header.\n");
        goto ENDPROC;
    }

    /* 先頭2バイトだけ読む */
    rc++;
    if (strncmp(fh, "BM", 2)) {
        fprintf(stderr, "ERROR: Inuput file is not bitmap.\n");
        goto ENDPROC;
    }

    /* 情報ヘッダを読み込む */
    rc++;
    if (sizeof(dh) != fread(&dh, 1, sizeof(dh), fpR)) {
        fprintf(stderr, "ERROR: Invalid bitmap file header.\n");
        goto ENDPROC;
    }

    printf("INPUT: width=%d, height=%d, bits=%d(%d), cmp=%d\n", dh.width, dh.height, (int)dh.bits, dh.cnum, dh.ctype);

    /* 128x128でなければエラー扱い */
    rc++;
    if (128 != dh.width || 128 != dh.height) {
        fprintf(stderr, "ERROR: Invalid input bitmap size. (128x128 only)");
        goto ENDPROC;
    }

    /* 8ビットカラーと4ビットカラー以外は弾く */
    rc++;
    if (8 != dh.bits && 4 != dh.bits) {
        fprintf(stderr, "ERROR: This program supports only 8bit color.\n");
        goto ENDPROC;
    }

    /* 無圧縮以外は弾く */
    rc++;
    if (dh.ctype) {
        fprintf(stderr, "ERROR: This program supports only none-compress type.\n");
        goto ENDPROC;
    }

    /* パレットを読み飛ばす */
    rc++;
    if (dh.bits == 8) {
        if (sizeof(pal256) != fread(pal256, 1, sizeof(pal256), fpR)) {
            fprintf(stderr, "ERROR: Could not read palette data.\n");
            goto ENDPROC;
        }
        /* 画像データを上下反転しながら読み込む */
        rc++;
        for (i = 127; 0 <= i; i--) {
            if (128 != fread(&bmp[i * 128], 1, 128, fpR)) {
                fprintf(stderr, "ERROR: Could not read graphic data.\n");
                goto ENDPROC;
            }
        }
        /* 色情報を mod 16 (0~15) にしておく*/
        for (i = 0; i < 16384; i++) {
            bmp[i] = bmp[i] & 0x0F;
        }
    } else {
        if (sizeof(pal16) != fread(pal16, 1, sizeof(pal16), fpR)) {
            fprintf(stderr, "ERROR: Could not read palette data.\n");
            goto ENDPROC;
        }
        /* 画像データを上下反転しながら読み込む */
        rc++;
        unsigned char tmp[64];
        for (i = 127; 0 <= i; i--) {
            if (64 != fread(&tmp, 1, 64, fpR)) {
                fprintf(stderr, "ERROR: Could not read graphic data.\n");
                goto ENDPROC;
            }
            for (int j = 0; j < 128; j++) {
                bmp[i * 128 + j] = j & 1 ? tmp[j / 2] & 0x0F : (tmp[j / 2] & 0xF0) >> 4;
            }
        }
    }

    /* Bitmap を CHR に変換 */
    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            for (j = 0; j < 8; j++) {
                for (i = 0; i < 4; i++) {
                    chr[y * 512 + x * 32 + j * 4 + i] = bmp[y * 1024 + x * 8 + j * 128 + i * 2];
                    chr[y * 512 + x * 32 + j * 4 + i] <<= 4;
                    chr[y * 512 + x * 32 + j * 4 + i] |= bmp[y * 1024 + x * 8 + j * 128 + i * 2 + 1];
                }
            }
        }
    }

    /* 書き込みファイルをオープンしてchrを書き込む */
    rc++;
    if (NULL == (fpW = fopen(argv[2], "wb"))) {
        fprintf(stderr, "ERROR: Could not open: %s\n", argv[2]);
        goto ENDPROC;
    }
    if (sizeof(chr) != fwrite(chr, 1, sizeof(chr), fpW)) {
        fprintf(stderr, "ERROR: File write error: %s\n", argv[2]);
        goto ENDPROC;
    }

    /* パレットデータの先頭16色をC形式で書き出す */
    if (4 <= argc) {
        fclose(fpW);
        if (NULL == (fpW = fopen(argv[3], "wt"))) {
            fprintf(stderr, "ERROR: Could not open: %s\n", argv[3]);
            goto ENDPROC;
        }
        unsigned int* pal = dh.bits == 8 ? pal256 : pal16;
        fprintf(fpW, "void init_palette(void)\n");
        fprintf(fpW, "{\n");
        for (int i = 0; i < (dh.bits == 8 ? 256 : 16); i++) {
            int r = (pal[i] & 0x00F80000) >> 19;
            int g = (pal[i] & 0x0000F800) >> 11;
            int b = (pal[i] & 0x000000F8) >> 3;
            fprintf(fpW, "    vgs0_palette_set(%d, %d, %d, %d, %d);\n", i / 16, i % 16, r, g, b);
        }
        fprintf(fpW, "}\n");
    }

    rc = 0;

    printf("succeed.\n");

    /* 終了処理 */
ENDPROC:
    if (fpR) {
        fclose(fpR);
    }
    if (fpW) {
        fclose(fpW);
    }
    return rc;
}