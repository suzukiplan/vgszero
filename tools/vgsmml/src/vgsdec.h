/**
 * 東方BGM on VGS for RasberryPi Pico
 * License under GPLv3: https://github.com/suzukiplan/tohovgs-pico/blob/master/LICENSE.txt
 */

#ifndef INCLUDE_VGSDEC_H
#define INCLUDE_VGSDEC_H

#define VGSDEC_REG_KEY_0 0x10
#define VGSDEC_REG_KEY_1 0x11
#define VGSDEC_REG_KEY_2 0x12
#define VGSDEC_REG_KEY_3 0x13
#define VGSDEC_REG_KEY_4 0x14
#define VGSDEC_REG_KEY_5 0x15
#define VGSDEC_REG_TONE_0 0x20
#define VGSDEC_REG_TONE_1 0x21
#define VGSDEC_REG_TONE_2 0x22
#define VGSDEC_REG_TONE_3 0x23
#define VGSDEC_REG_TONE_4 0x24
#define VGSDEC_REG_TONE_5 0x25
#define VGSDEC_REG_VOL_0 0x30
#define VGSDEC_REG_VOL_1 0x31
#define VGSDEC_REG_VOL_2 0x32
#define VGSDEC_REG_VOL_3 0x33
#define VGSDEC_REG_VOL_4 0x34
#define VGSDEC_REG_VOL_5 0x35
#define VGSDEC_REG_PLAYING 0x40
#define VGSDEC_REG_INDEX 0x50
#define VGSDEC_REG_LOOP_INDEX 0x51
#define VGSDEC_REG_LENGTH 0x52
#define VGSDEC_REG_TIME 0x60
#define VGSDEC_REG_LOOP_TIME 0x61
#define VGSDEC_REG_TIME_LENGTH 0x62
#define VGSDEC_REG_LOOP_COUNT 0x70
#define VGSDEC_REG_RESET 0x80
#define VGSDEC_REG_FADEOUT 0x90
#define VGSDEC_REG_FADEOUT_COUNTER 0xA0
#define VGSDEC_REG_VOLUME_RATE_0 0x100
#define VGSDEC_REG_VOLUME_RATE_1 0x101
#define VGSDEC_REG_VOLUME_RATE_2 0x102
#define VGSDEC_REG_VOLUME_RATE_3 0x103
#define VGSDEC_REG_VOLUME_RATE_4 0x104
#define VGSDEC_REG_VOLUME_RATE_5 0x105
#define VGSDEC_REG_VOLUME_RATE 0x200
#define VGSDEC_REG_SYNTHESIS_BUFFER 0x300
#define VGSDEC_REG_KEYON_0 0x400
#define VGSDEC_REG_KEYON_1 0x401
#define VGSDEC_REG_KEYON_2 0x402
#define VGSDEC_REG_KEYON_3 0x403
#define VGSDEC_REG_KEYON_4 0x404
#define VGSDEC_REG_KEYON_5 0x405
#define VGSDEC_REG_MUTE_0 0x500
#define VGSDEC_REG_MUTE_1 0x501
#define VGSDEC_REG_MUTE_2 0x502
#define VGSDEC_REG_MUTE_3 0x503
#define VGSDEC_REG_MUTE_4 0x504
#define VGSDEC_REG_MUTE_5 0x505
#define VGSDEC_REG_ADD_KEY_0 0x600
#define VGSDEC_REG_ADD_KEY_1 0x601
#define VGSDEC_REG_ADD_KEY_2 0x602
#define VGSDEC_REG_ADD_KEY_3 0x603
#define VGSDEC_REG_ADD_KEY_4 0x604
#define VGSDEC_REG_ADD_KEY_5 0x605
#define VGSDEC_REG_KOBUSHI 0x700

#ifdef __cplusplus
extern "C" {
#endif

struct VgsMetaHeader {
    char eyecatch[8];
    char format[8];
    char genre[32];
    unsigned char num;
    unsigned char reserved1[3];
    unsigned short secIi;
    unsigned short secIf;
    unsigned short secLi;
    unsigned short secLf;
    unsigned int reserved2;
};

struct VgsMetaData {
    unsigned short year;
    unsigned short aid;
    unsigned short track;
    unsigned short reserved;
    char album[56];
    char song[64];
    char team[32];
    char creator[32];
    char right[32];
    char code[32];
};

#ifdef _WIN32
void* __stdcall vgsdec_create_context();
int __stdcall vgsdec_load_bgm_from_file(void* context, const char* path);
int __stdcall vgsdec_load_bgm_from_memory(void* context, void* data, size_t size);
void __stdcall vgsdec_execute(void* context, void* buffer, size_t size);
int __stdcall vgsdec_get_value(void* context, int type);
void __stdcall vgsdec_set_value(void* context, int type, int value);
void __stdcall vgsdec_release_context(void* context);
struct VgsMetaHeader* __stdcall vgsdec_get_meta_header(void* context);
struct VgsMetaData* __stdcall vgsdec_get_meta_data(void* context, int index);
int __stdcall vgsdec_async_start(void* context);
int __stdcall vgsdec_async_enqueue(void* context, void* buffer, size_t size, void (*callback)(void* context, void* buffer, size_t size));
void __stdcall vgsdec_async_stop(void* context);
#else
void* vgsdec_create_context();
int vgsdec_load_bgm_from_file(void* context, const char* path);
int vgsdec_load_bgm_from_memory(void* context, void* data, size_t size);
void vgsdec_execute(void* context, void* buffer, size_t size);
int vgsdec_get_value(void* context, int type);
void vgsdec_set_value(void* context, int type, int value);
void vgsdec_release_context(void* context);
struct VgsMetaHeader* vgsdec_get_meta_header(void* context);
struct VgsMetaData* vgsdec_get_meta_data(void* context, int index);
int vgsdec_async_start(void* context);
int vgsdec_async_enqueue(void* context, void* buffer, size_t size, void (*callback)(void* context, void* buffer, size_t size));
void vgsdec_async_stop(void* context);
#endif

#ifdef __cplusplus
};
#endif

#endif
