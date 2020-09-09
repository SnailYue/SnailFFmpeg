//
// Created by surface on 2020/9/9.
//

#ifndef SNAILFFMPEG_VIDEODECODE_H
#define SNAILFFMPEG_VIDEODECODE_H

#include <string>
#include "LogUtil.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

using namespace std;

class VideoDecode {

public:

    static AVFormatContext *avFormatContext;
    static int videoStream;
    static AVCodecContext *avCodecContext;
    static AVCodec *avCodec;
    static AVFrame *avFrame;
    static AVFrame *avFrameRGBA;
    static uint8_t *buffer;

    static int init_ffmpeg(const char *url);

    static int find_decoder();

    static int release();

    static int play_video(const char *url);
};

#endif //SNAILFFMPEG_VIDEODECODE_H
