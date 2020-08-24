package com.snail.snailffmpeg.base

import android.media.MediaFormat

interface IDecoder : Runnable {
    fun pause()

    fun resume()

    fun seekTo(position: Long): Long

    fun seekAndPlay(position: Long): Long

    fun stop()

    fun isDecoding(): Boolean

    fun isSeeking(): Boolean

    fun isStop(): Boolean

    fun setSizeListener(listener: IDecoderProgress)

    fun setStateListener(listener: IDecodeStateListener?)

    fun getHeight(): Int

    fun getWidth(): Int

    fun getDuration(): Int

    fun getRotationAngle(): Int

    fun getCurTimeStamp(): Long

    fun getMediaFormat(): MediaFormat?

    fun getTrack(): Int

    fun getFilePath(): String

    fun withoutSync(): IDecoder
}