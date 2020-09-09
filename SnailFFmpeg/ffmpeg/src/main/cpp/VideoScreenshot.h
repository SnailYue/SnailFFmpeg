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

class VideoScreenshot {
public:

    static AVFormatContext *inputContext;
    static AVFormatContext *outputContext;

    static int64_t lastReadPacketTime;

    static int interrupt_cb(void *ctx);
    static int openInput(string inputUrl);
    static shared_ptr<AVPacket> readPacketFromSource();
    static int openOutput(string outUrl);
    static void init();
    static void closeInput();
    static void closeOutput();
    static int writePacket(shared_ptr<AVPacket> packet);
    static int initDecodeContext(AVStream *inputStream);
    static int initEncoderCodec(AVStream *inputStream, AVCodecContext **encodeContext);
    static bool decode(AVStream *inputStream, AVPacket *packet, AVFrame *frame);
    static shared_ptr<AVPacket> encode(AVCodecContext *encodeContext, AVFrame *frame);
    static int screenShot(string url, string output_name);
};

#endif //SNAILFFMPEG_VIDEOSCREENSHOT_H
