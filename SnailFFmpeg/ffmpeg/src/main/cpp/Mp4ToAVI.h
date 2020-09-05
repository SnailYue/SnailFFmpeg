//
// Created by surface on 2020/9/2.
//

#ifndef SNAILFFMPEG_MP4TOAVI_H
#define SNAILFFMPEG_MP4TOAVI_H

#include <jni.h>

extern "C" {

#include "include/libavformat/avformat.h"

int transformMp4(const char *in_path, const char *out_path) {
    AVOutputFormat *ofmt = NULL;
    AVBitStreamFilterContext *vbsf = NULL;
    /**
     * 定义输入输出avformatcontext
     */
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;

    AVPacket pkt;

    int ret;
    int frame_index = 0;
    /**
     * 初始化
     */
    av_register_all();
    /**
     * 打开输入文件
     */
    if ((ret = avformat_open_input(&ifmt_ctx, in_path, 0, 0)) < 0) {
        printf("Could not to open input file!!");
        goto end;
    }
    /**
     * 获取视频流
     */
    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        printf("Failed tp retrieve input stream information!!!");
        goto end;
    }
    /**
     * 初始化mp4的封装模式
     */
    vbsf = av_bitstream_filter_init("h264_mp4toannexb");
    av_dump_format(ifmt_ctx, 0, in_path, 0);
    /**
     * 初始化输出视频码流的avformatcontext
     */
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_path);
    if (!ofmt_ctx) {
        printf("Could not create output context");
        goto end;
    }
    ofmt = ofmt_ctx->oformat;
    /**
     * 遍历所有的流
     */
    for (int i = 0; i < ifmt_ctx->nb_streams; ++i) {
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
        if (!out_stream) {
            printf("Failed allocating output stream");
            ret = AVERROR_UNKNOWN;
            goto end;
        }
        /**
         * 复制AVCodecContext的设置属性
         */
        if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
            printf("Failed to copu context from input to output stream codec context");
            goto end;
        }
        out_stream->codec->codec_tag = 0;
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
            out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }
    }
    /**
     * 输出信息
     */
    av_dump_format(ofmt_ctx, 0, out_path, 1);
    /**
     * 打开输出文件
     */
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        /**
         * 打开输出文件
         */
        ret = avio_open(&ofmt_ctx->pb, out_path, AVIO_FLAG_WRITE);
        if (ret < 0) {
            printf("Could not open output file");
            goto end;
        }
    }
    /**
     * 写头文件
     */
    if (avformat_write_header(ofmt_ctx, NULL) < 0) {
        printf("Error occurred when opening output file");
        goto end;
    }
    while (1) {
        AVStream *in_stream, *out_stream;
        /**
         * 得到一个AVPacket
         */
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0) {
            break;
        }
        in_stream = ifmt_ctx->streams[pkt.stream_index];
        out_stream = ofmt_ctx->streams[pkt.stream_index];
        /**
         * 转换pts/dts
         */
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                                   (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                                   (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        if (pkt.stream_index == 0) {
            AVPacket fpkt = pkt;
            int a = av_bitstream_filter_filter(vbsf, out_stream->codec, NULL, &fpkt.data,
                                               &fpkt.size,
                                               pkt.data, pkt.size,
                                               fpkt.flags & AV_PKT_FLAG_KEY);
            pkt.data = fpkt.data;
            pkt.size = fpkt.size;
        }
        /**
         * 写帧信息
         */
        if (av_write_frame(ofmt_ctx, &pkt) < 0) {
            printf("Error muxing packet");
            break;
        }
        printf("Write %8d frame to output file", frame_index);
        av_packet_unref(&pkt);
        frame_index++;
    }
    /**
     * 写文件尾
     */
    av_write_trailer(ofmt_ctx);

    /**
     * end
     */
    end:
    avformat_close_input(&ifmt_ctx);
    /**
     * 关闭输出
     */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE)) {
        avio_close(ofmt_ctx->pb);
    }
    avformat_free_context(ofmt_ctx);
    return ret;
}

}

#endif //SNAILFFMPEG_MP4TOAVI_H
