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


const char *filter_descr = "overlay=100:100";


class AddLogo {
public:

    static AVFormatContext *context[2];
    static AVFormatContext *opCtx;


    static AVCodecContext *outPutEncContext;
    static AVCodecContext *decoderContext[2];

    static AVFilterInOut *inputs;
    static AVFilterInOut *outputs;
    static AVFilterGraph *filter_graph;

    static AVFilterContext *inputFilterContext[2];
    static AVFilterContext *outputFilterContext;

    static int interrupt_cb(void *context);

    static void init_register();

    static int open_input(char *fileName, int inputIndex);

    static shared_ptr<AVPacket> read_packet_from_source(int inputIndex);

    static int open_output(char *fileName, int inputIndex);

    static void close_input(int inputIndex);

    static void close_output();

    static int init_encoder_codec(int width, int height, int inputIndex);

    static int init_decode_codec(AVCodecID codecId, int inputIndex);

    static bool decode_video(AVPacket *packet, AVFrame *frame, int inputIndex);

    static int init_input_filter(AVFilterInOut *input, const char *filterName, int inputIndex);

    static int init_output_filter(AVFilterInOut *output, const char *filterName);

    static void free_inout();

    static void end();

    static int add_logo(string videoUrl, string pictureUrl, string outputName);
};

#endif //SNAILFFMPEG_ADDLOGO_H
