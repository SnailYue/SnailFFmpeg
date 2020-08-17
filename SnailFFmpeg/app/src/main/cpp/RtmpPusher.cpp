//
// Created by surface on 2020/8/16.
//

#include <jni.h>


extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1init(JNIEnv *env, jobject thiz) {
    // TODO: implement native_init()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1start(JNIEnv *env, jobject thiz, jstring path) {
    // TODO: implement native_start()
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_getInputSamples(JNIEnv *env, jobject thiz) {
    // TODO: implement getInputSamples()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1pushAudio(JNIEnv *env, jobject thiz,
                                                             jbyteArray data) {
    // TODO: implement native_pushAudio()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1pushVideo(JNIEnv *env, jobject thiz,
                                                             jbyteArray data) {
    // TODO: implement native_pushVideo()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1release(JNIEnv *env, jobject thiz) {
    // TODO: implement native_release()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1setAudioCodecInfo(JNIEnv *env, jobject thiz,
                                                                     jint sample_rate,
                                                                     jint channels) {
    // TODO: implement native_setAudioCodecInfo()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1setVideoCodecInfo(JNIEnv *env, jobject thiz,
                                                                     jint width, jint height,
                                                                     jint fps, jint bitrate) {
    // TODO: implement native_setVideoCodecInfo()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1stop(JNIEnv *env, jobject thiz) {
    // TODO: implement native_stop()
}
