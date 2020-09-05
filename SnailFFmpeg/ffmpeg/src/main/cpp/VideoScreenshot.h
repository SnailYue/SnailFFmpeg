//
// Created by surface on 2020/9/5.
//

#ifndef SNAILFFMPEG_VIDEOSCREENSHOT_H
#define SNAILFFMPEG_VIDEOSCREENSHOT_H

#include <string>
#include <iostream>

extern "C" {

#include <libavutil/time.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>

}

using namespace std;


AVFormatContext *inputContext = nullptr;
AVFormatContext *outputContext;

int64_t lastReadPacketTime;

static int interrupt_cb(void *ctx) {
    int timeout = 3;
    if (av_gettime() - lastReadPacketTime > timeout * 1000 * 1000) {
        return -1;
    }
    return 0;
}


/**
 * 打开输入
 * @param inputUrl
 * @return
 */
int openInput(string inputUrl) {
    inputContext = avformat_alloc_context();
    lastReadPacketTime = av_gettime();
    inputContext->interrupt_callback.callback = interrupt_cb;
    int ret = avformat_open_input(&inputContext, inputUrl.c_str(), nullptr, nullptr);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Input file open input failed");
        return ret;
    }
    ret = avformat_find_stream_info(inputContext, nullptr);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Find input file stream info failed");
    } else {
        av_log(NULL, AV_LOG_INFO, "Find input file stream success");
    }
    return ret;
}

shared_ptr<AVPacket> readPacketFromSource() {
    shared_ptr<AVPacket> packet(static_cast<AVPacket *>(av_malloc(sizeof(AVPacket))),
                                [&](AVPacket *p) {
                                    av_packet_free(&p);
                                    av_freep(&p);
                                });
    av_init_packet(packet.get());
    lastReadPacketTime = av_gettime();
    int ret = av_read_frame(inputContext, packet.get());
    if (ret >= 0) {
        return packet;
    } else {
        return nullptr;
    }
}


/**
 * 打开输出
 * @param outUrl
 * @return
 */
int openOutput(string outUrl) {
    int ret = avformat_alloc_output_context2(&outputContext, nullptr, "singlejpeg", outUrl.c_str());
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "open output context failed");
        goto Error;
    }

    ret = avio_open2(&outputContext->pb, outUrl.c_str(), AVIO_FLAG_WRITE, nullptr, nullptr);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "open avio failed");
        goto Error;
    }
    for (int i = 0; i < inputContext->nb_streams; ++i) {
        if (inputContext->streams[i]->codec->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO) {
            continue;
        }
        AVStream *stream = avformat_new_stream(outputContext,
                                               inputContext->streams[i]->codec->codec);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "copy codec context failed");
            goto Error;
        }
    }
    ret = avformat_write_header(outputContext, nullptr);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "format write header failed");
        goto Error;
    }
    av_log(NULL, AV_LOG_FATAL, "Open output file success %s\n", outUrl.c_str());
    return ret;
    Error:
    if (outputContext) {
        for (int i = 0; i < outputContext->nb_streams; ++i) {
            avcodec_close(outputContext->streams[i]->codec);
        }
        avformat_close_input(&outputContext);
    }
    return ret;
}


/**
 * 初始化
 */
void init() {
    av_register_all();
    avfilter_register_all();
    avformat_network_init();
    av_log_set_level(AV_LOG_WARNING);
}

/**
 * 关闭输入
 */
void closeInput() {
    if (inputContext != nullptr) {
        avformat_close_input(&inputContext);
    }
}

/**
 * 关闭输出
 */
void closeOutput() {
    if (outputContext != nullptr) {
        av_write_trailer(outputContext);
        for (int i = 0; i < outputContext->nb_streams; ++i) {
            AVCodecContext *codecContext = outputContext->streams[i]->codec;
            avcodec_close(codecContext);
        }
        avformat_close_input(&outputContext);
    }
}

/**
 * 写帧数据
 * @param packet
 * @return
 */
int writePacket(shared_ptr<AVPacket> packet) {
    auto inputStream = inputContext->streams[packet->stream_index];
    auto outputStream = outputContext->streams[packet->stream_index];
    return av_interleaved_write_frame(outputContext, packet.get());
}

/**
 * 初始化DecodeContext
 * @param inputStream
 * @return
 */
int initDecodeContext(AVStream *inputStream) {
    auto codecId = inputStream->codec->codec_id;
    auto codec = avcodec_find_decoder(codecId);
    if (!codec) {
        return -1;
    }
    int ret = avcodec_open2(inputStream->codec, codec, NULL);
    return ret;
}

/**
 * 初始化EncoderCodec
 * @param inputStream
 * @param encodeContext
 * @return
 */
int initEncoderCodec(AVStream *inputStream, AVCodecContext **encodeContext) {
    AVCodec *picCodec;
    picCodec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    (*encodeContext) = avcodec_alloc_context3(picCodec);

    (*encodeContext)->codec_id = picCodec->id;
    (*encodeContext)->time_base.num = inputStream->codec->time_base.num;
    (*encodeContext)->time_base.den = inputStream->codec->time_base.den;
    (*encodeContext)->pix_fmt = *picCodec->pix_fmts;
    (*encodeContext)->height = inputStream->codec->height;
    (*encodeContext)->width = inputStream->codec->width;

    int ret = avcodec_open2((*encodeContext), picCodec, nullptr);
    if (ret < 0) {
        cout << "open video codec failed" << endl;
        return ret;
    }
    return 1;
}

/**
 * 解码
 * @param inputStream
 * @param packet
 * @param frame
 * @return
 */
bool decode(AVStream *inputStream, AVPacket *packet, AVFrame *frame) {
    int gotFrame = 0;
    auto hr = avcodec_decode_video2(inputStream->codec, frame, &gotFrame, packet);
    if (hr >= 0 && gotFrame != 0) {
        return true;
    }
    return false;
}

/**
 * 编码
 * @param encodeContext
 * @param frame
 * @return
 */
shared_ptr<AVPacket> encode(AVCodecContext *encodeContext, AVFrame *frame) {
    int gotOutput = 0;
    shared_ptr<AVPacket> pkt(static_cast<AVPacket *>(av_malloc(sizeof(AVPacket))),
                             [&](AVPacket *p) {
                                 av_packet_free(&p);
                                 av_freep(&p);
                             });
    av_init_packet(pkt.get());
    pkt->data = NULL;
    pkt->size = 0;
    int ret = avcodec_encode_video2(encodeContext, pkt.get(), frame, &gotOutput);
    if (ret >= 0 && gotOutput) {
        return pkt;
    } else {
        return nullptr;
    }
}

/**
 * 调用的截屏方法
 * @param url
 * @param output_name
 * @return
 */
int screenShot(string url, string output_name) {
    init();
    int ret = openInput(url);
    if (ret >= 0) {
        ret = openOutput(output_name);
    }

    if (ret < 0) {
        closeInput();
        closeOutput();
    }

    AVCodecContext *encodeContext = nullptr;
    initDecodeContext(inputContext->streams[0]);
    AVFrame *videoFrame = av_frame_alloc();
    initEncoderCodec(inputContext->streams[0], &encodeContext);

    while (true) {
        auto packet = readPacketFromSource();
        if (packet && packet->stream_index == 0) {
            if (decode(inputContext->streams[0], packet.get(), videoFrame)) {
                auto packetEncode = encode(encodeContext, videoFrame);
                if (packetEncode) {
                    ret = writePacket(packetEncode);
                    if (ret >= 0) {
                        break;
                    }
                }
            }
        }
    }
    cout << "Get Picture End" << endl;
    av_frame_free(&videoFrame);
    avcodec_close(encodeContext);
    return 0;
}

#endif //SNAILFFMPEG_VIDEOSCREENSHOT_H
