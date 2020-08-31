package com.snail.snailffmpeg.encoder

import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaFormat
import android.os.Build
import android.util.Log
import com.snail.snailffmpeg.base.BaseEncoder
import com.snail.snailffmpeg.base.MMuxer
import java.nio.ByteBuffer


/**
 * 音频编码器类
 *
 * 用于设置音频通道的参数及写入操作
 */
class AudioEncoder(muxer: MMuxer) : BaseEncoder(muxer) {

    private val TAG = AudioEncoder::class.java.simpleName

    override fun encodeType(): String {
        return "audio/mp4a-latm"
    }

    override fun configEncoder(codec: MediaCodec) {
        val audioFormat = MediaFormat.createAudioFormat(encodeType(), 44100, 2)
        audioFormat.setInteger(MediaFormat.KEY_BIT_RATE, 128000)
        audioFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 100 * 1024)
        try {
            configEncoderWithCQ(codec, audioFormat)
        } catch (e: Exception) {
            e.printStackTrace()
            try {
                configEncoderWithVBR(codec, audioFormat)
            } catch (e: Exception) {
                e.printStackTrace()
                Log.e(TAG, "配置音频编码器失败")
            }
        }
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
        muxer.addAudioTrack(mediaFormat)
    }

    override fun frameWaitTimeMs(): Long {
        return 5
    }

    override fun writeData(
        muxer: MMuxer,
        byteBuffer: ByteBuffer,
        bufferInfo: MediaCodec.BufferInfo
    ) {
        Log.d(TAG, "writeData Audio")
        muxer.writeAudioData(byteBuffer, bufferInfo)
    }

    override fun release(muxer: MMuxer) {
        muxer.releaseAudioTrack()
    }
}