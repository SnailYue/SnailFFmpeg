//
// Created by surface on 2020/9/8.
//


/**
 * 1.swr_alloc 创建重采样上下文SwrContext
 * 2.av_opt_set_xxx() 设置转换的参数
 * 3.swr_init() 初始化SwrContext
 * 4.分配样本数据内存空间 av_samples_alloc_array_and_samples、av_samples_alloc等工具函数
 * 5.swr_convert() 重采样转换
 * 6.swr_free() 释放重采样上下文SwrContext
 */

#include "AudioDecode.h"

/**
 * 初始化
 * @param rate
 * @param channel
 * @param url
 * @return
 */
int AudioDecode::init_ffmpeg(int *rate, int *channel, char *url) {
    //注册
    av_register_all();
    avFormatContext = avformat_alloc_context();
    LOGE("initFFmpeg");
    char *file_name = url;
    //打开文件
    if (avformat_open_input(&avFormatContext, file_name, NULL, NULL) != 0) {
        LOGE("open file failed");
        return -1;
    }
    //检索流信息
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("find stream info failed");
        return -1;
    }
    //找到第一条音频数据
    audioStream = -1;
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO &&
            audioStream < 0) {
            audioStream = i;
        }
    }
    if (audioStream == -1) {
        LOGE("find audio stream failed");
        return -1;
    }
    //根据获取到的索引，获取到编码器上下文
    avCodecContext = avFormatContext->streams[audioStream]->codec;
    //获取编码器上下文的解码器
    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    if (!avCodec) {
        LOGE("unsupported codec");
        return -1;
    }
    //打开解码器
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        LOGE("can not open aCodec");
        return -1;
    }

    //格式转换
    aFrame = av_frame_alloc();
    //1.申请一个重采样结构体
    swrContext = swr_alloc();

    //2.设置重采样的相关的参数
    av_opt_set_int(swrContext, "in_channel_layout", avCodecContext->channel_layout, 0);
    av_opt_set_int(swrContext, "out_channel_layout", avCodecContext->channel_layout, 0);
    av_opt_set_int(swrContext, "in_sample_rate", avCodecContext->sample_rate, 0);
    av_opt_set_int(swrContext, "out_sample_rate", avCodecContext->sample_rate, 0);
    av_opt_set_sample_fmt(swrContext, "in_sample_fmt", avCodecContext->sample_fmt, 0);
    av_opt_set_sample_fmt(swrContext, "out_sample_fmt", avCodecContext->sample_fmt, 0);
    //3.初始化重采样结构体
    swr_init(swrContext);

    outputBufferSize = 8196;
    outputBuffer = (uint8_t *) malloc(sizeof(uint8_t) * outputBufferSize);
    //返回采样率和信道
    *rate = avCodecContext->sample_rate;
    *channel = avCodecContext->channels;
    return 0;
}


/**
 * 获取PCM数据，自动回调获取
 * @param pcm
 * @param pcmsize
 * @return
 */
int AudioDecode::get_pcm(void **pcm, size_t *pcmsize) {
    LOGI("get_pcm");
    //读取帧数据
    while (av_read_frame(avFormatContext, &packet) >= 0) {
        int frameFinished = 0;
        //判断packet中的数据是否来自音频流
        if (packet.stream_index == audioStream) {
            avcodec_decode_audio4(avCodecContext, aFrame, &frameFinished, &packet);
            if (frameFinished) {
                //4.获取数据的大小
                int dataSize = av_samples_get_buffer_size(aFrame->linesize,
                                                          avCodecContext->channels,
                                                          aFrame->nb_samples,
                                                          avCodecContext->sample_fmt, 1);
                if (dataSize > outputBufferSize) {
                    outputBuffer = (uint8_t *) realloc(outputBuffer,
                                                       sizeof(uint8_t) * outputBufferSize);
                }
                //5.将输入的音频按照定义的参数进行转换
                swr_convert(swrContext, &outputBuffer, aFrame->nb_samples,
                            (uint8_t const **) (aFrame->extended_data), aFrame->nb_samples);
                //返回PCM数据
                *pcm = outputBuffer;
                *pcmsize = dataSize;
                return 0;
            }
        }
    }
    return -1;
}

/**
 * 释放相关的资源
 * @return
 */
int AudioDecode::release_ffmpeg() {
    LOGI("release_ffmpeg");
    av_packet_unref(&packet);
    av_free(outputBuffer);
    av_free(aFrame);
    //6.释放重采样结构体
    swr_free(&swrContext);
    avcodec_close(avCodecContext);
    avformat_close_input(&avFormatContext);
    return 0;
}

