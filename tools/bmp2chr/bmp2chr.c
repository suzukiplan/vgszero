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
    int pal[256];
    struct DatHead dh;
    unsigned char bh, bl;
    unsigned char mh[4];
    int i, j, k, y, x, a;
    char bmp[16384];
    char chr[8192];

    /* 引数チェック */
    rc++;
    if (argc < 3) {
        fprintf(stderr, "usage: bmp2chr input.bmp output.chr\n");
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

    /* 8ビットカラー以外は弾く */
    rc++;
    if (8 != dh.bits) {
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
    if (sizeof(pal) != fread(pal, 1, sizeof(pal), fpR)) {
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