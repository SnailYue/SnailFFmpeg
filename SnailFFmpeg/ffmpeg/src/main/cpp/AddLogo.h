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

AVFormatContext *context[2];
AVFormatContext *opCtx;


AVCodecContext *outPutEncContext = NULL;
AVCodecContext *decoderContext[2];

AVFilterInOut *inputs;
AVFilterInOut *outputs;
AVFilterGraph *filter_graph = nullptr;

AVFilterContext *inputFilterContext[2];
AVFilterContext *outputFilterContext = nullptr;

const char *filter_descr = "overlay=100:100";


/**
 * 设置callback值
 * @param context
 * @return
 */
int _interrupt_cb(void *context) {
    return 0;
}

/**
 * 初始化注册
 */
void init_register() {
    LOGI("init_register");
    av_register_all();
    avfilter_register_all();
    avformat_network_init();
    avdevice_register_all();
    av_log_set_level(AV_LOG_ERROR);
}

/**
 * 打开输入
 * @param fileName
 * @param inputIndex
 * @return
 */
int open_input(char *fileName, int inputIndex) {
    LOGI("open_input");
    context[inputIndex] = avformat_alloc_context();
    context[inputIndex]->interrupt_callback.callback = _interrupt_cb;
    AVDictionary *format_opts = nullptr;

    int ret = avformat_open_input(&context[inputIndex], fileName, nullptr, &format_opts);
    if (ret < 0) {
        LOGE("Open input file failed");
        return ret;
    }

    ret = avformat_find_stream_info(context[inputIndex], nullptr);
    av_dump_format(context[inputIndex], 0, fileName, 0);
    if (ret >= 0) {
        LOGI("Open input stream successfully");
    }
    return ret;
}

/**
 * 读取packet
 * @param inputIndex
 * @return
 */
shared_ptr<AVPacket> read_packet_from_source(int inputIndex) {
    LOGI("read_packet_from_source");
    shared_ptr<AVPacket> packet(static_cast<AVPacket *>(av_malloc(sizeof(AVPacket))),
                                [&](AVPacket *p) {
                                    av_packet_free(&p);
                                    av_freep(&p);
                                });
    av_init_packet(packet.get());
    int ret = av_read_frame(context[inputIndex], packet.get());
    if (ret >= 0) {
        return packet;
    } else {
        LOGE("read frame failed");
        return nullptr;
    }
}

/**
 * 打开输出
 * @param fileName
 * @param inputIndex
 * @return
 */
int open_output(char *fileName, int inputIndex) {
    LOGI("open_output");
    int ret = avformat_alloc_output_context2(&opCtx, nullptr, "mpegts", fileName);
    if (ret < 0) {
        LOGE("alloc output context failed");
        goto Error;
    }
    ret = avio_open2(&opCtx->pb, fileName, AVIO_FLAG_READ_WRITE, nullptr, nullptr);
    if (ret < 0) {
        LOGE("avio open failed");
        goto Error;
    }
    for (int i = 0; i < context[inputIndex]->nb_streams; ++i) {
        AVStream *stream = avformat_new_stream(opCtx, outPutEncContext->codec);
        stream->codec = outPutEncContext;
        if (ret < 0) {
            goto Error;
        }
    }
    av_dump_format(opCtx, 0, fileName, 1);
    ret = avformat_write_header(opCtx, nullptr);
    if (ret < 0) {
        LOGE("write header failed");
        goto Error;
    }
    if (ret >= 0) {
        LOGE("Open output stream successfully");
    }
    return ret;

    Error :
    if (opCtx) {
        avformat_close_input(&opCtx);
    }
}

/**
 * 关闭输入
 * @param inputIndex
 */
void close_input(int inputIndex) {
    LOGI("close_input");
    if (context != nullptr) {
        avformat_close_input(&context[inputIndex]);
    }
}

/**
 * 关闭输出
 */
void close_output() {
    LOGI("close_output");
    if (opCtx != nullptr) {
        for (int i = 0; i < opCtx->nb_streams; ++i) {
            AVCodecContext *codecContext = opCtx->streams[i]->codec;
            avcodec_close(codecContext);
        }
        avformat_close_input(&opCtx);
    }
}

/**
 * 初始化encoder
 * @param width
 * @param height
 * @param inputIndex
 * @return
 */
int init_encoder_codec(int width, int height, int inputIndex) {
    LOGI("init_encoder_codec");
    /**
     * 此处需要在ffmpeg的编译过程中开启h264的支持，不然会返回NULL;
     */
    AVCodec *pH264Codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (pH264Codec == NULL) {
        LOGE("avcodec find encoder failed");
        return -1;
    }
    outPutEncContext = avcodec_alloc_context3(pH264Codec);
    outPutEncContext->gop_size = 30;
    outPutEncContext->has_b_frames = 0;
    outPutEncContext->max_b_frames = 0;
    outPutEncContext->codec_id = pH264Codec->id;
    outPutEncContext->time_base.num = context[inputIndex]->streams[0]->codec->time_base.num;
    outPutEncContext->time_base.den = context[inputIndex]->streams[0]->codec->time_base.den;
    outPutEncContext->pix_fmt = *pH264Codec->pix_fmts;
    outPutEncContext->width = width;
    outPutEncContext->height = height;

    outPutEncContext->me_subpel_quality = 0;
    outPutEncContext->refs = 1;
    outPutEncContext->scenechange_threshold = 0;
    outPutEncContext->trellis = 0;
    AVDictionary *options = nullptr;
    outPutEncContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    int ret = avcodec_open2(outPutEncContext, pH264Codec, &options);
    if (ret < 0) {
        LOGE("avcodec open2 failed");
        return ret;
    }
    return 1;
}


/**
 * 初始化decoder
 * @param codecId
 * @param inputIndex
 * @return
 */
int init_decode_codec(AVCodecID codecId, int inputIndex) {
    LOGI("init_decode_codec");
    auto codec = avcodec_find_decoder(codecId);
    if (!codec) {
        LOGE("find decoder failed");
        return -1;
    }
    decoderContext[inputIndex] = context[inputIndex]->streams[0]->codec;
    if (!decoderContext) {
        LOGE("decoderContext is failed");
        exit(1);
    }
    if (codec->capabilities & AV_CODEC_CAP_TRUNCATED) {
        decoderContext[inputIndex]->flags |= AV_CODEC_FLAG_TRUNCATED;
    }
    int ret = avcodec_open2(decoderContext[inputIndex], codec, NULL);
    return ret;
}


/**
 * 解码Video
 * @param packet
 * @param frame
 * @param inputIndex
 * @return
 */
bool decode_video(AVPacket *packet, AVFrame *frame, int inputIndex) {
    LOGI("decode_video");
    int gotFrame = 0;
    auto hr = avcodec_decode_video2(decoderContext[inputIndex], frame, &gotFrame, packet);
    if (hr >= 0 && gotFrame != 0) {
        frame->pts = packet->pts;
        return true;
    }
    return false;
}

/**
 * 初始化inputFilter
 * @param input
 * @param filterName
 * @param inputIndex
 * @return
 */
int init_input_filter(AVFilterInOut *input, const char *filterName, int inputIndex) {
    LOGI("init_input_filter");
    char args[512];
    memset(args, 0, sizeof(args));
    AVFilterContext *padFilterCOntext = input->filter_ctx;

    auto filter = avfilter_get_by_name("buffer");
    auto codeContext = context[inputIndex]->streams[0]->codec;

    int ret = avfilter_graph_create_filter(&inputFilterContext[inputIndex], filter, filterName,
                                           args, NULL, filter_graph);
    if (ret < 0) {
        LOGE("graph create filter failed");
        return ret;
    }
    ret = avfilter_link(inputFilterContext[inputIndex], 0, padFilterCOntext, input->pad_idx);
    return ret;
}


/**
 * 初始化outputFilter
 * @param output
 * @param filterName
 * @return
 */
int init_output_filter(AVFilterInOut *output, const char *filterName) {
    LOGI("init_output_filter");
    AVFilterContext *padFilterContext = output->filter_ctx;
    auto filter = avfilter_get_by_name("buffersink");
    int ret = avfilter_graph_create_filter(&outputFilterContext, filter, filterName, NULL, NULL,
                                           filter_graph);
    if (ret < 0) {
        LOGE("graph create filter failed");
        return ret;
    }
    ret = avfilter_link(padFilterContext, output->pad_idx, outputFilterContext, 0);
    return ret;
}

/**
 * 释放input
 */
void free_inout() {
    LOGI("free_inout");
    avfilter_inout_free(&inputs->next);
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
}

/**
 * 结束
 */
void end() {
    LOGI("end");
    close_input(0);
    close_input(1);
    close_output();
}

/**
 *主函数
 * @param videoUrl
 * @param pictureUrl
 * @param outputName
 * @return
 */
int add_logo(string videoUrl, string pictureUrl, string outputName) {
    LOGI("add_logo");
    string fileInput[2];
    fileInput[0] = videoUrl;
    fileInput[1] = pictureUrl;
    string fileOutput = outputName;
    std::thread decodeTask;

    init_register();
    for (int i = 0; i < 2; ++i) {
        if (open_input((char *) fileInput[i].c_str(), i) < 0) {
            this_thread::sleep_for(chrono::seconds(10));
            return 0;
        }
    }

    for (int i = 0; i < 2; ++i) {
        int ret = init_decode_codec(context[i]->streams[0]->codec->codec_id, i);
        if (ret < 0) {
            this_thread::sleep_for(chrono::seconds(10));
            return 0;
        }
    }
    int ret = init_encoder_codec(decoderContext[0]->width, decoderContext[0]->height, 0);
    if (ret < 0) {
        this_thread::sleep_for(chrono::seconds(10));
        return 0;
    }
    filter_graph = avfilter_graph_alloc();
    if (!filter_graph) {
        end();
        return -1;
    }

    avfilter_graph_parse2(filter_graph, filter_descr, &inputs, &outputs);
    init_input_filter(inputs, "MainFrame", 0);
    init_input_filter(inputs->next, "OverlayFrame", 1);
    init_output_filter(outputs, "output");
    free_inout();

    ret = avfilter_graph_config(filter_graph, NULL);
    if (ret < 0) {
        end();
        return -1;
    }
    if (open_output((char *) fileOutput.c_str(), 0) < 0) {
        this_thread::sleep_for(chrono::seconds(10));
        return 0;
    }

    AVFrame *pSrcFrame[2];
    AVFrame *inputFrame[2];
    pSrcFrame[0] = av_frame_alloc();
    pSrcFrame[1] = av_frame_alloc();
    inputFrame[0] = av_frame_alloc();
    inputFrame[1] = av_frame_alloc();

    auto filterFrame = av_frame_alloc();

    int got_output = 0;

    while (true) {
        auto packet = read_packet_from_source(1);
        ret = decode_video(packet.get(), pSrcFrame[1], 1);
        if (ret) {
            break;
        }
    }

    while (true) {
        auto packet = read_packet_from_source(0);
        if (packet) {
            if (decode_video(packet.get(), pSrcFrame[0], 0)) {
                av_frame_ref(inputFrame[0], pSrcFrame[0]);
                if (av_buffersrc_add_frame_flags(inputFilterContext[0], inputFrame[0],
                                                 AV_BUFFERSRC_FLAG_PUSH) >= 0) {
                    pSrcFrame[1]->pts = pSrcFrame[0]->pts;
                    if (av_buffersrc_add_frame_flags(inputFilterContext[1], pSrcFrame[1],
                                                     AV_BUFFERSRC_FLAG_PUSH) >= 0) {
                        ret = av_buffersink_get_frame_flags(outputFilterContext, filterFrame,
                                                            AV_BUFFERSINK_FLAG_NO_REQUEST);
                        if (ret >= 0) {
                            shared_ptr<AVPacket> pTmpPkt(
                                    static_cast<AVPacket *>(av_malloc(sizeof(AVPacket))),
                                    [&](AVPacket *p) {
                                        av_packet_free(&p);
                                        av_freep(&p);
                                    });
                            av_init_packet(pTmpPkt.get());
                            pTmpPkt->data = NULL;
                            pTmpPkt->size = 0;
                            ret = avcodec_encode_video2(outPutEncContext, pTmpPkt.get(),
                                                        filterFrame, &got_output);
                            if (ret >= 0 && got_output) {
                                int ret = av_write_frame(opCtx, pTmpPkt.get());
                            }
                        }
                        av_frame_unref(filterFrame);
                    }
                }
            }
        } else {
            break;
        }
    }
    return 0;
}

#endif //SNAILFFMPEG_ADDLOGO_H
