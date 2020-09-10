//
// Created by surface on 2020/9/8.
//

#ifndef SNAILFFMPEG_AUDIODECODE_H
#define SNAILFFMPEG_AUDIODECODE_H

#include <cstdint>
#include "LogUtil.h"

extern "C" {

#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>

}

class AudioDecode {
    uint8_t *outputBuffer;
    size_t outputBufferSize;

    AVPacket packet;
    int audioStream;
    AVFrame *aFrame;

    SwrContext *swrContext;
    AVFormatContext *avFormatContext;
    AVCodecContext *avCodecContext;
    int init_ffmpeg(int *rate, int *channel, char *url);

    int get_pcm(void **pcm, size_t *pcmsize);

    int release_ffmpeg();
};

#endif //SNAILFFMPEG_AUDIODECODE_H
