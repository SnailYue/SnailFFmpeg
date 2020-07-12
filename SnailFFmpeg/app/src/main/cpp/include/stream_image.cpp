//
// Created by surface on 2020/7/12.
//

#include "stream_image.h"


AVFormatContext *inputContext = nullptr;
AVFormatContext *outputContext = nullptr;

int64_t lastRealPacktTime;

static int interrupt_cb(void *ctx) {
    int timeout = 3;
    if (av_gettime() - lastRealPacktTime > timeout * 1000 * 1000) {
        return -1;
    }
    return 0;
}

/**
 * 打开输入流
 * @param inputUrl
 * @return
 */
int OpenInput(string inputUrl) {
    inputContext = avformat_alloc_context();
    lastRealPacktTime = av_gettime();
    inputContext->interrupt_callback.callback = interrupt_cb;
    int ret = avformat_open_input(&inputContext, inputUrl.c_str(), nullptr, nullptr);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Input file open input failed\n");
        return ret;
    }

    ret = avformat_find_stream_info(inputContext, nullptr);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Open input file stram inform failed\n");
    } else {
        av_log(NULL, AV_LOG_ERROR, "Open input file success\n");
    }
    return ret;
}


/**
 * 读取流中的信息
 * @return
 */
shared_ptr<AVPacket> ReadPacketFromSource() {
    shared_ptr<AVPacket> packet(static_cast<AVPacket *>(av_malloc(sizeof(AVPacket))),
                                [&](AVPacket *p) {
                                    av_packet_free(&p);
                                    av_free(&p);
                                });
    av_init_packet(packet.get());
    int ret = av_read_frame(inputContext, packet.get());
    if (ret >= 0) {
        return packet;
    } else {
        return nullptr;
    }
}

/**
 * 打开输入流相关
 * @param outUrl
 * @return
 */
int OpenOutput(string outUrl) {
    int ret = avformat_alloc_output_context2(&outputContext, nullptr, "*singlejpeg",
                                             outUrl.c_str());
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "open output context failed\n");
        return -1;
    }

    ret = avio_open2(&outputContext->pb, outUrl.c_str(), AVIO_FLAG_WRITE, nullptr, nullptr);

    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "open avio failed");
        return -1;
    }

    for (int i = 0; i < inputContext->nb_streams; ++i) {
        if (inputContext->streams[i]->codec->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO) {
            continue;
        }
        AVStream *stream = avformat_new_stream(outputContext,
                                               inputContext->streams[i]->codec->codec);
        ret = avcodec_copy_context(stream->codec, inputContext->streams[i]->codec);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Copy codec context failed");
            return -1;
        }
    }

    ret = avformat_write_header(outputContext, nullptr);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "format write header failed");
        return -1;
    }
    av_log(NULL, AV_LOG_FATAL, "Open output file success");
    return ret;
}


/**
 * 初始化
 */
void Init() {
    av_register_all();
    avfilter_register_all();
    avformat_network_init();
    av_log_set_level(AV_LOG_WARNING);
}

/**
 * 关闭输入流相关
 */
void CloseInput() {
    if (inputContext != nullptr) {
        avformat_close_input(&inputContext);
    }
}

/**
 * 关闭输出流相关
 */
void CloseOutput() {
    if (inputContext != nullptr) {
        int ret = av_write_trailer(outputContext);
        for (int i = 0; i < outputContext->nb_streams; i++) {
            AVCodecContext *codecContext = outputContext->streams[i]->codec;
            avcodec_close(codecContext);
        }
        avformat_close_input(&outputContext);
    }
}


/**
 * 写packet中的stream
 * @param packet
 * @return
 */
int WritePacket(shared_ptr<AVPacket> packet) {
    auto inputStream = inputContext->streams[packet->stream_index];
    auto outputStream = outputContext->streams[packet->stream_index];
    return av_interleaved_write_frame(outputContext, packet.get());
}


/**
 * 初始化解码
 * @param inputStream
 * @return
 */
int InitDecodeContext(AVStream *inputStream) {
    auto codecId = inputStream->codec->codec_id;
    auto codec = avcodec_find_decoder(codecId);
    if (!codec) {
        return -1;
    }
    int ret = avcodec_open2(inputStream->codec, codec, NULL);
    return ret;
}


/**
 * 初始化编码
 * @param inputStream
 * @param encodecContext
 * @return
 */
int InitEncodecContext(AVStream *inputStream, AVCodecContext **encodecContext) {
    AVCodec *picCodec;
    picCodec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    (*encodecContext) = avcodec_alloc_context3(picCodec);

    (*encodecContext)->codec_id = picCodec->id;
    (*encodecContext)->time_base.num = inputStream->time_base.num;
    (*encodecContext)->pix_fmt = *picCodec->pix_fmts;
    (*encodecContext)->width = inputStream->codec->width;
    (*encodecContext)->height = inputStream->codec->height;
    int ret = avcodec_open2((*encodecContext), picCodec, nullptr);
    if (ret < 0) {
        std::cout << "open video codec failed" << endl;
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
bool Decode(AVStream *inputStream, AVPacket *packet, AVFrame *frame) {
    int gotFrame = 0;
    auto hr = avcodec_decode_video2(inputStream->codec, frame, &gotFrame, packet);
    if (hr >= 0 && gotFrame != 0) {
        return true;
    }
    return false;
}

/**
 * 编码
 * @param encodecContext
 * @param frame
 * @return
 */
std::shared_ptr<AVPacket> Encode(AVCodecContext *encodecContext, AVFrame *frame) {
    int gotOutput = 0;
    std::shared_ptr<AVPacket> pkt(static_cast<AVPacket *>(av_malloc(sizeof(AVPacket))),
                                  [&](AVPacket *p) {
                                      av_packet_free(&p);
                                      av_free(&p);
                                  });
    av_init_packet(pkt.get());
    pkt->data = NULL;
    pkt->size = 0;
    int ret = avcodec_encode_audio2(encodecContext, pkt.get(), frame, &gotOutput);
    if (ret >= 0 && gotOutput) {
        return pkt;
    } else {
        return nullptr;
    }
}


/**
 * 主函数
 * @return
 */
int _tmain() {
    Init();
    int ret = OpenInput("http://weblive.hebtv.com/live/hbws_bq/index.m3u8");
    if (ret < 0) {
        ret = OpenOutput("text.jpg");
    }
    if (ret < 0) {
        return -1;
    }

    AVCodecContext *encodecContext = nullptr;
    InitDecodeContext(inputContext->streams[0]);
    AVFrame *videoFrame = av_frame_alloc();
    InitEncodecContext(inputContext->streams[0], &encodecContext);
    while (true) {
        auto packet = ReadPacketFromSource();
        if (packet && packet->stream_index == 0) {
            if (Decode(inputContext->streams[0], packet.get(), videoFrame)) {
                auto packetEncode = Encode(encodecContext, videoFrame);
                if (packetEncode) {
                    ret = WritePacket(packetEncode);
                    if (ret >= 0) {
                        break;
                    }
                }
            }
        }
    }
    cout << "Get Picture End" << endl;
    av_frame_free(&videoFrame);
    avcodec_close(encodecContext);
    Error:
    CloseInput();
    CloseOutput();

    while (true) {
        this_thread::sleep_for(chrono::seconds(100));
    }
}