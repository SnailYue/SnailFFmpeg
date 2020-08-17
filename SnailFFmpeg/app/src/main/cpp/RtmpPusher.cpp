//
// Created by surface on 2020/8/16.
//

#include <jni.h>
#include "VideoStream.h"
#include "AudioStream.h"
#include "SafeQueue.h"


JavaVM *javaVM;

VideoStream *videoStream = 0;
int isStart = 0;
pthread_t pid;
int readyPushing = 0;
u_int32_t start_time = 0;

AudioStream *audioStream = 0;

SafeQueue<RTMPPacket *> packets;

jobject jobject_error;


jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    javaVM = vm;
    return JNI_VERSION_1_6;
}

void throwErrToJava(int error_code) {
    JNIEnv *env;
    javaVM->AttachCurrentThread(&env, NULL);
    jclass classError = env->GetObjectClass(jobject_error);
    jmethodID methodErr = env->GetMethodID(classError, "errorFromNative", "(I)V");
    env->CallVoidMethod(jobject_error, methodErr, error_code);
    javaVM->DetachCurrentThread();

}

/**
 * 往队列中存放packet
 * @param packet
 */
void callback(RTMPPacket *packet) {
    if (packet) {
        packet->m_nTimeStamp = RTMP_GetTime() - start_time;
        packets.push(packet);
    }
}

/**
 * 释放packet
 * @param packet
 */
void releasePackets(RTMPPacket *&packet) {
    if (packet) {
        RTMPPacket_Free(packet);
        delete packet;
        packet = 0;
    }
}

/**
 * 线程任务
 * @param args
 * @return
 */
void *start(void *args) {
    char *url = static_cast<char *>(args);
    RTMP *rtmp = 0;
    do {
        rtmp = RTMP_Alloc();
        if (!rtmp) {
            break;
        }
        RTMP_Init(rtmp);
        int ret = RTMP_SetupURL(rtmp, url);
        if (!ret) {
            break;
        }
        rtmp->Link.timeout = 5;
        RTMP_EnableWrite(rtmp);
        ret = RTMP_Connect(rtmp, 0);
        if (!ret) {
            break;
        }
        ret = RTMP_ConnectStream(rtmp, 0);
        if (!ret) {
            break;
        }
        start_time = RTMP_GetTime();
        readyPushing = 1;
        packets.setWork(1);
        callback(audioStream->getAudioTag());
        RTMPPacket *packet = 0;
        while (readyPushing) {
            packets.pop(packet);
            if (!readyPushing) {
                break;
            }
            if (!packet) {
                continue;
            }
            packet->m_nInfoField2 = rtmp->m_stream_id;
            ret = RTMP_SendPacket(rtmp, packet, 1);
            if (!ret) {
                break
            }
        }
        releasePackets(packet);
    } while (0);
    isStart = 0;
    readyPushing = 0;
    packets.setWork(0);
    packets.clear();
    if (rtmp) {
        RTMP_Close(rtmp);
        RTMP_Free(rtmp);
    }
    delete (url);
    return 0;
}

/**
 * native初始化
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1init(JNIEnv *env, jobject thiz) {
    videoStream = new VideoStream;
    videoStream->setVideoCallback(callback);
    audioStream = new AudioStream;
    audioStream->setAudioCallback(callback);

    packets.setReleaseCallback(releasePackets);
    jobject_error = env->NewGlobalRef(thiz);
}

/**
 * 开始推流
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1start(JNIEnv *env, jobject thiz, jstring path) {
    if (isStart) {
        return;
    }
    isStart = 1;
    const char *_path = env->GetStringUTFChars(path, 0);
    char *url = new char[strlen(_path) + 1];
    strcpy(url, _path);
    pthread_create(&pid, 0, start, url);
    env->ReleaseStringUTFChars(path, _path);
}

/**
 * 获取inputSample
 */
extern "C"
JNIEXPORT jint JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_getInputSamples(JNIEnv *env, jobject thiz) {
    if (audioStream) {
        return audioStream->getInputSamples();
    }
    return -1;
}

/**
 * 推音频流
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1pushAudio(JNIEnv *env, jobject thiz,
                                                             jbyteArray data) {
    if (!audioStream || !readyPushing) {
        return;
    }
    jbyte *_data = env->GetByteArrayElements(data, 0);
    audioStream->encodeData(_data);
    env->ReleaseByteArrayElements(data, _data, 0);
}

/**
 * 推视频流
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1pushVideo(JNIEnv *env, jobject thiz,
                                                             jbyteArray data) {
    if (!videoStream || !readyPushing) {
        return;
    }
    jbyte *_data = env->GetByteArrayElements(data, NULL);
    videoStream->encodeData(_data);
    env->ReleaseByteArrayElements(data, _data, 0);
}

/**
 * 释放
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1release(JNIEnv *env, jobject thiz) {
    env->DeleteGlobalRef(jobject_error);
    delete videoStream;
    delete audioStream;
}

/**
 * 设置音频流信息
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1setAudioCodecInfo(JNIEnv *env, jobject thiz,
                                                                     jint sample_rate,
                                                                     jint channels) {
    if (audioStream) {
        audioStream->setAudioEncInfo(sample_rate, channels);
    }
}

/**
 * 设置视频流信息
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1setVideoCodecInfo(JNIEnv *env, jobject thiz,
                                                                     jint width, jint height,
                                                                     jint fps, jint bitrate) {
    if (videoStream) {
        videoStream->setVideoEncInfo(width, height, fps, bitrate);
    }
}

/**
 * 停止推流
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_live_LivePusher_native_1stop(JNIEnv *env, jobject thiz) {
    readyPushing = 0;
    packets.setWork(0);
    pthread_join(pid, 0);
}
