//
// Created by surface on 2020/9/14.
//

#include "MediaEncoder.h"

void MediaEncoder::custom_log(void *ptr, int livel, const char *fmt, va_list vl) {
    LOGI("custom_log");
    FILE *file = fopen("/storage/emulated/0/av_log.txt", "a+");
    if (file) {
        vfprintf(file, fmt, vl);
        fflush(file);
        fclose(file);
    }
}

/**
 * init
 *
 * @param width
 * @param height
 * @return
 */
int MediaEncoder::init(int width, int height) {
    LOGI("init");
    const char *outPath = "/storage/emulated/0/ffmpegcamera.flv";
    yuvWidth = width;
    yuvHeight = height;
    yLength = width * height;
    uvLength = yLength / 4;
    av_log_set_callback(custom_log);

    av_register_all();
    avformat_alloc_output_context2(&ofmtContext, NULL, "flv", outPath);
    avCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!avCodec) {
        LOGE("Can not find encoder");
        return -1;
    }
    codecContext = avcodec_alloc_context3(avCodec);
    if (!codecContext) {
        LOGE("Could bot allocate video codec context");
        return -1;
    }
    codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    codecContext->width = width;
    codecContext->height = height;
    codecContext->time_base.num = 1;
    codecContext->time_base.den = 30;
    codecContext->bit_rate = 800000;
    codecContext->gop_size = 300;
    if (ofmtContext->oformat->flags & AVFMT_GLOBALHEADER) {
        codecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }
    codecContext->qmin = 10;
    codecContext->qmax = 51;
    codecContext->max_b_frames = 3;

    AVDictionary *param = 0;
    av_dict_set(&param, "preset", "ultrafasr", 0);
    av_dict_set(&param, "tune", "zerolatency", 0);

    if (avcodec_open2(codecContext, avCodec, &param) < 0) {
        LOGE("Failed to open encoder!");
        return -1;
    }

    videoStream = avformat_new_stream(ofmtContext, avCodec);
    if (videoStream == NULL) {
        return -1;
    }
    videoStream->time_base.num = 1;
    videoStream->time_base.den = 30;
    videoStream->codec = codecContext;
    if (avio_open(&ofmtContext->pb, outPath, AVIO_FLAG_READ_WRITE) < 0) {
        LOGE("Failed to open output file");
        return -1;
    }
    avformat_write_header(ofmtContext, NULL);
    startTime = av_gettime();
    return 0;
}

/**
 * encode
 *
 * @param data
 * @return
 */
int MediaEncoder::encode(int8_t *data) {
    LOGI("encode");
    int ret;
    int encodeGotFrame = 0;
    avFrame = av_frame_alloc();

    uint8_t *outBuffer = (uint8_t *) av_malloc(
            avpicture_get_size(AV_PIX_FMT_YUV420P, codecContext->width, codecContext->height));
    avpicture_fill((AVPicture *) avFrame, outBuffer, AV_PIX_FMT_YUV420P, codecContext->width,
                   codecContext->height);
    memcpy(avFrame->data[0], data, yLength);
    for (int i = 0; i < uvLength; ++i) {
        *(avFrame->data[2] + i) = *(data + yLength + i * 2);
        *(avFrame->data[1] + i) = *(data + yLength + i * 2 + 1);
    }
    avFrame->format = AV_PIX_FMT_YUV420P;
    avFrame->width = yuvWidth;
    avFrame->height = yuvHeight;

    avPacket.data = NULL;
    avPacket.size = 0;

    av_init_packet(&avPacket);
    ret = avcodec_encode_video2(codecContext, &avPacket, avFrame, &encodeGotFrame);
    av_frame_free(&avFrame);

    if (encodeGotFrame == 1) {
        LOGI("SUcceed to encode frame");
        frameCount++;
        avPacket.stream_index = videoStream->index;
        AVRational timeBase = ofmtContext->streams[0]->time_base;
        AVRational frameRate = {60, 2};
        AVRational timeBaseQ = {1, AV_TIME_BASE};
        int64_t calcDuration = (double) (AV_TIME_BASE) * (1 / av_q2d(frameRate));

        avPacket.pts = av_rescale_q(frameCount * calcDuration, timeBaseQ, timeBase);
        avPacket.dts = avPacket.pts;
        avPacket.duration = av_rescale_q(calcDuration, timeBaseQ, timeBase);
        avPacket.pos = 1;

        int64_t ptsTime = av_rescale_q(avPacket.dts, timeBase, timeBaseQ);
        int64_t nowTime = av_gettime() - startTime;
        if (ptsTime > nowTime) {
            av_usleep(ptsTime - nowTime);
        }
        ret = av_interleaved_write_frame(ofmtContext, &avPacket);
        av_free_packet(&avPacket);
    }
    return ret;
}

/**
 * flush
 *
 * @return
 */
int MediaEncoder::flush() {
    LOGI("flush");
    int ret;
    int gotFrame;
    AVPacket encPacket;
    if (!(ofmtContext->streams[0]->codec->codec->capabilities & CODEC_CAP_DELAY)) {
        return 0;
    }
    while (1) {
        encPacket.data = NULL;
        encPacket.size = 0;
        av_init_packet(&encPacket);
        //open encode;
        ret = avcodec_encode_video2(ofmtContext->streams[0]->codec, &encPacket, NULL, &gotFrame);
        if (ret < 0) {
            break;
        }
        if (!gotFrame) {
            ret = 0;
            break;
        }
        LOGI("Succeed to encode 1 frame");
        AVRational timeBase = ofmtContext->streams[0]->time_base;
        AVRational frameRate = {60, 2};
        AVRational timeBaseQ = {1, AV_TIME_BASE};
        int64_t calcDuration = (double) (AV_TIME_BASE) * (1 / av_q2d(frameRate));

        //set pts,dts,duration&pos;
        encPacket.pts = av_rescale_q(frameCount * calcDuration, timeBaseQ, timeBase);
        encPacket.dts = encPacket.pts;
        encPacket.duration = av_rescale_q(calcDuration, timeBaseQ, timeBase);
        encPacket.pos = -1;

        frameCount++;
        ofmtContext->duration = encPacket.duration * frameCount;
        //write frame
        ret = av_interleaved_write_frame(ofmtContext, &encPacket);
        if (ret < 0) {
            break;
        }
    }
    //write trailer
    av_write_trailer(ofmtContext);
    return 0;
}

/**
 * 释放
 *
 * @return
 */
int MediaEncoder::close() {
    LOGI("close");
    if (videoStream) {
        avcodec_close(videoStream->codec);
    }
    avio_close(ofmtContext->pb);
    avformat_free_context(ofmtContext);
    return 0;
}