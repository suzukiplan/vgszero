//
//  sound-linux
//  Sound System for Linux (ALSA)
//
//  Created by Yoi Suzuki on 2023/09/01.
//  Copyright ©2023 SUZUKI PLAN. All rights reserved.
//

#include "sound-linux.h"
#include "BufferQueue.h"
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_CHANNELS 1
#define NUM_BUFFERS 3
#define BUFFER_SIZE 4096
#define SAMPLE_TYPE short
#define MAX_NUMBER 32767
#define SAMPLE_RATE 22050
#define SAMPLE_BUFS 4096

class Context {
private:
    pthread_mutex_t mutex;
    pthread_t tid;
    bool alive;
    BufferQueue* bq;

    static void* threadMain(void* arg)
    {
        Context* this_ = (Context*)arg;
        snd_pcm_t* snd;
        snd_pcm_hw_params_t* hwp;
        unsigned int rate = SAMPLE_RATE;
        int periods = 3;
        short buf[SAMPLE_BUFS / 2];

        snd_pcm_hw_params_alloca(&hwp);
        auto result = snd_pcm_open(&snd, "default", SND_PCM_STREAM_PLAYBACK, 0);
        if (result < 0) {
            fprintf(stderr, "snd_pcm_open failed: %d\n", result);
            return nullptr;
        }

        result = snd_pcm_hw_params_any(snd, hwp);
        if (result < 0) {
            fprintf(stderr, "snd_pcm_hw_params_any failed: %d\n", result);
            return nullptr;
        }

        result = snd_pcm_hw_params_set_format(snd, hwp, SND_PCM_FORMAT_S16_LE);
        if (result < 0) {
            fprintf(stderr, "snd_pcm_hw_params_set_format failed: %d\n", result);
            return nullptr;
        }

        result = snd_pcm_hw_params_set_rate_near(snd, hwp, &rate, nullptr);
        if (result < 0) {
            fprintf(stderr, "snd_pcm_hw_params_set_rate_near failed: %d\n", result);
            return nullptr;
        }
        if (rate != SAMPLE_RATE) {
            fprintf(stderr, "invalid sampling rate\n");
            snd_pcm_close(snd);
            return nullptr;
        }

        result = snd_pcm_hw_params_set_channels(snd, hwp, NUM_CHANNELS);
        if (result < 0) {
            fprintf(stderr, "snd_pcm_hw_params_set_channels failed: %d\n", result);
            snd_pcm_close(snd);
            return nullptr;
        }

        result = snd_pcm_hw_params_set_periods(snd, hwp, periods, 0);
        if (result < 0) {
            fprintf(stderr, "snd_pcm_hw_params_set_periods failed: %d\n", result);
            snd_pcm_close(snd);
            return nullptr;
        }

        result = snd_pcm_hw_params_set_buffer_size(snd, hwp, (periods * sizeof(buf)) / 4);
        if (result < 0) {
            fprintf(stderr, "snd_pcm_hw_params_set_buffer_size failed: %d\n", result);
            snd_pcm_close(snd);
            return nullptr;
        }

        result = snd_pcm_hw_params(snd, hwp);
        if (result < 0) {
            fprintf(stderr, "snd_pcm_hw_params failed: %d\n", result);
            snd_pcm_close(snd);
            return nullptr;
        }

        void* qbuf;
        size_t qsize;
        while (this_->isAlive()) {
            if (this_->getBufferSize() < sizeof(buf)) {
                usleep(1000);
                continue;
            }
            this_->lock();
            this_->bq->dequeue(&qbuf, &qsize, sizeof(buf));
            memcpy(buf, qbuf, qsize);
            this_->unlock();
            result = snd_pcm_writei(snd, buf, sizeof(buf) / 2);
            if (result < 0) {
                fprintf(stderr, "snd_pcm_writei failed: %d\n", result);
                break;
            }
        }
        snd_pcm_drain(snd);
        snd_pcm_close(snd);
        return nullptr;
    }

    inline void lock() { pthread_mutex_lock(&this->mutex); }
    inline void unlock() { pthread_mutex_unlock(&this->mutex); }

    inline bool isAlive()
    {
        bool result;
        this->lock();
        result = this->alive;
        this->unlock();
        return result;
    }

public:
    Context() {
        this->bq = new BufferQueue(65536);
        this->alive = true;
        pthread_mutex_init(&this->mutex, nullptr);
        pthread_create(&this->tid, nullptr, Context::threadMain, this);
        struct sched_param param;
        memset(&param, 0, sizeof(param));
        param.sched_priority = 46;
        pthread_setschedparam(this->tid, SCHED_OTHER, &param);
    }

    ~Context() {
        this->lock();
        this->alive = false;
        this->unlock();
        pthread_join(this->tid, nullptr);
        pthread_mutex_destroy(&this->mutex);        
    }

    inline void enqueue(void* buffer, size_t size)
    {
        this->lock();
        this->bq->enqueue(buffer, size);
        this->unlock();
    }

    inline size_t getBufferSize()
    {
        size_t result;
        this->lock();
        result = this->bq->getCursor();
        this->unlock();
        return  result;
    }
};

extern "C" void* sound_create() { return new Context; }
extern "C" void sound_destroy(void* ctx) { delete (Context*)ctx; }
extern "C" void sound_enqueue(void* ctx, void* buffer, size_t size) { ((Context*)ctx)->enqueue(buffer, size); }
extern "C" size_t sound_buffer_left(void* ctx) { return ((Context*)ctx)->getBufferSize(); }
