package com.snail.snailffmpeg.base

interface IDecoderProgress {

    fun videoSizeChange(width: Int, height: Int, rotationAngle: Int)

    fun videoProgressChange(position: Long)
}