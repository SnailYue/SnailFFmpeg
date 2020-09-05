package com.snail.ffmpeg.transform

import com.snail.ffmpeg.constant.MediaForm


object MediaTransFormUtil {

    init {
        System.loadLibrary("transform")
        native_init()
    }

    fun transformMedia(form: MediaForm, inPath: String, outPath: String) {
        when (form) {
            MediaForm.MP4TOAVI -> {
                native_mp4_to_avi(inPath, outPath)
            }
        }
    }

    external fun native_init()

    external fun native_mp4_to_avi(inPath: String, outPath: String)
}