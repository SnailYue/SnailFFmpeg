//
// Created by surface on 2020/6/20.
//

#ifndef FFMPEG_NATIVE_LIB_H
#define FFMPEG_NATIVE_LIB_H

#include <jni.h>
#include <string>
#include <android/log.h>


extern "C" {

//编码

#include "libavcodec/avcodec.h"

//封装格式处理

#include "libavformat/avformat.h"

//像素处理

#include "libswscale/swscale.h"

#include <android/native_window_jni.h>

#include <unistd.h>

JNIEXPORT jstring
JNICALL Java_com_test_ffmpeg_MainActivity_stringFromJNI(JNIEnv *env, jobject /* this */);

}

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"jason",FORMAT,##__VA_ARGS__);

#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"jason",FORMAT,##__VA_ARGS__);


#endif //FFMPEG_NATIVE_LIB_H
