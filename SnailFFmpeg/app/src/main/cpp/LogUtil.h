//
// Created by surface on 2020/8/18.
//

#ifndef SNAILFFMPEG_LOGUTIL_H
#define SNAILFFMPEG_LOGUTIL_H

#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,"Live",__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,"Live",__VA_ARGS__)

#endif //SNAILFFMPEG_LOGUTIL_H
