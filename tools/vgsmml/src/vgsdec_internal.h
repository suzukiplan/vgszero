/**
 * 東方BGM on VGS for RasberryPi Pico
 * License under GPLv3: https://github.com/suzukiplan/tohovgs-pico/blob/master/LICENSE.txt
 */

#ifndef INCLUDE_VGSDEC_INTERNAL_H
#define INCLUDE_VGSDEC_INTERNAL_H
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#define inline
#else
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vgsdec.h"

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

#define MAX_NOTES 65536

struct _NOTE {
    unsigned char type;
    unsigned char op1;
    unsigned char op2;
    unsigned char op3;
    unsigned int val;
};

struct _PSGCH {
    short* toneS;
    int* tone;
    int toneA;
    int toneR;
    int* toneM;
    int toneMA;
    int toneMR;
    int toneMAM;
    int toneMAA;
    int toneMAI;
    int toneMP;
    int toneMPA;
    int toneMPM;
    int toneMPI;
    unsigned char vol;
    unsigned char keyOn;
    unsigned char mute;
    unsigned int cur;
    unsigned int count;
    unsigned int env1;
    unsigned int env2;
    unsigned char toneT;
    unsigned char toneK;
    unsigned int pdown;
    unsigned int pcnt;
    int volumeRate;
};

struct _VGS_QDATA {
    struct _VGS_QDATA* next;
    void (*callback)(void* context, void* buffer, size_t size);
    void* context;
    void* buffer;
    size_t size;
};

struct _VGS_QUEUE {
    volatile int enabled;
    volatile int count;
#ifdef _WIN32
    CRITICAL_SECTION cs;
    uintptr_t tid;
#else
    pthread_mutex_t mt;
    pthread_attr_t ta;
    pthread_t tid;
#endif
    struct _VGS_QDATA* head;
    struct _VGS_QDATA* tail;
};

struct _VGSCTX {
#ifdef _WIN32
    CRITICAL_SECTION cs;
#else
    pthread_mutex_t mt;
#endif
    struct _NOTE notes[MAX_NOTES];
    struct VgsMetaHeader* mhead;
    struct VgsMetaData** mdata;
    unsigned char play;
    unsigned char mask;
    unsigned short mvol;
    unsigned int waitTime;
    unsigned int hz;
    int wav[6];
    int nidx;
    int stopped;
    unsigned int fade;
    unsigned int fcnt;
    struct _PSGCH ch[6];
    int mute;
    int loop;
    int fade2;
    unsigned int timeI;
    unsigned int timeL;
    unsigned int timeP;
    int addKey[6];
    int addOff[6];
    int loopIdx;
    int idxnum;
    int volumeRate;
    int synthesis;
    struct _VGS_QUEUE queue;
};

extern short* TONE1[85];
extern short* TONE2[85];
extern short* TONE3[85];
extern short* TONE4[85];

static void reset_context(struct _VGSCTX* c);
static void lock_context(struct _VGSCTX* c);
static void unlock_context(struct _VGSCTX* c);
static void lock_queue(struct _VGSCTX* c);
static void unlock_queue(struct _VGSCTX* c);
static inline void set_note(struct _VGSCTX* c, unsigned char cn, unsigned char t, unsigned char n);
static inline int get_next_note(struct _VGSCTX* c);
static void jump_time(struct _VGSCTX* c, int sec);
static size_t extract_meta_data(struct _VGSCTX* c, void* data, size_t size);
static void release_meta_data(struct _VGSCTX* c);
static void msleep(int ms);
#ifdef _WIN32
static unsigned __stdcall async_manager(void* context);
#else
static void* async_manager(void* context);
#endif

#endif
