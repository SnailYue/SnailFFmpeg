//
// Created by surface on 2020/9/16.
//

#include "MediaPlayer.h"

int MediaPlayer::init(JNIEnv *env, string filePath, jobject surface) {
    LOGI("MediaPlayer::init()");
    av_register_all();
    pFormatContext = avformat_alloc_context();
    if (avformat_open_input(&pFormatContext, filePath.c_str(), NULL, NULL) != 0) {
        LOGE("Could not open file");
        return -1;
    }

    if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
        LOGE("could not find stream information");
        return -1;
    }

    videoStream = -1;
    for (int i = 0; i < pFormatContext->nb_streams; ++i) {
        if (pFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO &&
            videoStream == -1) {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1) {
        LOGE("Could not find video stream");
        return -1;
    }
    pCodecContext = pFormatContext->streams[videoStream]->codec;
    pCodec = avcodec_find_decoder(pCodecContext->codec_id);
    if (pCodec == NULL) {
        LOGE("Codec not found");
        return -1;
    }
    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0) {
        LOGE("Could not open codec");
        return -1;
    }
    //1.ANativeWindow_fromSurface()   获取native层的window
    nativeWindow = ANativeWindow_fromSurface(env, surface);

    videoWidth = pCodecContext->width;
    videoHeight = pCodecContext->height;
    //2.ANativeWindow_setBufferGeometry()  设置native层 window的buffer的大小
    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    pFrame = av_frame_alloc();
    pFrameRGBA = av_frame_alloc();
    if (pFrameRGBA == NULL || pFrame == NULL) {
        LOGE("Could not allocate video frame");
        return -1;
    }

    numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecContext->width,
                                        pCodecContext->height, 1);
    buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                         pCodecContext->width, pCodecContext->height, 1);
    swsContext = sws_getContext(pCodecContext->width, pCodecContext->height, pCodecContext->pix_fmt,
                                pCodecContext->width, pCodecContext->height, AV_PIX_FMT_RGBA,
                                SWS_BILINEAR, NULL, NULL, NULL);
    return 0;
}

int MediaPlayer::play() {
    LOGI("MediaPlayer::play()");
    while (av_read_frame(pFormatContext, &packet) >= 0) {
        LOGI("read_frame...");
        if (packet.stream_index == videoStream) {
            avcodec_decode_video2(pCodecContext, pFrame, &frameFinished, &packet);
            if (frameFinished) {
                //3.ANativeWindow_lock()  给native window加锁
                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);
                sws_scale(swsContext, (uint8_t const *const *) pFrame->data, pFrame->linesize, 0,
                          pCodecContext->height, pFrameRGBA->data, pFrameRGBA->linesize);
                uint8_t *dst = (uint8_t *) windowBuffer.bits;
                int dstStride = windowBuffer.stride * 4;
                uint8_t *src = (pFrameRGBA->data[0]);
                int srcStride = pFrameRGBA->linesize[0];
                for (int i = 0; i < videoHeight; ++i) {
                    memcpy(dst + i * dstStride, src + i * srcStride, srcStride);
                }
                //4.ANativeWindow_unlockAndPost()  给native window解锁，并将新的缓冲区发送给显示
                ANativeWindow_unlockAndPost(nativeWindow);
            }
        }
        av_packet_unref(&packet);
    }
    release();
    return 0;
}

int MediaPlayer::release() {
    LOGI("MediaPlayer::release()");
    av_free(buffer);
    av_free(pFrameRGBA);
    av_free(pFrame);
    avcodec_close(pCodecContext);
    avformat_close_input(&pFormatContext);
    return 0;
}