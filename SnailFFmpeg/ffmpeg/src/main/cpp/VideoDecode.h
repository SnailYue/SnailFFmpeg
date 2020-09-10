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

    AVFormatContext *avFormatContext;
    int videoStream;
    AVCodecContext *avCodecContext;
    AVCodec *avCodec;
    AVFrame *avFrame;
    AVFrame *avFrameRGBA;
    uint8_t *buffer;


    int init_ffmpeg(const char *url);
    int find_decoder();
    int release();
    int play_video(const char *url);
};

#endif //SNAILFFMPEG_VIDEODECODE_H
