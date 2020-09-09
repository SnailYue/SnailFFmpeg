//
// Created by surface on 2020/9/2.
//

#ifndef SNAILFFMPEG_MP4TOAVI_H
#define SNAILFFMPEG_MP4TOAVI_H

#include <jni.h>

extern "C" {

#include "include/libavformat/avformat.h"

}

class Mp4ToAVI {
public:
    static int transformMp4(const char *in_path, const char *out_path);
};

#endif //SNAILFFMPEG_MP4TOAVI_H
