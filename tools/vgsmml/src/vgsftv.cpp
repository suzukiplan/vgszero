/**
 * VGS Fixed NOTE data to Variable NOTE data
 * 東方BGM on VGS for RasberryPi Pico
 * License under GPLv3: https://github.com/suzukiplan/tohovgs-pico/blob/master/LICENSE.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>

#define NTYPE_ENV1 1
#define NTYPE_ENV2 2
#define NTYPE_VOL 3
#define NTYPE_MVOL 4
#define NTYPE_KEYON 5
#define NTYPE_KEYOFF 6
#define NTYPE_WAIT 7
#define NTYPE_PDOWN 8
#define NTYPE_JUMP 9
#define NTYPE_LABEL 10
#define NTYPE_WAIT_8 11
#define NTYPE_WAIT_16 12
#define NTYPE_WAIT_32 13

#define MAX_NOTES 65536

typedef struct _NOTE {
    unsigned char type;
    unsigned char op1;
    unsigned char op2;
    unsigned char op3;
    unsigned int val;
} NOTE;

static size_t readWriteNotes(const char* notes, size_t notesSize, char* result, unsigned int* length, unsigned int* loop)
{
    NOTE note;
    std::map<int, int> indexMap;
    std::map<int, unsigned int> timeMap;
    int indexF = 0;
    int indexV = 0;
    unsigned int time = 0;
    while (sizeof(NOTE) <= notesSize) {
        memcpy(&note, notes, sizeof(NOTE));
        notes += sizeof(NOTE);
        notesSize -= sizeof(NOTE);
        indexMap[indexF] = indexV;
        timeMap[indexF] = time;
        if (note.type == NTYPE_WAIT) {
            if (note.val < 1) {
                // ignore 0 or minus
            } else if (note.val < 256) {
                unsigned char op1 = NTYPE_WAIT_8 << 4;
                if (result) {
                    memcpy(result, &op1, 1);
                    result++;
                }
                unsigned char v8 = note.val & 0xFF;
                time += v8;
                if (result) {
                    memcpy(result, &v8, 1);
                    result++;
                }
                indexV += 2;
            } else if (note.val < 65536) {
                unsigned char op1 = NTYPE_WAIT_16 << 4;
                if (result) {
                    memcpy(result, &op1, 1);
                    result++;
                }
                unsigned short v16 = note.val & 0xFFFF;
                time += v16;
                if (result) {
                    memcpy(result, &v16, 2);
                    result += 2;
                }
                indexV += 3;
            } else {
                unsigned char op1 = NTYPE_WAIT_32 << 4;
                if (result) {
                    memcpy(result, &op1, 1);
                    result++;
                }
                time += note.val;
                if (result) {
                    memcpy(result, &note.val, 4);
                    result += 4;
                }
                indexV += 5;
            }
            indexF++;
            continue;
        }
        unsigned char op1 = note.type & 0x0F;
        op1 <<= 4;
        op1 |= note.op1 & 0x0F;
        if (result) {
            memcpy(result, &op1, 1);
            result++;
        }
        indexV++;
        switch (note.type) {
            case NTYPE_JUMP: {
                int a = indexMap[note.val];
                *loop = timeMap[note.val];
                if (result) {
                    memcpy(result, &a, 4);
                    result += 4;
                }
                indexV += 4;
                break;
            }
            case NTYPE_ENV1:
            case NTYPE_ENV2:
            case NTYPE_PDOWN: {
                short v16 = (short)note.val;
                if (result) {
                    memcpy(result, &v16, 2);
                    result += 2;
                }
                indexV += 2;
                break;
            }
            case NTYPE_VOL:
            case NTYPE_MVOL: {
                unsigned char v8 = note.val & 0xFF;
                if (result) {
                    memcpy(result, &v8, 1);
                    result++;
                }
                indexV++;
                break;
            }
            case NTYPE_KEYON: {
                if (result) {
                    memcpy(result, &note.op2, 1);
                    result++;
                    memcpy(result, &note.op3, 1);
                    result++;
                }
                indexV += 2;
                break;
            }
            case NTYPE_KEYOFF:
            case NTYPE_LABEL: {
                break;
            }
            default:
                printf("detect invalid type (%d)\n", note.type);
                exit(-1);
        }
        indexF++;
    }
    *length = timeMap[indexF - 1];
    return indexV;
}

static char* toTimeStr(unsigned int duration)
{
    static char result[16];
    unsigned int sec = duration / 22050;
    unsigned int min = sec / 60;
    sec %= 60;
    snprintf(result, sizeof(result), "%02d:%02d", min, sec);
    return result;
}

extern "C" int vgsftv(const void* notes, size_t notesSize, void** ftv, size_t* ftvSize)
{
    unsigned int length = 0;
    unsigned int loop = 0;
    *ftvSize = readWriteNotes((const char*)notes, notesSize, nullptr, &length, &loop) + 16;
    printf("song-length: %s\n", toTimeStr(length));
    printf("loop-position: %s\n", toTimeStr(loop));
    printf("ftv-size: %lu bytes\n", *ftvSize);
    char* result = (char*)malloc(*ftvSize);
    if (!result) {
        return -1;
    }
    memcpy(&result[0], "VGSBGM-V", 8);
    memcpy(&result[8], &length, 4);
    memcpy(&result[12], &loop, 4);
    readWriteNotes((const char*)notes, notesSize, &result[16], &length, &loop);
    *ftv = result;
    return 0;
}
