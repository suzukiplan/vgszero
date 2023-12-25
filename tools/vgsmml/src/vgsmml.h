/**
 * 東方BGM on VGS for RasberryPi Pico
 * License under GPLv3: https://github.com/suzukiplan/tohovgs-pico/blob/master/LICENSE.txt
 */

#ifndef INCLUDE_VGSMML_H
#define INCLUDE_VGSMML_H

#define VGSMML_ERR_SUCCESS 0
#define VGSMML_ERR_INVALID 1
#define VGSMML_ERR_FILE_IO 2
#define VGSMML_ERR_NO_MEMORY 3
#define VGSMML_ERR_SYNTAX_MACRO 100
#define VGSMML_ERR_SYNTAX_CHANNEL 101
#define VGSMML_ERR_SYNTAX_UNKNOWN 102

#ifdef __cplusplus
extern "C" {
#endif

struct VgsBgmData {
    void* data;
    size_t size;
};

struct VgsMmlErrorInfo {
    int code; /* error code */
    int line; /* MML's line number (0: not syntax error) */
    char message[248];
};

#ifdef _WIN32
struct VgsBgmData* __stdcall vgsmml_compile_from_file(const char* path, struct VgsMmlErrorInfo* err);
struct VgsBgmData* __stdcall vgsmml_compile_from_memory(void* data, size_t size, struct VgsMmlErrorInfo* err);
struct VgsBgmData* __stdcall vgsmml_compile_from_memory2(const void* data, size_t size, struct VgsMmlErrorInfo* err);
void __stdcall vgsmml_free_bgm_data(struct VgsBgmData* data);
#else
struct VgsBgmData* vgsmml_compile_from_file(const char* path, struct VgsMmlErrorInfo* err);
struct VgsBgmData* vgsmml_compile_from_memory(void* data, size_t size, struct VgsMmlErrorInfo* err);
struct VgsBgmData* vgsmml_compile_from_memory2(const void* data, size_t size, struct VgsMmlErrorInfo* err);
void vgsmml_free_bgm_data(struct VgsBgmData* data);
#endif

#ifdef __cplusplus
};
#endif

#endif
