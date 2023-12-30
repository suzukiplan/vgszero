/* (C)2015, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS BGM Decoder
 *    Platform: Common
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#include "vgsdec_internal.h"
#include "vgstone.h"

#define ROUND(X, MIN, MAX) (X < MIN ? MIN : MAX < X ? MAX : X)
#ifdef _WIN32
#define VGS_STACK_SIZE 1048576
#else
#define VGS_STACK_SIZE (1048576 < PTHREAD_STACK_MIN ? PTHREAD_STACK_MIN : 1048576)
#define __stdcall
#endif

/*
 *----------------------------------------------------------------------------
 * Interface functions
 *----------------------------------------------------------------------------
 */
void* __stdcall vgsdec_create_context()
{
    struct _VGSCTX* result;
    result = (struct _VGSCTX*)malloc(sizeof(struct _VGSCTX));
    if (NULL == result) return NULL;
    memset(result, 0, sizeof(struct _VGSCTX));
#ifdef _WIN32
    InitializeCriticalSection(&(result->cs));
    InitializeCriticalSection(&(result->queue.cs));
#else
    pthread_mutex_init(&(result->mt), NULL);
    pthread_mutex_init(&(result->queue.mt), NULL);
    pthread_attr_init(&(result->queue.ta));
    pthread_attr_setstacksize(&(result->queue.ta), VGS_STACK_SIZE);
#endif
    return result;
}

int __stdcall vgsdec_load_bgm_from_file(void* context, const char* path)
{
    FILE* fp;
    void* data;
    int size;
    int result;

    fp = fopen(path, "rb");
    if (NULL == fp) {
        return -1;
    }
    if (-1 == fseek(fp, 0, SEEK_END)) {
        fclose(fp);
        return -1;
    }
    size = (int)ftell(fp);
    if (size < 1) {
        fclose(fp);
        return -1;
    }
    if (-1 == fseek(fp, 0, SEEK_SET)) {
        fclose(fp);
        return -1;
    }
    data = malloc(size);
    if (NULL == data) {
        fclose(fp);
        return -1;
    }
    if (size != fread(data, 1, (size_t)size, fp)) {
        fclose(fp);
        free(data);
        return -1;
    }
    fclose(fp);
    result = vgsdec_load_bgm_from_memory(context, data, size);
    free(data);
    return 0;
}

int __stdcall vgsdec_load_bgm_from_memory(void* context, void* data, size_t size)
{
    struct _VGSCTX* c = (struct _VGSCTX*)context;
    uLong nblen;
    int i;

    if (NULL == c || NULL == data) return -1;
    reset_context(c);
    if (0 == memcmp(data, "VGSPACK", 8)) {
        size_t msize = extract_meta_data(c, data, size);
        data = ((char*)data) + msize;
        size -= msize;
    }
    nblen = (uLong)sizeof(c->notes);
    memset(c->notes, 0, nblen);
    uncompress((unsigned char*)c->notes, &nblen, (const unsigned char*)data, size);
    c->idxnum = (int)(nblen / (uLong)sizeof(struct _NOTE));
    if (MAX_NOTES == c->idxnum) {
        return -1;
    }
    c->loopIdx = -1;
    c->timeL = 0;
    c->timeI = 0;
    for (i = 0; i < c->idxnum; i++) {
        if (NTYPE_WAIT == c->notes[i].type) {
            c->timeL += c->notes[i].val;
        }
        else if (NTYPE_LABEL == c->notes[i].type) {
            c->timeI = c->timeL;
            c->loopIdx = i;
        }
    }
    return 0;
}

void __stdcall vgsdec_execute(void* context, void* buffer, size_t size)
{
    struct _VGSCTX* c = (struct _VGSCTX*)context;
    char* buf = (char*)buffer;
    int i, j;
    int pw;
    int wav;
    int fm;
    short* bp;

    if (NULL == c || NULL == buf) return;
    if (0 == c->synthesis) {
        memset(buf, 0, size);
    }

    lock_context(c);
    for (i = 0; i < 6; i++) {
        c->wav[i] = 0;
    }
    if (c->play) {
        if (100 == c->fade2) {
            c->play = 0;
            unlock_context(c);
            return; /* end fadeout */
        }
        if (0 == c->waitTime) {
            c->waitTime = get_next_note(c);
            if (0 == c->waitTime) {
                c->play = 0;
                unlock_context(c);
                return; /* no data */
            }
        }
        if (c->mvol) {
            for (i = 0; i < (int)size; i += 2, c->hz++) {
                for (j = 0; j < 6; j++) {
                    if (c->ch[j].tone || c->ch[j].toneS) {
                        if (c->ch[j].toneM) {
                            c->ch[j].toneR += c->ch[j].toneA << 1;
                            c->ch[j].toneR %= 1128960; /* 4410 * 256 */
                            fm = c->ch[j].toneR;
                            if (c->hz % c->ch[j].toneMAI == 0) {
                                if (c->ch[j].toneMAA < 0) {
                                    if (c->ch[j].toneMAM < c->ch[j].toneMA) {
                                        c->ch[j].toneMA += c->ch[j].toneMAA;
                                    }
                                }
                                else {
                                    if (c->ch[j].toneMA < c->ch[j].toneMAM) {
                                        c->ch[j].toneMA += c->ch[j].toneMAA;
                                    }
                                }
                            }
                            if (c->hz % c->ch[j].toneMPI == 0) {
                                if (c->ch[j].toneMPA < 0) {
                                    if (c->ch[j].toneMPM < c->ch[j].toneMP) {
                                        c->ch[j].toneMP += c->ch[j].toneMPA;
                                    }
                                }
                                else {
                                    if (c->ch[j].toneMP < c->ch[j].toneMPM) {
                                        c->ch[j].toneMP += c->ch[j].toneMPA;
                                    }
                                }
                            }
                            c->ch[j].toneMR += c->ch[j].toneMA << 1;
                            c->ch[j].toneMR %= 1128960;
                            fm >>= 8;
                            fm += (c->ch[j].toneM[c->ch[j].toneMR >> 8] * c->ch[j].toneMP) >> 8;
                            fm %= 4410;
                            while (fm < 0) {
                                fm += 4410;
                            }
                            if (c->ch[j].tone) {
                                wav = c->ch[j].tone[fm] >> 2;
                            }
                            else {
                                wav = c->ch[j].toneS[fm] >> 2;
                            }
                        }
                        else {
                            c->ch[j].cur %= c->ch[j].toneS[0];
                            wav = c->ch[j].toneS[1 + c->ch[j].cur];
                            c->ch[j].cur += 2;
                        }
                        wav *= (c->ch[j].vol * c->mvol);
                        if (c->ch[j].keyOn) {
                            if (c->ch[j].count < c->ch[j].env1) {
                                c->ch[j].count++;
                                pw = (c->ch[j].count * 100) / c->ch[j].env1;
                            }
                            else {
                                pw = 100;
                            }
                        }
                        else {
                            if (c->ch[j].count < c->ch[j].env2) {
                                c->ch[j].count++;
                                pw = 100 - (c->ch[j].count * 100) / c->ch[j].env2;
                            }
                            else {
                                pw = 0;
                            }
                        }
                        if (!c->ch[j].mute) {
                            bp = (short*)(&buf[i]);
                            wav = (wav * pw / 100);
                            if (c->ch[j].volumeRate != 100) {
                                wav *= c->ch[j].volumeRate;
                                wav /= 100;
                            }
                            wav += *bp;
                            if (32767 < wav) {
                                wav = 32767;
                            }
                            else if (wav < -32768) {
                                wav = -32768;
                            }
                            if (c->fade2) {
                                wav *= 100 - c->fade2;
                                wav /= 100;
                            }
                            (*bp) = (short)wav;
                            if (i) {
                                c->wav[j] += pw < 0 ? -pw : pw;
                                c->wav[j] >>= 1;
                            }
                            else {
                                c->wav[j] = pw < 0 ? -pw : pw;
                            }
                        }
                        if (c->ch[j].pdown) {
                            c->ch[j].pcnt++;
                            if (c->ch[j].pdown < c->ch[j].pcnt) {
                                c->ch[j].pcnt = 0;
                                if (c->ch[j].toneK) {
                                    c->ch[j].toneK--;
                                }
                                set_note(c, j & 0xff, c->ch[j].toneT, c->ch[j].toneK);
                            }
                        }
                    }
                }
                if (c->volumeRate != 100) {
                    bp = (short*)(&buf[i]);
                    wav = (*bp) * c->volumeRate;
                    wav /= 100;
                    if (32767 < wav)
                        wav = 32767;
                    else if (wav < -32768)
                        wav = -32768;
                    (*bp) = (short)wav;
                }
                c->waitTime--;
                if (0 == c->waitTime) {
                    c->waitTime = get_next_note(c);
                    if (0 == c->waitTime) {
                        unlock_context(c);
                        return; /* no data */
                    }
                }
                /* fade out */
                if (c->fade2 && c->fade2 < 100) {
                    c->fcnt++;
                    if (1023 < c->fcnt) {
                        c->fcnt = 0;
                        c->fade2++;
                    }
                }
                if (c->fade) {
                    c->fcnt++;
                    if (c->fade < c->fcnt) {
                        c->mvol--;
                        c->fcnt = 0;
                    }
                }
            }
        }
    }
    else {
        c->stopped = 1;
    }
    unlock_context(c);
}

int __stdcall vgsdec_get_value(void* context, int type)
{
    struct _VGSCTX* c = (struct _VGSCTX*)context;
    if (NULL == c) return -1;
    switch (type) {
        case VGSDEC_REG_KEY_0:
            return c->ch[0].toneK;
        case VGSDEC_REG_KEY_1:
            return c->ch[1].toneK;
        case VGSDEC_REG_KEY_2:
            return c->ch[2].toneK;
        case VGSDEC_REG_KEY_3:
            return c->ch[3].toneK;
        case VGSDEC_REG_KEY_4:
            return c->ch[4].toneK;
        case VGSDEC_REG_KEY_5:
            return c->ch[5].toneK;
        case VGSDEC_REG_TONE_0:
            return c->ch[0].toneT;
        case VGSDEC_REG_TONE_1:
            return c->ch[1].toneT;
        case VGSDEC_REG_TONE_2:
            return c->ch[2].toneT;
        case VGSDEC_REG_TONE_3:
            return c->ch[3].toneT;
        case VGSDEC_REG_TONE_4:
            return c->ch[4].toneT;
        case VGSDEC_REG_TONE_5:
            return c->ch[5].toneT;
        case VGSDEC_REG_VOL_0:
            return c->wav[0];
        case VGSDEC_REG_VOL_1:
            return c->wav[1];
        case VGSDEC_REG_VOL_2:
            return c->wav[2];
        case VGSDEC_REG_VOL_3:
            return c->wav[3];
        case VGSDEC_REG_VOL_4:
            return c->wav[4];
        case VGSDEC_REG_VOL_5:
            return c->wav[5];
        case VGSDEC_REG_PLAYING:
            return c->play;
        case VGSDEC_REG_INDEX:
            return c->nidx;
        case VGSDEC_REG_LOOP_INDEX:
            return c->loopIdx;
        case VGSDEC_REG_LENGTH:
            return c->idxnum;
        case VGSDEC_REG_TIME:
            return c->timeP;
        case VGSDEC_REG_LOOP_TIME:
            return c->timeI;
        case VGSDEC_REG_TIME_LENGTH:
            return c->timeL;
        case VGSDEC_REG_LOOP_COUNT:
            return c->loop;
        case VGSDEC_REG_FADEOUT_COUNTER:
            return c->fade2;
        case VGSDEC_REG_VOLUME_RATE_0:
            return c->ch[0].volumeRate;
        case VGSDEC_REG_VOLUME_RATE_1:
            return c->ch[1].volumeRate;
        case VGSDEC_REG_VOLUME_RATE_2:
            return c->ch[2].volumeRate;
        case VGSDEC_REG_VOLUME_RATE_3:
            return c->ch[3].volumeRate;
        case VGSDEC_REG_VOLUME_RATE_4:
            return c->ch[4].volumeRate;
        case VGSDEC_REG_VOLUME_RATE_5:
            return c->ch[5].volumeRate;
        case VGSDEC_REG_VOLUME_RATE:
            return c->volumeRate;
        case VGSDEC_REG_SYNTHESIS_BUFFER:
            return c->synthesis;
        case VGSDEC_REG_KEYON_0:
            return c->ch[0].keyOn;
        case VGSDEC_REG_KEYON_1:
            return c->ch[1].keyOn;
        case VGSDEC_REG_KEYON_2:
            return c->ch[2].keyOn;
        case VGSDEC_REG_KEYON_3:
            return c->ch[3].keyOn;
        case VGSDEC_REG_KEYON_4:
            return c->ch[4].keyOn;
        case VGSDEC_REG_KEYON_5:
            return c->ch[5].keyOn;
        case VGSDEC_REG_MUTE_0:
            return c->ch[0].mute;
        case VGSDEC_REG_MUTE_1:
            return c->ch[1].mute;
        case VGSDEC_REG_MUTE_2:
            return c->ch[2].mute;
        case VGSDEC_REG_MUTE_3:
            return c->ch[3].mute;
        case VGSDEC_REG_MUTE_4:
            return c->ch[4].mute;
        case VGSDEC_REG_MUTE_5:
            return c->ch[5].mute;
        case VGSDEC_REG_ADD_KEY_0:
            return c->addKey[0];
        case VGSDEC_REG_ADD_KEY_1:
            return c->addKey[1];
        case VGSDEC_REG_ADD_KEY_2:
            return c->addKey[2];
        case VGSDEC_REG_ADD_KEY_3:
            return c->addKey[3];
        case VGSDEC_REG_ADD_KEY_4:
            return c->addKey[4];
        case VGSDEC_REG_ADD_KEY_5:
            return c->addKey[5];
        case VGSDEC_REG_KOBUSHI:
            return c->ch[0].toneM ? 1 : 0;
    }
    return -1;
}

void __stdcall vgsdec_set_value(void* context, int type, int value)
{
    struct _VGSCTX* c = (struct _VGSCTX*)context;
    int i;

    if (NULL == c) return;
    lock_context(c);
    switch (type) {
        case VGSDEC_REG_TIME:
            if (0 <= value) jump_time(c, value);
            break;
        case VGSDEC_REG_FADEOUT:
            if (value && 0 == c->fade2) c->fade2 = 1;
            break;
        case VGSDEC_REG_RESET:
            if (value) reset_context(c);
            break;
        case VGSDEC_REG_VOLUME_RATE_0:
            c->ch[0].volumeRate = ROUND(value, 0, 100);
            break;
        case VGSDEC_REG_VOLUME_RATE_1:
            c->ch[1].volumeRate = ROUND(value, 0, 100);
            break;
        case VGSDEC_REG_VOLUME_RATE_2:
            c->ch[2].volumeRate = ROUND(value, 0, 100);
            break;
        case VGSDEC_REG_VOLUME_RATE_3:
            c->ch[3].volumeRate = ROUND(value, 0, 100);
            break;
        case VGSDEC_REG_VOLUME_RATE_4:
            c->ch[4].volumeRate = ROUND(value, 0, 100);
            break;
        case VGSDEC_REG_VOLUME_RATE_5:
            c->ch[5].volumeRate = ROUND(value, 0, 100);
            break;
        case VGSDEC_REG_VOLUME_RATE:
            c->volumeRate = ROUND(value, 0, 100);
            break;
        case VGSDEC_REG_SYNTHESIS_BUFFER:
            c->synthesis = value ? 1 : 0;
            break;
        case VGSDEC_REG_MUTE_0:
            c->ch[0].mute = value ? 1 : 0;
            break;
        case VGSDEC_REG_MUTE_1:
            c->ch[1].mute = value ? 1 : 0;
            break;
        case VGSDEC_REG_MUTE_2:
            c->ch[2].mute = value ? 1 : 0;
            break;
        case VGSDEC_REG_MUTE_3:
            c->ch[3].mute = value ? 1 : 0;
            break;
        case VGSDEC_REG_MUTE_4:
            c->ch[4].mute = value ? 1 : 0;
            break;
        case VGSDEC_REG_MUTE_5:
            c->ch[5].mute = value ? 1 : 0;
            break;
        case VGSDEC_REG_ADD_KEY_0:
            c->addKey[0] = value;
            break;
        case VGSDEC_REG_ADD_KEY_1:
            c->addKey[1] = value;
            break;
        case VGSDEC_REG_ADD_KEY_2:
            c->addKey[2] = value;
            break;
        case VGSDEC_REG_ADD_KEY_3:
            c->addKey[3] = value;
            break;
        case VGSDEC_REG_ADD_KEY_4:
            c->addKey[4] = value;
            break;
        case VGSDEC_REG_ADD_KEY_5:
            c->addKey[5] = value;
            break;
        case VGSDEC_REG_KOBUSHI:
            if (value) {
                c->ch[0].toneM = NULL;
                for (i = 1; i < 6; i++) {
                    c->ch[i].toneMR = 0;    /* モジュレータ・ラジアン初期値(0固定) */
                    c->ch[i].toneM = S;     /* モジュレータ波形 */
                    c->ch[i].toneMA = 0;    /* モジュレータ・ラジアン加算(A)初期値 */
                    c->ch[i].toneMAM = 200; /* モジュレータ・ラジアン加算(A)最大値/最小値 */
                    c->ch[i].toneMAI = 40;  /* モジュレータ・ラジアン加算(A)増幅間隔(Hz) */
                    c->ch[i].toneMAA = 1;   /* モジュレータ・ラジアン加算(A)増幅値 */
                    c->ch[i].toneMP = 0;    /* モジュレータ・ベロシティ(P)初期値 */
                    c->ch[i].toneMPM = 512; /* モジュレータ・ベロシティ(P)最大値/最小値 */
                    c->ch[i].toneMPI = 12;  /* モジュレータ・ベロシティ(P)増幅間隔(Hz) */
                    c->ch[i].toneMPA = 32;  /* モジュレータ・ベロシティ(P)増幅値 */
                }
            }
            else {
                for (i = 0; i < 6; i++) {
                    c->ch[i].toneM = NULL;
                }
            }
            break;
    }
    unlock_context(c);
}

void __stdcall vgsdec_release_context(void* context)
{
    struct _VGSCTX* c = (struct _VGSCTX*)context;

    if (NULL != c) {
        vgsdec_async_stop(c);
#ifdef _WIN32
        DeleteCriticalSection(&(c->cs));
        DeleteCriticalSection(&(c->queue.cs));
#else
        pthread_mutex_destroy(&(c->mt));
        pthread_mutex_destroy(&(c->queue.mt));
        pthread_attr_destroy(&(c->queue.ta));
#endif
        release_meta_data(c);
        memset(c, 0, sizeof(struct _VGSCTX));
        free(c);
    }
}

struct VgsMetaHeader* __stdcall vgsdec_get_meta_header(void* context)
{
    struct _VGSCTX* c = (struct _VGSCTX*)context;
    if (NULL == c) return NULL;
    return c->mhead;
}

struct VgsMetaData* __stdcall vgsdec_get_meta_data(void* context, int index)
{
    struct _VGSCTX* c = (struct _VGSCTX*)context;
    if (NULL == c || NULL == c->mhead || NULL == c->mdata || index < 0) return NULL;
    if (c->mhead->num <= index) return NULL;
    return c->mdata[index];
}

int __stdcall vgsdec_async_start(void* context)
{
    struct _VGSCTX* c = (struct _VGSCTX*)context;
    if (NULL == c || c->queue.enabled) return -1;
#ifdef _WIN32
    c->queue.tid = _beginthreadex(NULL, VGS_STACK_SIZE, async_manager, context, 0, NULL);
    if (-1L == c->queue.tid) {
        return -1;
    }
#else
    if (pthread_create(&(c->queue.tid), &(c->queue.ta), async_manager, context)) {
        return -1;
    }
#endif
    c->queue.enabled = 1;
    return 0;
}

int __stdcall vgsdec_async_enqueue(void* context, void* buffer, size_t size, void (*callback)(void* context, void* buffer, size_t size))
{
    struct _VGSCTX* c = (struct _VGSCTX*)context;
    struct _VGS_QDATA* qdata;
    if (NULL == c || !c->queue.enabled || NULL == buffer || size < 1) return -1;

    /* allocate and set */
    qdata = (struct _VGS_QDATA*)malloc(sizeof(struct _VGS_QDATA));
    if (NULL == qdata) return -1;
    qdata->next = NULL;
    qdata->callback = callback;
    qdata->context = context;
    qdata->buffer = buffer;
    qdata->size = size;

    /* enqueue */
    lock_queue(c);
    if (c->queue.enabled) {
        if (NULL == c->queue.tail) {
            c->queue.head = qdata;
            c->queue.tail = qdata;
        }
        else {
            c->queue.tail->next = qdata;
            c->queue.tail = qdata;
        }
        c->queue.count++;
    }
    else {
        free(qdata);
    }
    unlock_queue(c);
    return 0;
}

void __stdcall vgsdec_async_stop(void* context)
{
    struct _VGSCTX* c = (struct _VGSCTX*)context;
    struct _VGS_QDATA* cur;
    struct _VGS_QDATA* next;
    if (NULL == c || !c->queue.enabled) return;

    lock_queue(c);        /* stop another dequeue */
    lock_context(c);      /* waiting for executing async-decode */
    c->queue.enabled = 0; /* reset (set end-flag) */
    unlock_context(c);
    unlock_queue(c);
#ifdef _WIN32
    WaitForSingleObject((HANDLE)c->queue.tid, INFINITE);
    CloseHandle((HANDLE)c->queue.tid);
#else
    pthread_join(c->queue.tid, NULL);
#endif

    /* purge */
    for (cur = c->queue.head; cur; cur = next) {
        next = cur->next;
        free(cur);
    }
    c->queue.head = NULL;
    c->queue.tail = NULL;
    c->queue.count = 0;
}

/*
 *----------------------------------------------------------------------------
 * Internal functions
 *----------------------------------------------------------------------------
 */
static void reset_context(struct _VGSCTX* c)
{
    int i;
    c->play = 1;
    c->mask = 0;
    c->mvol = 0;
    c->waitTime = 0;
    memset(c->wav, 0, sizeof(c->wav));
    c->nidx = 0;
    c->stopped = 0;
    c->fade = 0;
    c->fcnt = 0;
    memset(c->ch, 0, sizeof(c->ch));
    c->mute = 0;
    c->loop = 0;
    c->fade2 = 0;
    memset(c->addKey, 0, sizeof(c->addKey));
    memset(c->addOff, 0, sizeof(c->addOff));
    c->volumeRate = 100;
    for (i = 0; i < 6; i++) {
        c->ch[i].volumeRate = 100;
    }
    c->hz = 0;
    c->timeP = 0;
}

static size_t extract_meta_data(struct _VGSCTX* c, void* data, size_t size)
{
    size_t msize = 0;
    char* d = (char*)data;
    int i;

    /* release old data */
    release_meta_data(c);

    /* read header */
    msize = sizeof(struct VgsMetaHeader);
    if (size < msize) return 0;
    c->mhead = (struct VgsMetaHeader*)malloc(msize);
    if (NULL == c->mhead) return 0;
    memcpy(c->mhead, d, msize);
    d += msize;

    /* read data */
    if (c->mhead->num) {
        c->mdata = (struct VgsMetaData**)malloc(sizeof(struct VgsMetaData*) * c->mhead->num);
        memset(c->mdata, 0, sizeof(struct VgsMetaData*) * c->mhead->num);
        for (i = 0; i < c->mhead->num; i++) {
            if (size < msize + sizeof(struct VgsMetaData)) {
                release_meta_data(c);
                return 0;
            }
            c->mdata[i] = (struct VgsMetaData*)malloc(sizeof(struct VgsMetaData));
            if (NULL == c->mdata[i]) {
                release_meta_data(c);
                return 0;
            }
            memcpy(c->mdata[i], d, sizeof(struct VgsMetaData));
            c->mdata[i]->year = ntohs(c->mdata[i]->year);
            c->mdata[i]->aid = ntohs(c->mdata[i]->aid);
            c->mdata[i]->track = ntohs(c->mdata[i]->track);
            d += sizeof(struct VgsMetaData);
            msize += sizeof(struct VgsMetaData);
        }
    }
    return msize;
}

static void release_meta_data(struct _VGSCTX* c)
{
    int i;
    if (NULL != c->mhead) {
        if (NULL != c->mdata) {
            /* release data */
            for (i = 0; i < c->mhead->num; i++) {
                if (NULL != c->mdata[i]) free(c->mdata[i]);
                c->mdata[i] = NULL;
            }
            free(c->mdata);
            c->mdata = NULL;
        }
        /* release header */
        free(c->mhead);
        c->mhead = NULL;
    }
}

static void lock_context(struct _VGSCTX* c)
{
#ifdef _WIN32
    EnterCriticalSection(&(c->cs));
#else
    pthread_mutex_lock(&(c->mt));
#endif
}

static void unlock_context(struct _VGSCTX* c)
{
#ifdef _WIN32
    LeaveCriticalSection(&(c->cs));
#else
    pthread_mutex_unlock(&(c->mt));
#endif
}

static void lock_queue(struct _VGSCTX* c)
{
#ifdef _WIN32
    EnterCriticalSection(&(c->queue.cs));
#else
    pthread_mutex_lock(&(c->queue.mt));
#endif
}

static void unlock_queue(struct _VGSCTX* c)
{
#ifdef _WIN32
    LeaveCriticalSection(&(c->queue.cs));
#else
    pthread_mutex_unlock(&(c->queue.mt));
#endif
}

/* set tone and tune */
static inline void set_note(struct _VGSCTX* c, unsigned char cn, unsigned char t, unsigned char n)
{
    n += c->addKey[cn];
    if (c->ch[cn].toneM) {
        c->ch[cn].toneS = NULL;
        c->ch[cn].toneA = A[n % 88];
        switch (t) {
            case 0: /* SANKAKU */
                c->ch[cn].tone = S;
                break;
            case 1: /* NOKOGIR */
                c->ch[cn].tone = W;
                break;
            case 2: /* KUKEI */
                c->ch[cn].tone = Q;
                break;
            default: /* NOIZE */
                c->ch[cn].tone = N;
                break;
        }
    }
    else {
        c->ch[cn].tone = NULL;
        switch (t) {
            case 0: /* SANKAKU */
                c->ch[cn].toneS = TONE1[n % 85];
                break;
            case 1: /* NOKOGIR */
                c->ch[cn].toneS = TONE2[n % 85];
                break;
            case 2: /* KUKEI */
                c->ch[cn].toneS = TONE3[n % 85];
                break;
            default: /* NOIZE */
                c->ch[cn].toneS = TONE4[n % 85];
                break;
        }
    }
}

/* get next waittime */
static inline int get_next_note(struct _VGSCTX* c)
{
    int ret;
    if (c->notes[c->nidx].type == NTYPE_WAIT && 0 == c->notes[c->nidx].val) {
        return 0;
    }
    for (; NTYPE_WAIT != c->notes[c->nidx].type; c->nidx++) {
        switch (c->notes[c->nidx].type) {
            case NTYPE_ENV1: /* op1=ch, val=env1 */
                c->ch[c->notes[c->nidx].op1].env1 = c->notes[c->nidx].val;
                break;
            case NTYPE_ENV2: /* op1=ch, val=env1 */
                c->ch[c->notes[c->nidx].op1].env2 = c->notes[c->nidx].val;
                break;
            case NTYPE_VOL: /* op1=ch, val=vol */
                c->ch[c->notes[c->nidx].op1].vol = c->notes[c->nidx].val;
                break;
            case NTYPE_MVOL: /* val=mvol */
                c->mvol = c->notes[c->nidx].val;
                break;
            case NTYPE_KEYON: /* op1=ch, op2=tone, op3=key */
                c->ch[c->notes[c->nidx].op1].keyOn = 1;
                c->ch[c->notes[c->nidx].op1].count = 0;
                c->ch[c->notes[c->nidx].op1].cur = 0;
                c->ch[c->notes[c->nidx].op1].toneT = c->notes[c->nidx].op2;
                c->ch[c->notes[c->nidx].op1].toneK = c->notes[c->nidx].op3;
                set_note(c, c->notes[c->nidx].op1, c->notes[c->nidx].op2, c->notes[c->nidx].op3);
                break;
            case NTYPE_KEYOFF: /* op1=ch */
                c->ch[c->notes[c->nidx].op1].keyOn = 0;
                c->ch[c->notes[c->nidx].op1].count = 0;
                break;
            case NTYPE_PDOWN: /* op1=ch, val=Hz */
                c->ch[c->notes[c->nidx].op1].pdown = c->notes[c->nidx].val;
                c->ch[c->notes[c->nidx].op1].pcnt = 0;
                break;
            case NTYPE_JUMP: /* val=address */
                c->nidx = c->notes[c->nidx].val;
                c->loop++;
                c->timeP = c->timeI;
                break;
            case NTYPE_LABEL:
                break;
            default:
                return 0;
        }
    }
    ret = c->notes[c->nidx].val;
    if (ret) {
        c->nidx++;
    }
    c->timeP += ret;
printf("len %d\n", ret);
    return ret;
}

/* jump specific time */
static void jump_time(struct _VGSCTX* c, int sec)
{
    int wt;
    reset_context(c);
    while (0 < sec) {
        wt = get_next_note(c);
        if (0 == wt) break;
        sec -= wt;
    }
}

static void msleep(int ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

/* async decode manager */
#ifdef _WIN32
static unsigned __stdcall async_manager(void* context)
#else
static void* async_manager(void* context)
#endif
{
    struct _VGSCTX* c = (struct _VGSCTX*)context;
    struct _VGS_QDATA* data;
    while (c->queue.enabled) {
        /* dequeue */
        lock_queue(c);
        if (0 == c->queue.count) {
            data = NULL;
        }
        else {
            data = c->queue.head;
            if (c->queue.head == c->queue.tail) {
                c->queue.head = NULL;
                c->queue.tail = NULL;
            }
            else {
                c->queue.head = c->queue.head->next;
            }
            data->next = NULL;
            c->queue.count--;
        }
        unlock_queue(c);
        /* sleep if empty */
        if (NULL == data) {
            msleep(5);
            continue;
        }
        /* decode */
        vgsdec_execute(c, data->buffer, data->size);
        data->callback(c, data->buffer, data->size);
        free(data);
    }
#ifdef _WIN32
    _endthreadex(0);
    return 0;
#else
    return NULL;
#endif
}
