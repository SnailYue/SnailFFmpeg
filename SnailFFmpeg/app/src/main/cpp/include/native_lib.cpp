//
// Created by surface on 2020/6/20.
//

#include "native_lib.h"
#include "init_lib.cpp"

using namespace std;

extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_native_KotlinToJNI_00024Companion_initFFmpeg(JNIEnv *env, jobject thiz) {
    initFFmpeg();
}