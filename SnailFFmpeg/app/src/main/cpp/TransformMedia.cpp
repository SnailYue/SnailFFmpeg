
//
// Created by surface on 2020/9/2.
//

#include <jni.h>

#include "Mp4ToAVI.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_ffmpeg_transform_MediaTransFormUtil_native_1init(JNIEnv *env,
                                                                            jobject thiz) {

}

extern "C"
JNIEXPORT void JNICALL
Java_com_snail_snailffmpeg_ffmpeg_transform_MediaTransFormUtil_native_1mp4_1to_1avi(JNIEnv *env,
                                                                                    jobject thiz,
                                                                                    jstring in_path,
                                                                                    jstring out_path
) {
    const char *_in_path = env->GetStringUTFChars(in_path, 0);
    const char *_out_path = env->GetStringUTFChars(out_path, 0);
    transformMp4(_in_path, _out_path);
    env->ReleaseStringUTFChars(in_path, _in_path);
    env->ReleaseStringUTFChars(out_path, _out_path);
}