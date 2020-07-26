//
// Created by surface on 2020/7/19.
//

#include "video_codec.h"


int playVideo(string url) {
    const char *file_name = url.c_str();
    /**
     * 初始化注册
     */
    av_register_all();
    /**
     * 申请格式上下文
     */
    AVFormatContext *pFormatContext = avformat_alloc_context();
    /**
     * 打开视频流
     */
    if (avformat_open_input(&pFormatContext, file_name, nullptr, nullptr) != 0) {
        av_log(nullptr, AV_LOG_ERROR, "Could not find stream");
        return -1;
    }
    /**
     * 获取视频流的信息
     */
    if (avformat_find_stream_info(pFormatContext, nullptr) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "Could not find stream information");
        return -1;
    }
    int videoStream = -1;
    /**
     * 查找视频流
     */
    for (int i = 0; i < pFormatContext->nb_streams; ++i) {
        if (pFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
        }
    }
    if (videoStream == -1) {
        av_log(nullptr, AV_LOG_ERROR, "Could not find video stream");
        return -1;
    }
    /**
     * 获取视频流的解码器上下文
     */
    AVCodecContext *pCodecContext = pFormatContext->streams[videoStream]->codec;
    /**
     * 查找解码器
     */
    AVCodec *pCodec = avcodec_find_decoder(pCodecContext->codec_id);
    if (nullptr == pCodec) {
        av_log(nullptr, AV_LOG_ERROR, "Codec not found");
        return -1;
    }
    /**
     * 打开解码器
     */
    if (avcodec_open2(pCodecContext, pCodec, nullptr) < 0) {
        av_log(nullptr, AV_LOG_ERROR, "Could not open codec");
        return -1;
    }

    int videoWidth = pCodecContext->width;
    int videoHeight = pCodecContext->height;

    AVFrame *pFrame = av_frame_alloc();
    AVFrame *pFrameRGBA = av_frame_alloc();

    if (nullptr == pFrame || nullptr == pFrameRGBA) {
        av_log(nullptr, AV_LOG_ERROR, "Could not allocate video frame");
        return -1;
    }
    /**
     * 获取视频流缓存的大小
     */
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecContext->width,
                                            pCodecContext->height, 1);
    uint8_t *buffer = (uint8_t *) malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                         pCodecContext->width, pCodecContext->height, 1);
    /**
     * 格式转化
     */
    struct SwsContext *sws_context = sws_getContext(pCodecContext->width, pCodecContext->height,
                                                    pCodecContext->pix_fmt, pCodecContext->width,
                                                    pCodecContext->height, AV_PIX_FMT_RGBA,
                                                    SWS_BILINEAR,
                                                    nullptr, nullptr, nullptr);
    int frameFinished;
    AVPacket packet;
    /**
     * 读取视频流的帧信息
     */
    while (av_read_frame(pFormatContext, &packet) >= 0) {
        if (packet.stream_index == videoStream) {
            avcodec_decode_video2(pCodecContext, pFrame, &frameFinished, &packet);
            if (frameFinished) {
                sws_scale(sws_context, (uint8_t const *const *) pFrame->data, pFrame->linesize, 0,
                          pCodecContext->height, pFrameRGBA->data, pFrameRGBA->linesize);
            }
        }
        av_packet_unref(&packet);
    }

    /**
     * 释放相关的对象
     */
    av_free(buffer);
    av_free(pFrameRGBA);
    av_free(pFrame);
    avcodec_close(pCodecContext);
    avformat_close_input(&pFormatContext);

    return 0;
}