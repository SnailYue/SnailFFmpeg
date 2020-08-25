package com.snail.snailffmpeg.decoder

import android.media.*
import com.snail.snailffmpeg.base.BaseDecoder
import com.snail.snailffmpeg.base.IExtractor
import java.nio.ByteBuffer

class AudioDecoder(path: String) : BaseDecoder(path) {

    private var mSampleRate = -1

    private var mChannels = 1

    private var mPCMEncodeBit = AudioFormat.ENCODING_PCM_16BIT

    private var mAudiTrack: AudioTrack? = null

    private var mAudioOutTempBuf: ShortArray? = null


    override fun check(): Boolean {
        return true
    }

    override fun initExtractor(path: String): IExtractor {
        return AudioExtractor(path)
    }

    override fun initSpecParams(format: MediaFormat) {
        mChannels = format.getInteger(MediaFormat.KEY_CHANNEL_COUNT)
        mSampleRate = format.getInteger(MediaFormat.KEY_SAMPLE_RATE)
        mPCMEncodeBit =
            if (format.containsKey(MediaFormat.KEY_PCM_ENCODING))
                format.getInteger(MediaFormat.KEY_PCM_ENCODING)
            else
                AudioFormat.ENCODING_PCM_16BIT
    }

    override fun configCodec(codec: MediaCodec, format: MediaFormat): Boolean {
        codec.configure(format, null, null, 0)
        return true
    }

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

    override fun render(outputBuffer: ByteBuffer, bufferInfo: MediaCodec.BufferInfo) {
        if (mAudioOutTempBuf!!.size < bufferInfo.size / 2) {
            mAudioOutTempBuf = ShortArray(bufferInfo.size / 2)
        }
        outputBuffer.position(0)
        outputBuffer.asShortBuffer().get(mAudioOutTempBuf, 0, bufferInfo.size / 2)
        mAudiTrack!!.write(mAudioOutTempBuf!!, 0, bufferInfo.size / 2)
    }

    override fun doneDecode() {
        mAudiTrack?.stop()
        mAudiTrack?.release()
    }

}