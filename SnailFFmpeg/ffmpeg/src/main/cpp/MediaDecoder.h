//
// Created by surface on 2020/9/15.
//

#ifndef SNAILFFMPEG_MEDIADECODER_H
#define SNAILFFMPEG_MEDIADECODER_H


extern "C" {

#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

}

#include <cstdarg>
#include <regex>

#include "LogUtil.h"

using namespace std;

class MediaDecoder {
public:
    AVFormatContext *pFormatContext;
    int videoIndex;
    AVCodecContext *pCodecContext;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameYUV;
    uint8_t *outBuffer;
    AVPacket *packet;
    int ySize;
    int gotPicture;
    struct SwsContext *imgConvertContext;
    FILE *fpYUV;
    int frameCount;


    static void custom_log(void *ptr, int level, const char *fmt, va_list vl);

    int init(string input, string output);

    int decode();

    int release();

};


#endif //SNAILFFMPEG_MEDIADECODER_H
