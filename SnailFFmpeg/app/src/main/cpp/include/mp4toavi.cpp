//
// Created by surface on 2020/7/12.
//

#include "mp4toavi.h"

int mp4toavi(const char *in_filename, const char *out_filename) {

    AVOutputFormat *ofmt = NULL;
    AVBitStreamFilterContext *vbsf = NULL;

    /**
     * 输入输出AVFormatContext
     */
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVPacket pkt;

    int ret;
    int frame_index = 0;

    /**
     * 初始化ffmpeg
     */
    av_register_all();

    /**
     * 打开输入文件
     */
    if (ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) {
        printf("Could not open input files.");
        return -1;
    }
    /**
     * 获取文件的流信息
     */
    if (ret = avformat_find_stream_info(ifmt_ctx, 0) < 0) {
        printf("Failed to retrieve input stream information.");
        return -1;
    }
    /**
     * 使用h.264转换
     */
    vbsf = av_bitstream_filter_init("h264_maptoannexb");

    /**
     * 输出输入的信息
     */
    av_dump_format(ifmt_ctx, 0, in_filename, 0);

    /**
     * 初始化输出视频码流的avformatContext
     */
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);

    if (!ofmt_ctx) {
        printf("Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        return -1;
    }

    ofmt = ofmt_ctx->oformat;
    /**
     * 通过输入的avstream创建输出的avstream
     */
    for (int i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream *in_stream = ifmt_ctx->streams[i];
        /**
         * 创建新的输入流
         */
        AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
        if (!out_stream) {
            printf("Failed allocation output stram\n");
            ret = AVERROR_UNKNOWN;
            return -1;
        }
        /**
         * 复制avcodecContext的设置属性
         */
        if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
            printf("Failed to copy context from input to output stream codec context\n");
            return -1;
        }

        out_stream->codec->codec_tag = 0;
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
            out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }
    }
    /**
     * 输出信息
     */
    av_dump_format(ofmt_ctx, 0, out_filename, 1);
    /**
     * 打开输出文件
     */
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            printf("Could not open output file");
            return -1;
        }
    }

    /**
     * 写头文件
     */
    if (avformat_write_header(ofmt_ctx, NULL) < 0) {
        printf("Error occurred when opening output file");
        return -1;
    }

    while (1) {
        AVStream *in_stream, *out_stream;
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
            av_bitstream_filter_filter(vbsf, out_stream->codec, NULL, &fpkt.data,
                                               &fpkt.size,
                                               pkt.data, pkt.size, pkt.flags & AV_PKT_FLAG_KEY);
            pkt.data = fpkt.data;
            pkt.size = fpkt.size;
        }
        /**
         * 写avPacket
         */
        if (av_write_frame(ofmt_ctx, &pkt) < 0) {
            printf("Error muxing packet\n");
            break;
        }
        printf("Write frame to output file\n");
        av_packet_unref(&pkt);
        frame_index++;
    }
    /**
     * 写文件尾
     */
    av_write_trailer(ofmt_ctx);
    /**
     * 关闭输出
     */
    avformat_close_input(&ifmt_ctx);
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE)) {
        avio_close(ofmt_ctx->pb);
    }
    avformat_free_context(ofmt_ctx);
    system("pause");
    return 0;
}
