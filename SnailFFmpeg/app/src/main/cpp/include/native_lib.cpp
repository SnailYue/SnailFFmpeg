//
// Created by surface on 2020/6/20.
//

#include "native_lib.h"

extern "C" jstring Java_com_test_ffmpeg_MainActivity_stringFromJNI(JNIEnv *env, jobject /* this */) {

    std::string hello = "Hello from C++";

    av_register_all();

    return env->NewStringUTF(hello.c_str());

}


