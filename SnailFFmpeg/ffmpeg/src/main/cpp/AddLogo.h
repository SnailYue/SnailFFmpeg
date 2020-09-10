//
// Created by surface on 2020/9/6.
//

#ifndef SNAILFFMPEG_ADDLOGO_H
#define SNAILFFMPEG_ADDLOGO_H


#include <iostream>
#include <thread>
#include "LogUtil.h"

extern "C" {

#include <libavutil/time.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>

#include "include/libavformat/avformat.h"
#include "include/libavfilter/avfilter.h"
#include "include/libavdevice/avdevice.h"

}

using namespace std;


static char *filter_descr = "overlay=100:100";


class AddLogo {

public:

    AVFormatContext *context[2];
    AVFormatContext *opCtx;

    AVCodecContext *outPutEncContext;
    AVCodecContext *decoderContext[2];

    AVFilterInOut *inputs;
    AVFilterInOut *outputs;
    AVFilterGraph *filter_graph;

    AVFilterContext *inputFilterContext[2];
    AVFilterContext *outputFilterContext;


    static int interrupt_cb(void *context);

    void init_register();

    int open_input(char *fileName, int inputIndex);

    shared_ptr<AVPacket> read_packet_from_source(int inputIndex);

    int open_output(char *fileName, int inputIndex);

    void close_input(int inputIndex);

    void close_output();

    int init_encoder_codec(int width, int height, int inputIndex);

    int init_decode_codec(AVCodecID codecId, int inputIndex);

    bool decode_video(AVPacket *packet, AVFrame *frame, int inputIndex);

    int init_input_filter(AVFilterInOut *input, const char *filterName, int inputIndex);

    int init_output_filter(AVFilterInOut *output, const char *filterName);

    void free_inout();

    void end();

    int add_logo(string videoUrl, string pictureUrl, string outputName);
};

#endif //SNAILFFMPEG_ADDLOGO_H
