//
// Created by surface on 2020/6/29.
//

#include <string>
#include <iostream>
#include "init_lib.h"
#include "libavformat/avformat.h"

using namespace std;

AVOutputFormat *fmt;
AVFormatContext *oc;

AVStream *st;
AVCodecContext *c;


/**
 * 注册API
 */
static void register_api() {
    av_register_all();
    cout << "register api" << endl;
}

/**
 * 申请avformat
 * @param filename
 */
static void init_format() {
    avformat_alloc_output_context2(&oc, NULL, "flv", "");
    if (!oc) {
        return;
    }
    fmt = oc->oformat;
    cout << "init format" << endl;
}


/**
 * 申请avstream
 */
static void init_avstream() {
    st = avformat_new_stream(oc, NULL);
    if (!st) {
        exit(1);
    }
    st->id = oc->nb_streams - 1;
    cout << "init AVStream" << endl;
}


/**
 * 初始化ffmpeg
 */

static void init_ffmpeg() {
    register_api();
    init_format();
    init_avstream();
}







