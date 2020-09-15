//
// Created by surface on 2020/9/15.
//


#include "MediaDecoder.h"


void MediaDecoder::custom_log(void *ptr, int level, const char *fmt, va_list vl) {
    FILE *file = fopen("/storage/emulated/0/av_log.txt", "a+");
    if (file) {
        vfprintf(file, fmt, vl);
        fflush(file);
        fclose(file);
    }
}

int MediaDecoder::init(string input, string output) {
    av_log_set_callback(custom_log);
    av_register_all();
    avformat_network_init();

    pFormatContext = avformat_alloc_context();
    if (avformat_open_input(&pFormatContext, input.c_str(), NULL, NULL) != 0) {
        LOGE("Could not open input stream");
        return -1;
    }
    if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
        LOGE("Could not find stream information");
        return -1;
    }
    videoIndex = -1;
    for (int i = 0; i < pFormatContext->nb_streams; ++i) {
        if (pFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
            break;
        }
    }
    if (videoIndex == -1) {
        LOGE("Could not find a video stream");
        return -1;
    }
    pCodecContext = pFormatContext->streams[videoIndex]->codec;
    pCodec = avcodec_find_encoder(pCodecContext->codec_id);
    if (pCodec == NULL) {
        LOGE("Could not find Codec");
        return -1;
    }
    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0) {
        LOGE("Could not open codec");
        return -1;
    }
    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();
    outBuffer = (unsigned char *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecContext->width,
                                     pCodecContext->height, 1));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, outBuffer, AV_PIX_FMT_YUV420P,
                         pCodecContext->width, pCodecContext->height, 1);
    packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    imgConvertContext = sws_getContext(pCodecContext->width, pCodecContext->height,
                                       pCodecContext->pix_fmt, pCodecContext->width,
                                       pCodecContext->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL,
                                       NULL, NULL);
    fpYUV = fopen(output.c_str(), "wb+");
    if (fpYUV == NULL) {
        LOGE("Could not open output file");
        return -1;
    }
    frameCount = 0;
}

int MediaDecoder::decode() {
    int ret;
    while (av_read_frame(pFormatContext, packet) >= 0) {
        if (packet->stream_index == videoIndex) {
            ret = avcodec_decode_video2(pCodecContext, pFrame, &gotPicture, packet);
            if (ret < 0) {
                LOGE("Decode Error");
                return -1;
            }
            if (gotPicture) {
                sws_scale(imgConvertContext, (const uint8_t *const *) pFrame->data,
                          pFrame->linesize, 0, pCodecContext->height, pFrameYUV->data,
                          pFrameYUV->linesize);
                fwrite(pFrameYUV->data[0], 1, ySize, fpYUV);
                fwrite(pFrameYUV->data[1], 1, ySize / 4, fpYUV);
                fwrite(pFrameYUV->data[2], 1, ySize / 4, fpYUV);
                frameCount++;
            }
        }
        av_free_packet(packet);
    }
    while (1) {
        ret = avcodec_decode_video2(pCodecContext, pFrame, &gotPicture, packet);
        if (ret < 0) {
            break;
        }
        if (!gotPicture) {
            break;
        }
        sws_scale(imgConvertContext, (const uint8_t *const *) pFrame->data, pFrame->linesize, 0,
                  pCodecContext->height, pFrameYUV->data, pFrameYUV->linesize);
        fwrite(pFrameYUV->data[0], 1, pCodecContext->width * pCodecContext->height, fpYUV);
        fwrite(pFrameYUV->data[1], 1, pCodecContext->width * pCodecContext->height / 4, fpYUV);
        fwrite(pFrameYUV->data[2], 1, pCodecContext->width * pCodecContext->height / 4, fpYUV);
        frameCount++;
    }
}

int MediaDecoder::release() {
    sws_freeContext(imgConvertContext);
    fclose(fpYUV);
    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(pCodecContext);
    avformat_close_input(&pFormatContext);
    return 0;
}
