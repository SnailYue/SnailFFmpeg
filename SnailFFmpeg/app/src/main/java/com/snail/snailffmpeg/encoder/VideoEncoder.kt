package com.snail.snailffmpeg.encoder

import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaFormat
import android.os.Build
import android.util.Log
import android.view.Surface
import com.snail.snailffmpeg.base.BaseEncoder
import com.snail.snailffmpeg.base.MMuxer
import java.lang.Exception
import java.lang.IllegalArgumentException
import java.nio.ByteBuffer

/**
 * 视频编码器类
 *
 * 用于设置视频通道的参数及控制数据的写入
 */
class VideoEncoder(muxer: MMuxer, width: Int, height: Int) : BaseEncoder(muxer, width, height) {

    private val TAG = VideoEncoder::class.java.simpleName

    private var mSurface: Surface? = null

    override fun encodeType(): String {
        return "video/avc"
    }

    override fun configEncoder(codec: MediaCodec) {
        if (mWidth <= 0 || mHeight <= 0) {
            throw IllegalArgumentException("Encode width or height is invalid, width: $mWidth, height: $mHeight")
        }
        val bitrate = 3 * mWidth * mHeight
        val outputFormat = MediaFormat.createVideoFormat(encodeType(), mWidth, mHeight)
        outputFormat.setInteger(MediaFormat.KEY_BIT_RATE, bitrate)
        outputFormat.setInteger(MediaFormat.KEY_FRAME_RATE, 30)
        outputFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1)
        outputFormat.setInteger(
            MediaFormat.KEY_COLOR_FORMAT,
            MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface
        )

        try {
            configEncoderWithCQ(codec, outputFormat)
        } catch (e: Exception) {
            e.printStackTrace()
            // 捕获异常，设置为系统默认配置 BITRATE_MODE_VBR
            try {
                configEncoderWithVBR(codec, outputFormat)
            } catch (e: Exception) {
                e.printStackTrace()
                Log.e(TAG, "配置视频编码器失败")
            }
        }

        mSurface = codec.createInputSurface()
    }

    private fun configEncoderWithCQ(codec: MediaCodec, outputFormat: MediaFormat) {
        outputFormat.setInteger(
            MediaFormat.KEY_BITRATE_MODE,
            MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CQ
        )
        codec.configure(outputFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)
    }

    private fun configEncoderWithVBR(codec: MediaCodec, outputFormat: MediaFormat) {
        outputFormat.setInteger(
            MediaFormat.KEY_BITRATE_MODE,
            MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_VBR
        )
        codec.configure(outputFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)
    }

    override fun addTrack(muxer: MMuxer, mediaFormat: MediaFormat) {
        muxer.addVideoTrack(mediaFormat)
    }

    override fun writeData(
        muxer: MMuxer,
        byteBuffer: ByteBuffer,
        bufferInfo: MediaCodec.BufferInfo
    ) {
        Log.d(TAG, "writeData Video")
        muxer.writeVideoData(byteBuffer, bufferInfo)
    }

    override fun encodeManually(): Boolean {
        return false
    }

    override fun release(muxer: MMuxer) {
        muxer.releaseVideoTrack()
    }

}