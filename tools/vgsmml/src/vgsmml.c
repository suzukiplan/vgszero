/**
 * 東方BGM on VGS for RasberryPi Pico
 * License under GPLv3: https://github.com/suzukiplan/tohovgs-pico/blob/master/LICENSE.txt
 */

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <unistd.h>
#define __stdcall
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "vgsdec_internal.h"
#include "vgsmml.h"

#define HZ 22050

/* Macro */
struct MyMacro {
    char* name;
    char* value;
    struct MyMacro* next;
};

/* Channel record */
struct Channel {
    struct _NOTE note;
    struct Channel* next;
};

/* Channel table */
struct ChannelTbl {
    struct Channel* head;
    struct Channel* tail;
    int oct;   /* octave */
    int len;   /* length */
    int huten; /* length of huten */
    int per;   /* rate of key-on */
    int tone;  /* tone value */
    int vol;   /* volume */
};

/* MML table */
struct MML {
    struct ChannelTbl ch[6 + 1];
    int tempo;
    struct MyMacro* mhead;
    struct MyMacro* mtail;
    int kbase[6];
};

/* internal functions */
static struct VgsBgmData* phase3(char* buf, int* pos, struct VgsMmlErrorInfo* err);
static struct VgsBgmData* phase4(struct MML* mml, char* buf, int* pos, struct VgsMmlErrorInfo* err);
static struct VgsBgmData* phase5(struct Channel* ch0, struct VgsMmlErrorInfo* err);
static struct VgsBgmData* phase6(void* raw, size_t size, struct VgsMmlErrorInfo* err);
static void free_mml(struct MML* mml);
static void trimstring(char* src);
static int chkmname(char* name);
static int madd(struct MML* mml, struct VgsMmlErrorInfo* err, int line, char* name, char* value);
static int canl(struct MML* mml, struct VgsMmlErrorInfo* err, int line, int cn, char* fmt);
static int getval(char** str);
static int cadd(struct MML* mml, int cn, struct _NOTE* note);
static int merge(struct MML* mml, struct Channel* c2);

void __stdcall vgsmml_free_bgm_data(struct VgsBgmData* data)
{
    if (NULL == data) return;
    if (data->data) free(data->data);
    free(data);
}

struct VgsBgmData* __stdcall vgsmml_compile_from_file(const char* path, struct VgsMmlErrorInfo* err)
{
    struct VgsBgmData* result;
    char* data;
    FILE* fp;
    long size;

    if (NULL == err) return NULL;
    memset(err, 0, sizeof(struct VgsMmlErrorInfo));

    if (NULL == path) {
        strcpy(err->message, "invalid argument");
        err->code = VGSMML_ERR_INVALID;
        return NULL;
    }

    /* open MML file */
    if (NULL == (fp = fopen(path, "rb"))) {
        strcpy(err->message, "file open error");
        err->code = VGSMML_ERR_FILE_IO;
        return NULL;
    }

    /* get file size */
    if (-1 == fseek(fp, 0, SEEK_END)) {
        strcpy(err->message, "file seek error");
        err->code = VGSMML_ERR_FILE_IO;
        fclose(fp);
        return NULL;
    }
    size = ftell(fp);
    if (-1 == fseek(fp, 0, SEEK_SET)) {
        strcpy(err->message, "file seek error");
        err->code = VGSMML_ERR_FILE_IO;
        fclose(fp);
        return NULL;
    }
    if (size < 0) {
        strcpy(err->message, "invalid file size");
        err->code = VGSMML_ERR_FILE_IO;
        fclose(fp);
        return NULL;
    }
    if (-1 == fseek(fp, 0, SEEK_SET)) {
        strcpy(err->message, "file seek error");
        err->code = VGSMML_ERR_FILE_IO;
        fclose(fp);
        return NULL;
    }

    /* get text buffer */
    if (NULL == (data = malloc(size + 1))) {
        strcpy(err->message, "no memory");
        err->code = VGSMML_ERR_NO_MEMORY;
        fclose(fp);
        return NULL;
    }

    /* read */
    if (size != fread(data, 1, size, fp)) {
        strcpy(err->message, "file read error");
        err->code = VGSMML_ERR_FILE_IO;
        free(data);
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    ((char*)data)[size] = '\0';

    /* compile */
    result = vgsmml_compile_from_memory(data, size + 1, err);
    free(data);
    return result;
}

/* NOTE: YOU MUST SPECIFY THE NULL TERMINATED STRING TO THE DATA ARGUMENT */
struct VgsBgmData* __stdcall vgsmml_compile_from_memory(void* data, size_t size, struct VgsMmlErrorInfo* err)
{
    struct VgsBgmData* result;
    int nLine, cLine;
    int i;
    int* pos;
    char* buf = (char*)data;

    if (NULL == err) {
        return NULL;
    }
    if (NULL == data || size < 1) {
        strcpy(err->message, "invalid data size");
        err->code = VGSMML_ERR_INVALID;
        return NULL;
    }

    /* memory check */
    if ('\0' != buf[size - 1]) {
        strcpy(err->message, "needed specify the \'\\0\' terminated string to the data argument.");
        err->code = VGSMML_ERR_INVALID;
        return NULL;
    }

    /* count line */
    for (nLine = 1, i = 0; buf[i]; i++) {
        if ('\n' == buf[i]) nLine++;
    }

    /* allocate line head position buffer */
    if (NULL == (pos = (int*)malloc((nLine + 1) * sizeof(int)))) {
        strcpy(err->message, "no memory");
        err->code = VGSMML_ERR_NO_MEMORY;
        return NULL;
    }

    /* set line head position */
    pos[0] = 0;
    for (cLine = 0, i = 0; buf[i] && cLine < nLine; i++) {
        switch (buf[i]) {
            case '\r':
            case '#':
                buf[i] = '\0';
                break;
            case '\t':
                buf[i] = ' ';
                break;
            case '\n':
                buf[i] = '\0';
                cLine++;
                pos[cLine] = i + 1;
                break;
        }
    }
    pos[nLine] = -1;

    /* execute next phase */
    result = phase3(buf, pos, err);
    free(pos);
    return result;
}

/* compiles without memory destruction */
struct VgsBgmData* __stdcall vgsmml_compile_from_memory2(const void* data, size_t size, struct VgsMmlErrorInfo* err)
{
    struct VgsBgmData* result;
    char* buf;

    if (NULL == err) {
        return NULL;
    }
    if (NULL == data || size < 1) {
        strcpy(err->message, "invalid data size");
        err->code = VGSMML_ERR_INVALID;
        return NULL;
    }

    buf = (char*)malloc(size + 1);
    if (NULL == buf) {
        strcpy(err->message, "no memory");
        err->code = VGSMML_ERR_NO_MEMORY;
        return NULL;
    }
    memcpy(buf, data, size);
    buf[size] = '\0';

    result = vgsmml_compile_from_memory(buf, size + 1, err);
    free(buf);
    return result;
}

/* phase-3: create context & set initial value */
static struct VgsBgmData* phase3(char* buf, int* pos, struct VgsMmlErrorInfo* err)
{
    int i;
    struct VgsBgmData* result;
    struct MML* mml;
    struct _NOTE es = {NTYPE_ENV1, 0, 0, 0, 1000};
    struct _NOTE ee = {NTYPE_ENV2, 0, 0, 0, 10000};
    struct _NOTE vo = {NTYPE_VOL, 0, 0, 0, 10};
    struct _NOTE mv = {NTYPE_MVOL, 0, 0, 0, 10};

    /* allocate mml context */
    mml = (struct MML*)malloc(sizeof(struct MML));
    if (NULL == mml) {
        strcpy(err->message, "no memory");
        err->code = VGSMML_ERR_NO_MEMORY;
        return NULL;
    }
    memset(mml, 0, sizeof(struct MML));

    /* set initial values */
    if (cadd(mml, 0, &mv)) {
        strcpy(err->message, "no memory");
        err->code = VGSMML_ERR_NO_MEMORY;
        free_mml(mml);
        return NULL;
    }
    mml->tempo = (HZ * 60) / 120 * 4;
    for (i = 0; i < 6; i++) {
        mml->ch[i].oct = 4;
        mml->ch[i].len = 4;
        mml->ch[i].tone = 0;
        mml->ch[i].per = 80;
        if (cadd(mml, i, &es) || cadd(mml, i, &ee) || cadd(mml, i, &vo)) {
            strcpy(err->message, "no memory");
            err->code = VGSMML_ERR_NO_MEMORY;
            free_mml(mml);
            return NULL;
        }
    }

    /* execute next phase */
    result = phase4(mml, buf, pos, err);
    free_mml(mml);
    return result;
}

/* release mml context */
static void free_mml(struct MML* mml)
{
    /* release channel table */
    {
        struct Channel* cur;
        struct Channel* next;
        int i;
        for (i = 0; i < 7; i++) {
            for (cur = mml->ch[i].head; cur; cur = next) {
                next = cur->next;
                free(cur);
            }
        }
    }
    /* release macro table */
    {
        struct MyMacro* cur;
        struct MyMacro* next;
        for (cur = mml->mhead; cur; cur = next) {
            next = cur->next;
            if (cur->name) free(cur->name);
            if (cur->value) free(cur->value);
            free(cur);
        }
    }
    /* release mml table */
    free(mml);
}

/* phase-4: compile */
static struct VgsBgmData* phase4(struct MML* mml, char* buf, int* pos, struct VgsMmlErrorInfo* err)
{
    char* cp;
    struct _NOTE note;
    struct Channel* ch;
    int i;

    for (i = 0; pos[i] != -1; i++) {
        trimstring(&buf[pos[i]]);
        switch (buf[pos[i]]) {
            case '\0': /* empty */
                break;
            case '$': /* macro */
                if (NULL == (cp = strchr(&buf[pos[i]], ' '))) {
                    strcpy(err->message, "invalid macro syntax");
                    err->line = i + 1;
                    err->code = VGSMML_ERR_SYNTAX_MACRO;
                    return NULL;
                }
                *cp = '\0';
                cp++;
                trimstring(cp);
                if (chkmname(&buf[pos[i] + 1])) {
                    strcpy(err->message, "invalid macro name");
                    err->line = i + 1;
                    err->code = VGSMML_ERR_SYNTAX_MACRO;
                    return NULL;
                }
                if (madd(mml, err, i + 1, &buf[pos[i] + 1], cp)) {
                    return NULL;
                }
                break;
            case 'c': /* channel */
            case 'C':
                if (('h' != buf[pos[i] + 1] && 'H' != buf[pos[i] + 1]) || !isdigit(buf[pos[i] + 2])) {
                    strcpy(err->message, "unknown statement");
                    err->line = i + 1;
                    err->code = VGSMML_ERR_SYNTAX_UNKNOWN;
                    return NULL;
                }
                if (' ' == buf[pos[i] + 3]) {
                    if (canl(mml, err, i + 1, buf[pos[i] + 2] - '0', &buf[pos[i] + 4])) return NULL;
                } else {
                    if (canl(mml, err, i + 1, buf[pos[i] + 2] - '0', &buf[pos[i] + 3])) return NULL;
                }
                break;
            default:
                strcpy(err->message, "unknown statement");
                err->line = i + 1;
                err->code = VGSMML_ERR_SYNTAX_UNKNOWN;
                return NULL;
        }
    }

    note.type = NTYPE_WAIT;
    note.op1 = 0;
    note.op2 = 0;
    note.op3 = 0;
    note.val = 0;
    for (i = 0; i < 6; i++) {
        if (cadd(mml, i, &note)) {
            strcpy(err->message, "no memory");
            err->code = VGSMML_ERR_NO_MEMORY;
            return NULL;
        }
    }

    for (i = 1; i < 6; i++) {
        if (merge(mml, mml->ch[i].head)) {
            strcpy(err->message, "no memory");
            err->code = VGSMML_ERR_NO_MEMORY;
            return NULL;
        }
    }

    for (i = 0, ch = mml->ch[0].head; ch; ch = ch->next, i++) {
        if (NTYPE_LABEL == ch->note.type) {
            break;
        }
    }
    if (ch) {
        for (; ch; ch = ch->next) {
            if (ch->note.type == NTYPE_WAIT && ch->note.val == 0) {
                break;
            }
        }
        if (ch) {
            ch->note.type = NTYPE_JUMP;
            ch->note.val = i;
        }
    }

    /* execute next phase */
    return phase5(mml->ch[0].head, err);
}

/* phase-5: make raw result */
static struct VgsBgmData* phase5(struct Channel* ch0, struct VgsMmlErrorInfo* err)
{
    struct VgsBgmData* result;
    struct Channel* cur;
    struct _NOTE* raw;
    size_t size;
    int count;

    /* get record count of channel 0 */
    for (count = 0, cur = ch0; cur; cur = cur->next) {
        count++;
    }

    /* allocate raw note data area */
    size = count * sizeof(struct _NOTE);
    raw = (struct _NOTE*)malloc(size);
    if (NULL == raw) {
        strcpy(err->message, "no memory");
        err->code = VGSMML_ERR_NO_MEMORY;
        return NULL;
    }

    /* copy ch0 to raw area */
    for (count = 0, cur = ch0; cur; cur = cur->next, count++) {
        memcpy(&raw[count], &cur->note, sizeof(struct _NOTE));
    }

    /* execute next phase */
    result = phase6(raw, size, err);
    free(raw);
    return result;
}

/* phase-6: make compressed result */
static struct VgsBgmData* phase6(void* raw, size_t size, struct VgsMmlErrorInfo* err)
{
    struct VgsBgmData* result;
#if 0
    int cmp_status;
    uLong src_len;
    uLong cmp_len;
    unsigned char* pCmp;
#endif

    result = (struct VgsBgmData*)malloc(sizeof(struct VgsBgmData));
    if (NULL == result) {
        strcpy(err->message, "no memory");
        err->code = VGSMML_ERR_NO_MEMORY;
        return NULL;
    }
#if 1 /* without compress */
    result->data = malloc(size);
    result->size = size;
    memcpy(result->data, raw, size);
    return result;
#else
    src_len = (uLong)size;
    cmp_len = compressBound(src_len);
    pCmp = (unsigned char*)malloc(cmp_len);
    if (NULL == pCmp) {
        free(result);
        strcpy(err->message, "no memory");
        err->code = VGSMML_ERR_NO_MEMORY;
        return NULL;
    }
    memset(pCmp, 0, cmp_len);
    cmp_status = compress(pCmp, &cmp_len, (const unsigned char*)raw, src_len);
    if (cmp_status != Z_OK) {
        free(pCmp);
        free(result);
        strcpy(err->message, "no memory");
        err->code = VGSMML_ERR_NO_MEMORY;
        return NULL;
    }

    result->data = pCmp;
    result->size = cmp_len;
    printf("compressed %lu -> %lu\n", size, cmp_len);
    return result;
#endif
}

static void trimstring(char* src)
{
    int i, j;
    int len;
    for (i = 0; ' ' == src[i]; i++)
        ;
    if (i) {
        for (j = 0; src[i] != '\0'; j++, i++) {
            src[j] = src[i];
        }
        src[j] = '\0';
    }
    for (len = (int)strlen(src) - 1; 0 <= len && ' ' == src[len]; len--) {
        src[len] = '\0';
    }
}

static int chkmname(char* name)
{
    int i;
    for (i = 0; name[i]; i++) {
        if (!isalpha(name[i]) && !isdigit(name[i])) {
            return -1;
        }
    }
    if (0 == i) {
        return -1;
    }
    return 0;
}

/* add a note to a channel */
static int cadd(struct MML* mml, int cn, struct _NOTE* note)
{
    struct Channel* ch;
    ch = (struct Channel*)malloc(sizeof(struct Channel));
    if (NULL == ch) {
        return -1;
    }
    memcpy(&(ch->note), note, sizeof(struct _NOTE));
    ch->next = NULL;
    if (NULL == mml->ch[cn].tail) {
        mml->ch[cn].tail = ch;
        mml->ch[cn].head = ch;
    } else {
        mml->ch[cn].tail->next = ch;
        mml->ch[cn].tail = ch;
    }
    return 0;
}

static int madd(struct MML* mml, struct VgsMmlErrorInfo* err, int line, char* name, char* value)
{
    struct MyMacro* mp;
    for (mp = mml->mhead; mp; mp = mp->next) {
        if (0 == strcmp(name, mp->name)) {
            strcpy(err->message, "duplicated variable name");
            err->code = VGSMML_ERR_SYNTAX_MACRO;
            err->line = line;
            return -1;
        }
    }
    mp = (struct MyMacro*)malloc(sizeof(struct MyMacro));
    if (NULL == mp) {
        strcpy(err->message, "no memory");
        err->code = VGSMML_ERR_NO_MEMORY;
        return -1;
    }

    mp->name = (char*)malloc(strlen(name) + 1);
    if (NULL == mp->name) {
        free(mp);
        strcpy(err->message, "no memory");
        err->code = VGSMML_ERR_NO_MEMORY;
        return -1;
    }
    strcpy(mp->name, name);

    mp->value = (char*)malloc(strlen(value) + 1);
    if (NULL == mp->value) {
        free(mp->name);
        free(mp);
        strcpy(err->message, "no memory");
        err->code = VGSMML_ERR_NO_MEMORY;
        return -1;
    }
    strcpy(mp->value, value);

    mp->next = NULL;

    if (NULL == mml->mtail) {
        mml->mhead = mp;
        mml->mtail = mp;
    } else {
        mml->mtail->next = mp;
        mml->mtail = mp;
    }
    return 0;
}

static int canl(struct MML* mml, struct VgsMmlErrorInfo* err, int line, int cn, char* fmt)
{
    int rc = 0;
    int i, j, k, l, m, n;
    char* cp;
    char* cp2;
    char* cbuf;
    char* cbuf2;
    struct MyMacro* mp;
    struct _NOTE note;

    trimstring(fmt);

    if (NULL == (cbuf = (char*)malloc(strlen(fmt) + 1))) {
        strcpy(err->message, "no memory");
        err->code = VGSMML_ERR_NO_MEMORY;
        return -1;
    }
    strcpy(cbuf, fmt);

    if (cn < 0 || 5 < cn) {
        strcpy(err->message, "invalid channel number");
        err->code = VGSMML_ERR_SYNTAX_CHANNEL;
        err->line = line;
        rc = 4;
        goto ENDPROC;
    }

    while (NULL != (cp = strchr(cbuf, '('))) {
        if (NULL == (cp2 = strchr(cp + 1, ')'))) {
            strcpy(err->message, "there is no \')\' corresponding to \'(\''");
            err->code = VGSMML_ERR_SYNTAX_CHANNEL;
            err->line = line;
            rc = 4;
            goto ENDPROC;
        }
        *cp = '\0';
        *cp2 = '\0';
        cp++;
        cp2++;
        for (mp = mml->mhead; mp; mp = mp->next) {
            if (0 == strcmp(cp, mp->name)) {
                break;
            }
        }
        if (NULL == mp) {
            strcpy(err->message, "undeclared variable was specified");
            err->code = VGSMML_ERR_SYNTAX_CHANNEL;
            err->line = line;
            rc = 4;
            goto ENDPROC;
        }
        i = (int)(strlen(cbuf) + strlen(mp->value) + strlen(cp2));
        if (NULL == (cbuf2 = (char*)malloc(i + 1))) {
            strcpy(err->message, "no memory");
            err->code = VGSMML_ERR_NO_MEMORY;
            rc = 255;
            goto ENDPROC;
        }
        strcpy(cbuf2, cbuf);
        strcat(cbuf2, mp->value);
        strcat(cbuf2, cp2);
        free(cbuf);
        cbuf = cbuf2;
    }

    trimstring(cbuf);
    while (NULL != (cp = strchr(cbuf, ' '))) {
        for (; *cp != '\0'; cp++) {
            *cp = *(cp + 1);
        }
    }

    for (cp = cbuf; *cp; cp++) {
        if ('a' <= *cp && *cp <= 'z') {
            (*cp) -= 'a' - 'A';
        }
    }

    for (cp = cbuf; *cp; cp++) {
        if ('A' <= *cp && *cp <= 'G') {
            n = mml->ch[cn].oct * 12;
            switch (*cp) {
                case 'C':
                    n -= 9;
                    break;
                case 'D':
                    n -= 7;
                    break;
                case 'E':
                    n -= 5;
                    break;
                case 'F':
                    n -= 4;
                    break;
                case 'G':
                    n -= 2;
                    break;
                case 'A':
                    ;
                    break;
                case 'B':
                    n += 2;
                    break;
            }
            for (; '+' == *(cp + 1) || '-' == *(cp + 1); cp++) {
                if ('+' == *(cp + 1)) {
                    n++;
                } else {
                    n--;
                }
            }
            n += mml->kbase[cn];
            if (n < 0) n = 0;
            if (84 < n) n = 84;
            cp++;
            i = getval(&cp);
            if (i < 0) {
                cp--;
                i = mml->ch[cn].len;
                j = mml->ch[cn].huten;
            } else {
                if ('.' == *(cp + 1)) {
                    cp++;
                    j = 1;
                } else {
                    j = 0;
                }
            }
            k = mml->tempo / i;
            if (j) {
                j = i * 2;
                k += mml->tempo / j;
            }
            while ('^' == *(cp + 1)) {
                cp += 2;
                i = getval(&cp);
                if (i < 0) {
                    cp--;
                    i = mml->ch[cn].len;
                    j = mml->ch[cn].huten;
                } else {
                    if ('.' == *(cp + 1)) {
                        cp++;
                        j = 1;
                    } else {
                        j = 0;
                    }
                }
                k += mml->tempo / i;
                if (j) {
                    j = i * 2;
                    k += mml->tempo / j;
                }
            }
            l = k * mml->ch[cn].per / 100;
            m = k - l;
            if (m == 0) {
                m++;
                l--;
            }
            note.type = NTYPE_KEYON;
            note.op1 = (unsigned char)cn;
            note.op2 = (unsigned char)mml->ch[cn].tone;
            note.op3 = (unsigned char)n;
            note.val = 0;
            if (0 != (rc = cadd(mml, cn, &note))) {
                strcpy(err->message, "no memory");
                err->code = VGSMML_ERR_NO_MEMORY;
                rc = 255;
                goto ENDPROC;
            }
            note.type = NTYPE_WAIT;
            note.op1 = 0;
            note.op2 = 0;
            note.op3 = 0;
            note.val = l;
            if (0 != (rc = cadd(mml, cn, &note))) {
                strcpy(err->message, "no memory");
                err->code = VGSMML_ERR_NO_MEMORY;
                rc = 255;
                goto ENDPROC;
            }
            note.type = NTYPE_KEYOFF;
            note.op1 = cn;
            note.op2 = 0;
            note.op3 = 0;
            note.val = 0;
            if (0 != (rc = cadd(mml, cn, &note))) {
                strcpy(err->message, "no memory");
                err->code = VGSMML_ERR_NO_MEMORY;
                rc = 255;
                goto ENDPROC;
            }
            note.type = NTYPE_WAIT;
            note.op1 = 0;
            note.op2 = 0;
            note.op3 = 0;
            note.val = m;
            if (0 != (rc = cadd(mml, cn, &note))) {
                strcpy(err->message, "no memory");
                err->code = VGSMML_ERR_NO_MEMORY;
                rc = 255;
                goto ENDPROC;
            }
        } else if ('R' == *cp) {
            cp++;
            i = getval(&cp);
            if (0 == i) {
                strcpy(err->message, "zero divide error");
                err->code = VGSMML_ERR_SYNTAX_CHANNEL;
                err->line = line;
                rc = 4;
                goto ENDPROC;
            } else if (i < 0) {
                cp--;
                i = mml->ch[cn].len;
                j = mml->ch[cn].huten;
            } else {
                if ('.' == *(cp + 1)) {
                    cp++;
                    j = 1;
                } else {
                    j = 0;
                }
            }
            k = mml->tempo / i;
            if (j) {
                j = i * 2;
                k += mml->tempo / j;
            }
            note.type = NTYPE_WAIT;
            note.op1 = 0;
            note.op2 = 0;
            note.op3 = 0;
            note.val = k;
            if (0 != (rc = cadd(mml, cn, &note))) {
                strcpy(err->message, "no memory");
                err->code = VGSMML_ERR_NO_MEMORY;
                rc = 255;
                goto ENDPROC;
            }
        } else if ('P' == *cp) {
            cp++;
            if (*cp == '-') {
                cp++;
                i = getval(&cp);
                if (i < 0) {
                    strcpy(err->message, "invalid value was specified to the P operand");
                    err->code = VGSMML_ERR_SYNTAX_CHANNEL;
                    err->line = line;
                    rc = 4;
                    goto ENDPROC;
                }
                note.type = NTYPE_PDOWN;
                note.op1 = cn;
                note.op2 = 0;
                note.op3 = 0;
                note.val = i;
                if (0 != (rc = cadd(mml, cn, &note))) {
                    strcpy(err->message, "no memory");
                    err->code = VGSMML_ERR_NO_MEMORY;
                    rc = 255;
                    goto ENDPROC;
                }
            } else {
                strcpy(err->message, "invalid value was specified to the P operand");
                err->code = VGSMML_ERR_SYNTAX_CHANNEL;
                err->line = line;
                rc = 4;
                goto ENDPROC;
            }
        } else if ('\\' == *cp) {
            cp++;
            if (*cp == 'S') {
                cp++;
                i = getval(&cp);
                if (i < 1) {
                    strcpy(err->message, "invalid value was specified to the \\S operand");
                    err->code = VGSMML_ERR_SYNTAX_CHANNEL;
                    err->line = line;
                    rc = 4;
                    goto ENDPROC;
                }
                note.type = NTYPE_ENV1;
                note.op1 = cn;
                note.op2 = 0;
                note.op3 = 0;
                note.val = i;
                if (0 != (rc = cadd(mml, cn, &note))) {
                    strcpy(err->message, "no memory");
                    err->code = VGSMML_ERR_NO_MEMORY;
                    rc = 255;
                    goto ENDPROC;
                }
            } else if (*cp == 'E') {
                cp++;
                i = getval(&cp);
                if (i < 1) {
                    strcpy(err->message, "invalid value was specified to the \\E operand");
                    err->code = VGSMML_ERR_SYNTAX_CHANNEL;
                    err->line = line;
                    rc = 4;
                    goto ENDPROC;
                }
                note.type = NTYPE_ENV2;
                note.op1 = cn;
                note.op2 = 0;
                note.op3 = 0;
                note.val = i;
                if (0 != (rc = cadd(mml, cn, &note))) {
                    strcpy(err->message, "no memory");
                    err->code = VGSMML_ERR_NO_MEMORY;
                    rc = 255;
                    goto ENDPROC;
                }
            } else {
                strcpy(err->message, "unknown operand was specified");
                err->code = VGSMML_ERR_SYNTAX_CHANNEL;
                err->line = line;
                rc = 4;
                goto ENDPROC;
            }
        } else if ('%' == *cp) {
            cp++;
            i = getval(&cp);
            if (i < 1 || 100 < i) {
                strcpy(err->message, "invalid value was specified to the %% operand");
                err->code = VGSMML_ERR_SYNTAX_CHANNEL;
                err->line = line;
                rc = 4;
                goto ENDPROC;
            }
            mml->ch[cn].per = i;
        } else if ('@' == *cp) {
            cp++;
            i = getval(&cp);
            if (i < 0) {
                strcpy(err->message, "invalid value was specified to the @ operand");
                err->code = VGSMML_ERR_SYNTAX_CHANNEL;
                err->line = line;
                rc = 4;
                goto ENDPROC;
            }
            mml->ch[cn].tone = i;
        } else if ('T' == *cp) {
            cp++;
            i = getval(&cp);
            if (i < 1) {
                strcpy(err->message, "invalid value was specified to the T operand");
                err->code = VGSMML_ERR_SYNTAX_CHANNEL;
                err->line = line;
                rc = 4;
                goto ENDPROC;
            }
            mml->tempo = (HZ * 60) / i * 4;
        } else if ('L' == *cp) {
            cp++;
            i = getval(&cp);
            if (i < 1) {
                strcpy(err->message, "invalid value was specified to the L operand");
                err->code = VGSMML_ERR_SYNTAX_CHANNEL;
                err->line = line;
                rc = 4;
                goto ENDPROC;
            }
            mml->ch[cn].len = i;
            if ('.' == *(cp + 1)) {
                cp++;
                mml->ch[cn].huten = 1;
            } else {
                mml->ch[cn].huten = 0;
            }
        } else if ('O' == *cp) {
            cp++;
            i = getval(&cp);
            if (i < 0) {
                strcpy(err->message, "invalid value was specified to the O operand");
                err->code = VGSMML_ERR_SYNTAX_CHANNEL;
                err->line = line;
                rc = 4;
                goto ENDPROC;
            }
            mml->ch[cn].oct = i;
        } else if ('>' == *cp) {
            mml->ch[cn].oct++;
        } else if ('<' == *cp) {
            mml->ch[cn].oct--;
        } else if ('V' == *cp) {
            cp++;
            if ('+' == *cp || '-' == *cp) {
                for (; '+' == *cp || '-' == *cp; cp++) {
                    if ('+' == *cp)
                        mml->ch[cn].vol++;
                    else
                        mml->ch[cn].vol--;
                }
                cp--;
                note.type = NTYPE_VOL;
                note.op1 = (unsigned char)cn;
                note.op2 = 0;
                note.op3 = 0;
                note.val = mml->ch[cn].vol;
                if (cadd(mml, cn, &note)) {
                    strcpy(err->message, "no memory");
                    err->code = VGSMML_ERR_NO_MEMORY;
                    rc = 255;
                    goto ENDPROC;
                }
            } else {
                i = getval(&cp);
                if (i < 0) {
                    strcpy(err->message, "invalid value was specified to the V operand");
                    err->code = VGSMML_ERR_SYNTAX_CHANNEL;
                    err->line = line;
                    rc = 4;
                    goto ENDPROC;
                }
                note.type = NTYPE_VOL;
                note.op1 = (unsigned char)cn;
                note.op2 = 0;
                note.op3 = 0;
                note.val = i;
                if (cadd(mml, cn, &note)) {
                    strcpy(err->message, "no memory");
                    err->code = VGSMML_ERR_NO_MEMORY;
                    rc = 255;
                    goto ENDPROC;
                }
                mml->ch[cn].vol = i;
            }
        } else if ('M' == *cp) {
            cp++;
            i = getval(&cp);
            if (i < 0) {
                strcpy(err->message, "invalid value was specified to the M operand");
                err->code = VGSMML_ERR_SYNTAX_CHANNEL;
                err->line = line;
                rc = 4;
                goto ENDPROC;
            }
            note.type = NTYPE_MVOL;
            note.op1 = 0;
            note.op2 = 0;
            note.op3 = 0;
            note.val = i;
            if (cadd(mml, cn, &note)) {
                strcpy(err->message, "no memory");
                err->code = VGSMML_ERR_NO_MEMORY;
                rc = 255;
                goto ENDPROC;
            }
        } else if ('|' == *cp) {
            note.type = NTYPE_LABEL;
            note.op1 = 0;
            note.op2 = 0;
            note.op3 = 0;
            note.val = 0;
            if (cadd(mml, cn, &note)) {
                strcpy(err->message, "no memory");
                err->code = VGSMML_ERR_NO_MEMORY;
                rc = 255;
                goto ENDPROC;
            }
        } else if ('K' == *cp) {
            cp++;
            if ('+' == *cp || '-' == *cp) {
                for (; '+' == *cp || '-' == *cp; cp++) {
                    if ('+' == *cp)
                        mml->kbase[cn]++;
                    else
                        mml->kbase[cn]--;
                }
                cp--;
            } else {
                strcpy(err->message, "invalid value was specified to the K operand");
                err->code = VGSMML_ERR_SYNTAX_CHANNEL;
                err->line = line;
                rc = 4;
                goto ENDPROC;
            }
        } else {
            strcpy(err->message, "unknown operand was specified");
            err->code = VGSMML_ERR_SYNTAX_CHANNEL;
            err->line = line;
            rc = 4;
            goto ENDPROC;
        }
    }

ENDPROC:
    free(cbuf);
    return rc;
}

static int getval(char** str)
{
    int n = 0;
    char* ptr;
    for (ptr = *str; isdigit(*ptr); ptr++) {
        n *= 10;
        n += (*ptr) - '0';
    }
    if (ptr == *str) {
        return -1;
    }
    (*str) = ptr - 1;
    return n;
}

static int merge(struct MML* mml, struct Channel* c2)
{
    struct Channel* cur;
    struct Channel* next;
    struct Channel* c1 = mml->ch[0].head;
    mml->ch[6].head = NULL;
    mml->ch[6].tail = NULL;
    while (1) {
        for (; NTYPE_WAIT != c1->note.type; c1 = c1->next) {
            if (cadd(mml, 6, &(c1->note))) {
                return -1;
            }
        }
        for (; NTYPE_WAIT != c2->note.type; c2 = c2->next) {
            if (cadd(mml, 6, &(c2->note))) {
                return -1;
            }
        }
        if (0 == c1->note.val || 0 == c2->note.val) {
            break;
        }
        if (c1->note.val < c2->note.val) {
            c2->note.val -= c1->note.val;
            if (cadd(mml, 6, &(c1->note))) {
                return -1;
            }
            c1 = c1->next;
        } else if (c1->note.val > c2->note.val) {
            c1->note.val -= c2->note.val;
            if (cadd(mml, 6, &(c2->note))) {
                return -1;
            }
            c2 = c2->next;
        } else {
            if (cadd(mml, 6, &(c1->note))) {
                return -1;
            }
            c1 = c1->next;
            c2 = c2->next;
        }
    }
    for (; NTYPE_WAIT != c1->note.type || 0 != c1->note.val; c1 = c1->next) {
        if (cadd(mml, 6, &(c1->note))) {
            return -1;
        }
    }
    for (; NTYPE_WAIT != c2->note.type || 0 != c2->note.val; c2 = c2->next) {
        if (cadd(mml, 6, &(c2->note))) {
            return -1;
        }
    }
    if (cadd(mml, 6, &(c2->note))) {
        return -1;
    }
    for (cur = mml->ch[0].head; cur; cur = next) {
        next = cur->next;
        free(cur);
    }
    mml->ch[0].head = mml->ch[6].head;
    mml->ch[0].tail = mml->ch[6].tail;
    mml->ch[6].head = NULL;
    mml->ch[6].tail = NULL;
    return 0;
}
