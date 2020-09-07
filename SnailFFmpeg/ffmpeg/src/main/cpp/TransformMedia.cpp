
//
// Created by surface on 2020/9/2.
//

#include <jni.h>

#include "VideoScreenshot.h"
#include "AddLogo.h"

extern "C" {

#include "Mp4ToAVI.h"

void JNICALL
Java_com_snail_ffmpeg_transform_MediaTransFormUtil_native_1init(JNIEnv *env,
                                                                jobject thiz) {

}


void JNICALL
Java_com_snail_ffmpeg_transform_MediaTransFormUtil_native_1mp4_1to_1avi(JNIEnv *env,
                                                                        jobject thiz,
                                                                        jstring in_path,
                                                                        jstring out_path
) {
    printf("native_1mp4_1to_1avi");
    const char *_in_path = env->GetStringUTFChars(in_path, 0);
    const char *_out_path = env->GetStringUTFChars(out_path, 0);
    transformMp4(_in_path, _out_path);
    env->ReleaseStringUTFChars(in_path, _in_path);
    env->ReleaseStringUTFChars(out_path, _out_path);
}


void JNICALL
Java_com_snail_ffmpeg_transform_MediaTransFormUtil_native_1screenshot_1from_1stream(JNIEnv *env,
                                                                                    jobject thiz,
                                                                                    jstring url,
                                                                                    jstring output_name) {
    printf("native_1screenshot_1from_1stream");
    const char *_url = env->GetStringUTFChars(url, 0);
    const char *_output_name = env->GetStringUTFChars(output_name, 0);
    screenShot(_url, _output_name);
    env->ReleaseStringUTFChars(url, _url);
    env->ReleaseStringUTFChars(output_name, _output_name);
}


void JNICALL
Java_com_snail_ffmpeg_transform_MediaTransFormUtil_native_1add_1logo(JNIEnv *env, jobject thiz,
                                                                     jstring video_url,
                                                                     jstring picture_url,
                                                                     jstring output_name) {
    const char *_video_url = env->GetStringUTFChars(video_url, 0);
    const char *_picture_url = env->GetStringUTFChars(picture_url, 0);
    const char *_output_name = env->GetStringUTFChars(output_name, 0);
    add_logo(_video_url, _picture_url, _output_name);
    env->ReleaseStringUTFChars(video_url, _video_url);
    env->ReleaseStringUTFChars(picture_url, _picture_url);
    env->ReleaseStringUTFChars(output_name, _output_name);

}

}