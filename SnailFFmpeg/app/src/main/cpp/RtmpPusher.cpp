//
// Created by surface on 2020/8/16.
//

#include <jni.h>
#include "VideoStream.h"
#include "AudioStream.h"


JavaVM *javaVM;

VideoStream *videoStream = 0;
int isStart = 0;
pthread_t pid;
int readyPushing = 0;
u_int32_t start_time = 0;

AudioStream *audioStream = 0;


jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    javaVM = vm;
    return JNI_VERSION_1_6;
}

void callback(RTMPPacket *packet) {
    if (packet) {
        packet->m_nTimeStamp = RTMP_GetTime() - start_time;
    }
}

void releasePackets(RTMPPacket *packet) {
    if (packet) {
        RTMPPacket_Free(packet);
        delete packet;
        packet = 0;
    }
}

void *start(void *args) {
    char *url = static_cast<char *>(args);
    RTMP *rtmp = 0;
    do {

    }while (0);
    isStart = 0;
    readyPushing = 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1init(JNIEnv *env, jobject thiz) {
    videoStream = new VideoStream;
    videoStream->setVideoCallback()
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
