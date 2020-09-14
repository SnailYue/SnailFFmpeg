//
// Created by surface on 2020/9/14.
//

#ifndef SNAILFFMPEG_MEDIAENCODER_H
#define SNAILFFMPEG_MEDIAENCODER_H

#include "LogUtil.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/time.h>
}

class MediaEncoder {
public:
    AVFormatContext *ofmtContext;
    AVStream *videoStream;
    AVCodecContext *codecContext;
    AVCodec *avCodec;
    AVPacket avPacket;
    AVFrame *avFrame;
    int frameCount = 0;
    int yuvWidth;
    int yuvHeight;
    int yLength;
    int uvLength;
    int64_t startTime;

    static void custom_log(void *ptr, int livel, const char *fmt, va_list vl);

    int init(int width, int height);

    int encode(int8_t *data);

    int flush();

    int close();

};


#endif //SNAILFFMPEG_MEDIAENCODER_H
