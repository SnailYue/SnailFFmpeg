//
// Created by surface on 2020/9/16.
//

#ifndef SNAILFFMPEG_MEDIAPLAYER_H
#define SNAILFFMPEG_MEDIAPLAYER_H

#include <string>
#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "LogUtil.h"

extern "C" {

#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

}
using namespace std;

class MediaPlayer {
public:

    AVFormatContext *pFormatContext;
    int videoStream;
    AVCodecContext *pCodecContext;
    AVCodec *pCodec;
    ANativeWindow *nativeWindow;
    int videoWidth;
    int videoHeight;
    ANativeWindow_Buffer windowBuffer;
    AVFrame *pFrameRGBA;
    AVFrame *pFrame;
    int numBytes;
    uint8_t *buffer;
    struct SwsContext *swsContext;
    int frameFinished;
    AVPacket packet;


    int init(JNIEnv *env, string filePath, jobject surface);

    int play();

    int release();

};


#endif //SNAILFFMPEG_MEDIAPLAYER_H
