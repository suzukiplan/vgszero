//
//  sound-ios
//  Sound System for iOS (AudioQueue)
//
//  Created by 鈴木　洋司　 on 2018/02/28.
//  Copyright © 2018年 SUZUKI PLAN. All rights reserved.
//

#ifndef sound_ios_h
#define sound_ios_h
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void* sound_create(void);
void sound_destroy(void* context);
void sound_enqueue(void* context, void* buffer, size_t size);
size_t sound_buffer_left(void* context);

#ifdef __cplusplus
};
#endif

#endif /* sound_ios_h */
