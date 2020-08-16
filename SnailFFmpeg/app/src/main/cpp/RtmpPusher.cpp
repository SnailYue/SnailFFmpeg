//
// Created by surface on 2020/8/16.
//

#include <jni.h>

#define RTMP_PUSHER_FUNC(RETURN_TYPE, FUNC_NAME, ...) \
    extern "C" \
    JNIEXPORT RETURN_TYPE JNICALL Java_com_snail_snailffmpeg_LivePusher_ ## FUNC_NAMR \
    (JNIEnv *env,jobject instance, ##__VA_ARGS__) \


RTMP_PUSHER_FUNC(void, native_1init) {

}
