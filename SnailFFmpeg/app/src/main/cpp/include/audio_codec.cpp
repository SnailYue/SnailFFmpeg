//
// Created by surface on 2020/7/17.
//

#include "audio_codec.h"

uint8_t *outputBuffer;
size_t outputBufferSize;

AVPacket packet;
int audioStream;
AVFrame *aFrame;
SwrContext *swr;
AVFormatContext *aFormatContext;
AVCodecContext *aCodecContext;

/**
 * 初始化相关
 * @param rate
 * @param channel
 * @param url
 * @return
 */
int initFFmpeg(int *rate, int *channel, char *url) {

    av_register_all();
    aFormatContext = avformat_alloc_context();
    av_log(NULL, AV_LOG_INFO, "url = %s", url);
    char *file_name = url;

    /**
     * 打开音频文件
     */
    if (avformat_open_input(&aFormatContext, file_name, NULL, NULL) != 0) {
        av_log(NULL, AV_LOG_ERROR, "Could not open file");
        return -1;
    }
    /**
     * 检索流信息
     */
    if (avformat_find_stream_info(aFormatContext, NULL) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Could not find stream");
        return -1;
    }

    /**
     * 找到第一条音频流
     */
    audioStream = -1;
    for (int i = 0; i < aFormatContext->nb_streams; ++i) {
        if (aFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO &&
            audioStream < 0) {
            audioStream = i;
        }
    }

    if (audioStream == -1) {
        av_log(NULL, AV_LOG_ERROR, "Could not find audio");
        return -1;
    }
    /**
     * 获取流中的解码器上下文的指针
     */
    aCodecContext = aFormatContext->streams[audioStream]->codec;
    /**
     * 查找流中的解码器上下文中的解码器
     */
    AVCodec *aCodec = avcodec_find_decoder(aCodecContext->codec_id);
    if (!aCodec) {
        av_log(NULL, AV_LOG_ERROR, "Unsupported codec");
        return -1;
    }
    /**
     * 打开解码器
     */
    if (avcodec_open2(aCodecContext, aCodec, NULL) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Could not open codec");
        return -1;
    }

    aFrame = av_frame_alloc();
    swr = swr_alloc();
    /**
     * 重采样
     */
    av_opt_set_int(swr, "int_channel_layout", aCodecContext->channel_layout, 0);
    av_opt_set_int(swr, "out_channel_layout", aCodecContext->channel_layout, 0);
    av_opt_set_int(swr, "in_sample_rate", aCodecContext->sample_rate, 0);
    av_opt_set_int(swr, "out_sample_rate", aCodecContext->sample_rate, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", aCodecContext->sample_fmt, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    swr_init(swr);

    /**
     * 分配PCM的数据缓存
     */
    outputBufferSize = 8196;
    outputBuffer = (uint8_t *) malloc(sizeof(uint8_t) * outputBufferSize);

    /**
     * 返回采样率和信道
     */
    *rate = aCodecContext->sample_rate;
    *channel = aCodecContext->channels;
    return 0;
}

/**
 * 获取PCM数据，自动回调获取
 * @param pcm
 * @param pcmsize
 * @return
 */
int getPCM(void **pcm, size_t *pcmsize) {
    av_log(NULL, AV_LOG_INFO, "getPCM");
    /**
     * 读取音频流
     */
    while (av_read_frame(aFormatContext, &packet) >= 0) {
        int frameFinished = 0;
        /**
         * 判断packet中的信息是否来自音频流
         */
        if (packet.stream_index == audioStream) {
            avcodec_decode_audio4(aCodecContext, aFrame, &frameFinished, &packet);
            if (frameFinished) {
                /**
                 * 获取音频数据所占的字节数
                 */
                int data_size = av_samples_get_buffer_size(aFrame->linesize,
                                                           aCodecContext->channels,
                                                           aFrame->nb_samples,
                                                           aCodecContext->sample_fmt, 1);
                av_log(NULL, AV_LOG_INFO, "data_size = %d", data_size);
                /**
                 * 再分配内存
                 */
                if (data_size > outputBufferSize) {
                    outputBufferSize = data_size;
                    outputBuffer = (uint8_t *) realloc(outputBuffer,
                                                       sizeof(uint8_t) * outputBufferSize);
                }
                /**
                 * 音频格式转换
                 */
                swr_convert(swr, &outputBuffer, aFrame->nb_samples,
                            (uint8_t const **) (aFrame->extended_data), aFrame->nb_samples);
                /**
                 * 返回PCM数据
                 */
                *pcm = outputBuffer;
                *pcmsize = data_size;
                return 0;
            }
        }
    }
}


/**
 * 释放相关的资源
 * @return
 */
int releaseFFmpeg() {
    av_packet_unref(&packet);
    av_free(outputBuffer);
    av_free(aFrame);
    avcodec_close(aCodecContext);
    avformat_close_input(&aFormatContext);
    return 0;
}