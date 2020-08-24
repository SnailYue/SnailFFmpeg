package com.snail.snailffmpeg.base

import android.media.MediaFormat
import java.nio.ByteBuffer

interface IExtractor {

    fun getFormat(): MediaFormat?

    fun readBuffer(byteBuffer: ByteBuffer): Int

    fun getCurrentTimeStamp(): Long

    fun getSampleFlag(): Int

    fun seek(position: Long): Long

    fun setStartPosition(position: Long)

    fun stop()
}