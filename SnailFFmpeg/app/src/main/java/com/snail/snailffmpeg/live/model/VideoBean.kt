package com.snail.snailffmpeg.live.model

data class VideoBean(
    val width: Int,
    val height: Int,
    val cameraId: Int,
    val bitRate: Int,
    val frameRate: Int
)