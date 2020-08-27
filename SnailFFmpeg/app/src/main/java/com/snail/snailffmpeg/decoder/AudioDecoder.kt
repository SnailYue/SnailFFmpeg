package com.snail.snailffmpeg.decoder

import android.media.*
import com.snail.snailffmpeg.base.BaseDecoder
import com.snail.snailffmpeg.base.IExtractor
import java.nio.ByteBuffer


/**
 * 音频解码器类
 */
class AudioDecoder(path: String) : BaseDecoder(path) {

    private var mSampleRate = -1

    private var mChannels = 1

    private var mPCMEncodeBit = AudioFormat.ENCODING_PCM_16BIT

    private var mAudiTrack: AudioTrack? = null

    private var mAudioOutTempBuf: ShortArray? = null


    override fun check(): Boolean {
        return true
    }

    /**
     * 实例音频提取器
     */
    override fun initExtractor(path: String): IExtractor {
        return AudioExtractor(path)
    }

    /**
     * 初始化特殊参数
     */
    override fun initSpecParams(format: MediaFormat) {
        mChannels = format.getInteger(MediaFormat.KEY_CHANNEL_COUNT)
        mSampleRate = format.getInteger(MediaFormat.KEY_SAMPLE_RATE)
        mPCMEncodeBit =
            if (format.containsKey(MediaFormat.KEY_PCM_ENCODING))
                format.getInteger(MediaFormat.KEY_PCM_ENCODING)
            else
                AudioFormat.ENCODING_PCM_16BIT
    }

    /**
     * 配置解码器
     */
    override fun configCodec(codec: MediaCodec, format: MediaFormat): Boolean {
        codec.configure(format, null, null, 0)
        return true
    }

    /**
     * 初始化渲染器
     */
    override fun initRender(): Boolean {
        val channel = if (mChannels == 1) {
            AudioFormat.CHANNEL_OUT_MONO
        } else {
            AudioFormat.CHANNEL_OUT_STEREO
        }

        val minBufferSize = AudioTrack.getMinBufferSize(mSampleRate, channel, mPCMEncodeBit)
        mAudiTrack = AudioTrack(
            AudioManager.STREAM_MUSIC,
            mSampleRate,
            channel,
            mPCMEncodeBit,
            minBufferSize,
            AudioTrack.MODE_STREAM
        )
        mAudiTrack!!.play()
        return true
    }

    /**
     * 渲染
     */
    override fun render(outputBuffer: ByteBuffer, bufferInfo: MediaCodec.BufferInfo) {
        if (mAudioOutTempBuf!!.size < bufferInfo.size / 2) {
            mAudioOutTempBuf = ShortArray(bufferInfo.size / 2)
        }
        outputBuffer.position(0)
        outputBuffer.asShortBuffer().get(mAudioOutTempBuf, 0, bufferInfo.size / 2)
        mAudiTrack!!.write(mAudioOutTempBuf!!, 0, bufferInfo.size / 2)
    }

    /**
     * 解码完成
     */
    override fun doneDecode() {
        mAudiTrack?.stop()
        mAudiTrack?.release()
    }

    override fun getTrack(): Int {
        return (mExtractor as AudioExtractor).getAudioTrack()
    }

}