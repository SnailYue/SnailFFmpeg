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
    static uint8_t *outputBuffer;
    static size_t outputBufferSize;

    static AVPacket packet;
    static int audioStream;
    static AVFrame *aFrame;

    static SwrContext *swrContext;
    static AVFormatContext *avFormatContext;
    static AVCodecContext *avCodecContext;
    static int init_ffmpeg(int *rate, int *channel, char *url);

    static int get_pcm(void **pcm, size_t *pcmsize);

    static int release_ffmpeg();
};

#endif //SNAILFFMPEG_AUDIODECODE_H
