package com.snail.snailffmpeg.codec

import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaFormat
import java.nio.ByteBuffer

class AVMediaCodec {
    var mWidth: Int = 0
    var mHeight: Int = 0
    var mMediaCodec: MediaCodec? = null
    var mInfo: ByteArray? = null

    /**
     * 构造函数
     */
    constructor(width: Int, height: Int, frameRate: Int, bitRate: Int) {
        mWidth = width
        mHeight = height
        mMediaCodec = MediaCodec.createDecoderByType("video/avc")
        var mediaFormat = MediaFormat.createAudioFormat("video/avc", width, height)
        mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, bitRate)
        mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE, frameRate)
        mediaFormat.setInteger(
            MediaFormat.KEY_COLOR_FORMAT,
            MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface
        )
        mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1)
        mediaFormat.setInteger(
            MediaFormat.KEY_BITRATE_MODE,
            MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_VBR
        )
        mMediaCodec?.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)
        mMediaCodec?.start()
    }

    /**
     * 关闭解码器
     */
    fun closeEncoder() {
        mMediaCodec?.apply {
            stop()
            release()
        }
    }

    fun offerEncoder(input: ByteArray, output: ByteArray) {
        var pos = 0
        var inputBuffers = mMediaCodec?.inputBuffers
        var outputBuffers = mMediaCodec?.outputBuffers
        var inputBufferIndex: Int = mMediaCodec?.dequeueInputBuffer(-1)!!
        if (inputBufferIndex >= 0) {
            var inputBuffer: ByteBuffer = inputBuffers?.get(inputBufferIndex)!!
            inputBuffer.clear()
            inputBuffer.put(input)
            mMediaCodec?.queueInputBuffer(inputBufferIndex, 0, input.size, 0, 0)
        }


    }

}