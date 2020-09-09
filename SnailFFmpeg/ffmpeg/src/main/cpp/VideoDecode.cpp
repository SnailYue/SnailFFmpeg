//
// Created by surface on 2020/9/9.
//

#include "VideoDecode.h"

/**
 * 初始化
 * @param url
 * @return
 */
int VideoDecode::init_ffmpeg(const char *url) {
    LOGI("init_ffmpeg");
    av_register_all();
    avFormatContext = avformat_alloc_context();

    if (avformat_open_input(&avFormatContext, url, NULL, NULL) != 0) {
        LOGE("open input file failed");
        return -1;
    }

    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("find stream info failed");
        return -1;
    }

    avFrame = av_frame_alloc();
    avFrameRGBA = av_frame_alloc();
    if (avFrame == NULL || avFrameRGBA == NULL) {
        LOGE("Could not allocate vide frame");
        return -1;
    }

    return 0;
}

/**
 * 查找解码器
 * @return
 */
int VideoDecode::find_decoder() {
    videoStream = -1;
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
        }
    }
    if (videoStream == -1) {
        LOGE("find video stream failed");
        return -1;
    }
    avCodecContext = avFormatContext->streams[videoStream]->codec;
    avCodec = avcodec_find_decoder(avCodecContext->codec_id);

    if (avCodec == NULL) {
        LOGE("Codec not found");
        return -1;
    }
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        LOGE("Could not open codec");
        return -1;
    }
}

/**
 * 释放资源
 * @return
 */
int VideoDecode::release() {
    av_free(buffer);
    av_free(avFrameRGBA);
    av_free(avFrame);
    avcodec_close(avCodecContext);
    avformat_close_input(&avFormatContext);
}

/**
 * 解码视频入口
 * @param url
 * @return
 */
int VideoDecode::play_video(const char *url) {
    LOGI("play_video");
    init_ffmpeg(url);
    find_decoder();

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, avCodecContext->width,
                                            avCodecContext->height, 1);
    buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));

    av_image_fill_arrays(avFrameRGBA->data, avFrameRGBA->linesize,
                         buffer, AV_PIX_FMT_RGBA, avCodecContext->width, avCodecContext->height,
                         1);

    struct SwsContext *swsContext = sws_getContext(avCodecContext->width, avCodecContext->height,
                                                   avCodecContext->pix_fmt, avCodecContext->width,
                                                   avCodecContext->height, AV_PIX_FMT_RGBA,
                                                   SWS_BILINEAR, NULL, NULL, NULL);
    int frameFinished;
    AVPacket packet;
    while (av_read_frame(avFormatContext, &packet) >= 0) {
        if (packet.stream_index == videoStream) {
            avcodec_decode_video2(avCodecContext, avFrame, &frameFinished, &packet);
            sws_scale(swsContext, (uint8_t const *const *) avFrame->data, avFrame->linesize, 0,
                      avCodecContext->height, avFrameRGBA->data, avFrameRGBA->linesize);
        }
        av_packet_unref(&packet);
    }
    release();
    return 0;
}