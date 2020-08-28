package com.snail.snailffmpeg.encoder

import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaFormat
import android.util.Log
import android.view.Surface
import com.snail.snailffmpeg.base.BaseEncoder
import com.snail.snailffmpeg.base.MMuxer
import java.lang.Exception
import java.lang.IllegalArgumentException
import java.nio.ByteBuffer

/**
 * 视频编码器类
 */
class VideoEncoder(muxer: MMuxer, width: Int, height: Int) : BaseEncoder(muxer, width, height) {

    private val TAG = VideoEncoder::class.java.simpleName

    private var mSurface: Surface? = null

    /**
     * 设置视频编码格式
     */
    override fun encodeType(): String {
        return "video/avc"
    }

    /**
     * 设置视频编码参数
     */
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
            try {
                configEncodeWithVBR(codec, outputFormat)
            } catch (e: Exception) {
                e.printStackTrace()
                Log.e(TAG, "配置视频编码异常")
            }
        }
        mSurface = codec.createInputSurface()
    }

    /**
     * 设置码率 CQ
     */
    private fun configEncoderWithCQ(codec: MediaCodec, outputFormat: MediaFormat) {
        outputFormat.setInteger(
            MediaFormat.KEY_BITRATE_MODE,
            MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CQ
        )
        codec.configure(outputFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)
    }

    /**
     * 设置码率 VBR
     */
    private fun configEncodeWithVBR(codec: MediaCodec, outputFormat: MediaFormat) {
        outputFormat.setInteger(
            MediaFormat.KEY_BITRATE_MODE,
            MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_VBR
        )
        codec.configure(outputFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)
    }

    override fun addTrack(muxer: MMuxer, mediaFormat: MediaFormat) {
        muxer.addVideoTrack(mediaFormat)
    }

    /**
     * 写编码数据
     */
    override fun writeData(
        muxer: MMuxer,
        byteBuffer: ByteBuffer,
        bufferInfo: MediaCodec.BufferInfo
    ) {
        muxer.writeVideoData(byteBuffer, bufferInfo)
    }

    override fun encodeManually(): Boolean {
        return false
    }


    override fun release(muxer: MMuxer) {
        muxer.releaseVideoTrack()
    }

    fun getEncodeSurface(): Surface? {
        return mSurface
    }

}