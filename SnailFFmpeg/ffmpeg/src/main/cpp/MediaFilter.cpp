//
// Created by surface on 9/17/2020.
//


#include "MediaFilter.h"

/**
 *
 * 1.afilter_graph_alloc  为AVFilterGraph分配内存
 * 2.avfilter_graph_create_filter(src) 创建并想AVFilterGraph中添加一个AVfilterContext
 * 3.avfilter_graph_create_filter(sink)
 * 4.avfilter_graph_parse_ptr 将一串通过字符串描述的Graph团价到AVFilterGraph中
 * 5.avfilter_graph_config 检查AVFilterGraph配置有效性，并配置其中所有连接和格式
 * 6.avbuffersrc_add_frame 添加一个AVFrame到source filter中
 * 7.avbuffersink_get_frame 从sink filter中获取一个AVFrame
 *
 * 初始化
 * @param env
 * @param url
 * @param surface
 * @return
 */
int MediaFilter::init(JNIEnv *env, string url, jobject surface) {
    LOGI("MediaFilter::init");
    av_register_all();
    avfilter_register_all();
    avFormatContext = avformat_alloc_context();

    //Open video file
    if (avformat_open_input(&avFormatContext, url.c_str(), NULL, NULL) != 0) {
        LOGE("Could not open file");
        return -1;
    }
    //find stream information
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("Could not find stream information");
        return -1;
    }
    videoStream = -1;
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO &&
            videoStream < 0) {
            videoStream = i;
        }
    }
    if (videoStream == -1) {
        LOGE("Could not find video stream info");
        return -1;
    }
    //find avcodecContext
    avCodecContext = avFormatContext->streams[videoStream]->codec;
    //init Avfilter
    bufferSrc = avfilter_get_by_name("buffer");
    bufferSink = avfilter_get_by_name("buffersink");
    //1.init AvfilterInOut
    outputs = avfilter_inout_alloc();
    inputs = avfilter_inout_alloc();
    filterGraph = avfilter_graph_alloc();

    snprintf(args, sizeof(args), "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             avCodecContext->width, avCodecContext->height, avCodecContext->pix_fmt,
             avCodecContext->time_base.num, avCodecContext->time_base.den,
             avCodecContext->sample_aspect_ratio.num, avCodecContext->sample_aspect_ratio.den);
    //2.avfilter_graph_create_filter()
    int ret = avfilter_graph_create_filter(&bufferSrcContext, bufferSrc, "in", args, NULL,
                                           filterGraph);
    if (ret < 0) {
        LOGE("Could not create buffer source");
        return ret;
    }

    bufferSinkParams = av_buffersink_params_alloc();
    bufferSinkParams->pixel_fmts = pixFormats;
    ret = avfilter_graph_create_filter(&bufferSinkContext, bufferSink, "out", NULL,
                                       bufferSinkParams, filterGraph);
    av_free(bufferSinkParams);
    if (ret < 0) {
        LOGE("Could not create buffer sink");
        return ret;
    }
    outputs->name = av_strdup("in");
    outputs->filter_ctx = bufferSrcContext;
    outputs->pad_idx = 0;
    outputs->next = NULL;

    inputs->name = av_strdup("out");
    inputs->filter_ctx = bufferSinkContext;
    inputs->pad_idx = 0;
    inputs->next = NULL;
    //3.avfilter_graph_parse_ptr
    if ((ret = avfilter_graph_parse_ptr(filterGraph, filtersDescr, &inputs, &outputs, NULL)) < 0) {
        LOGE("Could not avfilter_graph_config");
        return ret;
    }
    //4.avfilter_graph_config
    if ((ret = avfilter_graph_config(filterGraph, NULL)) < 0) {
        LOGE("Could not avfilter_graph_config");
        return ret;
    }
    avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    if (avCodec == NULL) {
        LOGE("Codec not found");
        return -1;
    }
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        LOGE("Could not open codec");
        return -1;
    }

    nativeWindow = ANativeWindow_fromSurface(env, surface);
    videoWidth = avCodecContext->width;
    videoHeight = avCodecContext->height;

    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    avFrame = av_frame_alloc();
    avFrameRGBA = av_frame_alloc();

    if (avFrame == NULL || avFrameRGBA == NULL) {
        LOGE("Could not allocate video frame");
        return -1;
    }

    numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, avCodecContext->width,
                                        avCodecContext->height, 1);
    buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(avFrameRGBA->data, avFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                         avCodecContext->width, avCodecContext->height, 1);

    swsContext = sws_getContext(avCodecContext->width, avCodecContext->height,
                                avCodecContext->pix_fmt, avCodecContext->width,
                                avCodecContext->height, AV_PIX_FMT_RGBA,
                                SWS_BILINEAR, NULL, NULL,
                                NULL);
    return 0;
}

/**
 * 播放
 * @return
 */
int MediaFilter::play() {
    LOGI("MediaFilter::play");
    while (av_read_frame(avFormatContext, &packet) >= 0) {
        LOGI("MediaFilter::play->read_frame");
        if (packet.stream_index == videoStream) {
            avcodec_decode_video2(avCodecContext, avFrame, &frameFinished, &packet);
            if (frameFinished) {
                avFrame->pts = av_frame_get_best_effort_timestamp(avFrame);
                //5.av_buffersrc_add_frame
                if (av_buffersrc_add_frame(bufferSrcContext, avFrame)) {
                    LOGE("Could not av_buffersrc_add_frame");
                    break;
                }
                //6.av_buffersink_get_frame
                if ((av_buffersink_get_frame(bufferSinkContext, avFrame)) < 0) {
                    LOGE("Could not av_buffersink_get_frame");
                    break;
                }
                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);
                sws_scale(swsContext, (uint8_t const *const *) avFrame->data, avFrame->linesize, 0,
                          avCodecContext->height, avFrameRGBA->data, avFrameRGBA->linesize);
                uint8_t *dst = (uint8_t *) windowBuffer.bits;
                int dstStride = windowBuffer.stride * 4;
                uint8_t *src = (avFrameRGBA->data[0]);
                int srcStride = avFrameRGBA->linesize[0];
                for (int i = 0; i < videoHeight; ++i) {
                    memcpy(dst + i * dstStride, src + i * srcStride, srcStride);
                }
                ANativeWindow_unlockAndPost(nativeWindow);
            }
        }
        av_packet_unref(&packet);
    }
    release();
    return 0;
}

/**
 * 释放相关
 * @return
 */
int MediaFilter::release() {
    LOGI("MediaFilter::release");
    av_free(buffer);
    av_free(avFrameRGBA);
    av_free(avFrame);
    avfilter_graph_free(&filterGraph);
    avcodec_close(avCodecContext);
    avformat_close_input(&avFormatContext);
    return 0;
}