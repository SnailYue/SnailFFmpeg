//
// Created by surface on 2020/9/5.
//

#ifndef SNAILFFMPEG_VIDEOSCREENSHOT_H
#define SNAILFFMPEG_VIDEOSCREENSHOT_H

#include <string>
#include <iostream>

extern "C" {

#include <libavutil/time.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>

}

using namespace std;

static int64_t lastReadPacketTime = 0;

class VideoScreenshot {
public:

    AVFormatContext *inputContext;
    AVFormatContext *outputContext;

    static int interrupt_cb(void *ctx);
    int openInput(string inputUrl);
    shared_ptr<AVPacket> readPacketFromSource();
    int openOutput(string outUrl);
    void init();
    void closeInput();
    void closeOutput();
    int writePacket(shared_ptr<AVPacket> packet);
    int initDecodeContext(AVStream *inputStream);
    int initEncoderCodec(AVStream *inputStream, AVCodecContext **encodeContext);
    bool decode(AVStream *inputStream, AVPacket *packet, AVFrame *frame);
    shared_ptr<AVPacket> encode(AVCodecContext *encodeContext, AVFrame *frame);
    int screenShot(string url, string output_name);
};

#endif //SNAILFFMPEG_VIDEOSCREENSHOT_H
