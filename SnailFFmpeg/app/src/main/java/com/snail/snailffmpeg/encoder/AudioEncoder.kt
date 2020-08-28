package com.snail.snailffmpeg.encoder

import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaFormat
import android.util.Log
import com.snail.snailffmpeg.base.BaseEncoder
import com.snail.snailffmpeg.base.MMuxer
import java.nio.ByteBuffer


/**
 * 音频编码器类
 */
class AudioEncoder(muxer: MMuxer) : BaseEncoder(muxer) {

    private val TAG = AudioEncoder::class.java.simpleName

    /**
     * 编码类型
     */
    override fun encodeType(): String {
        return "audio/mp4a-latm"
    }

    /**
     * 配置编码器的参数
     */
    override fun configEncoder(codec: MediaCodec) {
        val audioFormat = MediaFormat.createAudioFormat(encodeType(), 44100, 2)
        audioFormat.setInteger(MediaFormat.KEY_BIT_RATE, 44100)
        audioFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 100 * 1024)

        try {
            configEncodeWithCQ(codec, audioFormat)
        } catch (e: Exception) {
            e.printStackTrace()
            try {
                configEncodeWithVBR(codec, audioFormat)
            } catch (e: Exception) {
                e.printStackTrace()
                Log.d(TAG, "配置音频参数失败")
            }
        }
    }

    /**
     * 设置CQ码率
     */
    private fun configEncodeWithCQ(codec: MediaCodec, outputFormat: MediaFormat) {
        outputFormat.setInteger(
            MediaFormat.KEY_BITRATE_MODE,
            MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CQ
        )
        codec.configure(outputFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)
    }

    /**
     * 设置VBR码率
     */
    private fun configEncodeWithVBR(codec: MediaCodec, outputFormat: MediaFormat) {
        outputFormat.setInteger(
            MediaFormat.KEY_BITRATE_MODE,
            MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_VBR
        )
        codec.configure(outputFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)
    }

    override fun addTrack(muxer: MMuxer, mediaFormat: MediaFormat) {
        muxer.addAudioTrack(mediaFormat)
    }

    override fun frameWaitTimeMs(): Long {
        return 5
    }

    /**
     * 写音频数据
     */
    override fun writeData(
        muxer: MMuxer,
        byteBuffer: ByteBuffer,
        bufferInfo: MediaCodec.BufferInfo
    ) {
        muxer.writeAudioData(byteBuffer, bufferInfo)
    }

    /**
     * 释放
     */
    override fun release(muxer: MMuxer) {
        muxer.releaseAudioTrack()
    }

}