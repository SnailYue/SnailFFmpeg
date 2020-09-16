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

    fun screenshotFromStream(url: String, name: String) {
        native_screenshot_from_stream(url, name)
    }

    fun addPictureLogo(video: String, picture: String, output: String) {
        native_add_logo(video, picture, output);
    }

    fun startPlayer(videoUrl: String, surfaceView: Any) {
        native_start_player(videoUrl, surfaceView)
    }

    external fun native_init()

    external fun native_mp4_to_avi(inPath: String, outPath: String)

    external fun native_screenshot_from_stream(url: String, outputName: String)

    external fun native_add_logo(videoUrl: String, pictureUrl: String, outputName: String)

    external fun native_start_player(videoUrl: String, surface: Any)
}