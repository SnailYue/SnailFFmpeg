package com.snail.snailffmpeg.model

data class VideoInfo(
    var width: Int,
    var height: Int,
    var bitrate: Int = 480000,
    var fps: Int = 25,
    var cameraId: Int
)