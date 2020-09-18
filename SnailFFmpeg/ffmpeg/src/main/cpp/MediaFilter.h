//
// Created by surface on 9/17/2020.
//

#ifndef SNAILFFMPEG_MEDIAFILTER_H
#define SNAILFFMPEG_MEDIAFILTER_H


#include <string>
#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "LogUtil.h"

extern "C" {

#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavfilter/buffersrc.h>

}

using namespace std;

static enum AVPixelFormat pixFormats[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE};

static char *filtersDescr = "lutyuv='u=128:v=128'";

class MediaFilter {

public:
    AVFilterContext *bufferSinkContext;
    AVFilterContext *bufferSrcContext;
    AVFilterGraph *filterGraph;
    AVFormatContext *avFormatContext;
    AVCodecContext *avCodecContext;
    int videoStream;
    AVFilter *bufferSrc;
    AVFilter *bufferSink;
    AVFilterInOut *outputs;
    AVFilterInOut *inputs;
    char args[512];
    AVBufferSinkParams *bufferSinkParams;
    AVCodec *avCodec;
    ANativeWindow *nativeWindow;
    int videoWidth, videoHeight;
    ANativeWindow_Buffer windowBuffer;
    AVFrame *avFrame, *avFrameRGBA;
    int numBytes;
    uint8_t *buffer;
    struct SwsContext *swsContext;
    int frameFinished;
    AVPacket packet;

    //function
    int init(JNIEnv *env, string url, jobject surface);

    int play();

    int release();

};


#endif //SNAILFFMPEG_MEDIAFILTER_H
