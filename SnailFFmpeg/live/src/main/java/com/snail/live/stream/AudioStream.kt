package com.snail.live.stream

import android.media.AudioFormat
import android.media.AudioRecord
import android.media.MediaRecorder
import com.snail.live.LivePusher
import com.snail.live.model.AudioBean
import com.snail.snailffmpeg.utils.PLog
import kotlinx.coroutines.*


/**
 * 音频流
 */
class AudioStream {
    var inputSample: Int = 0
    var audioRecord: AudioRecord? = null
    var mLivePusher: LivePusher? = null

    /**
     * 是否推流
     */
    var isLiving: Boolean = false

    /**
     * 是否静音
     */
    var isMute: Boolean = false
        set(value) {
            isMute = value
        }

    /**
     * 协程
     */
    val scope = CoroutineScope(Job())

    constructor(livePusher: LivePusher, audioBean: AudioBean) {
        mLivePusher = livePusher
        var channelConfig =
            if (audioBean.numChannels == 2) AudioFormat.CHANNEL_IN_STEREO else AudioFormat.CHANNEL_IN_MONO

        mLivePusher?.setAudioCodecInfo(audioBean.sampleRate, audioBean.numChannels)
        inputSample = mLivePusher?.getInputSample()!! * 2

        var minBufferSize = AudioRecord.getMinBufferSize(
            audioBean.sampleRate,
            channelConfig,
            audioBean.audioFormat
        ) * 2

        var bufferSizeInBytes = if (minBufferSize > inputSample) minBufferSize else inputSample
        audioRecord = AudioRecord(
            MediaRecorder.AudioSource.MIC,
            audioBean.sampleRate,
            channelConfig,
            audioBean.audioFormat,
            bufferSizeInBytes
        )
    }

    /**
     * 开始推流
     */
    fun startLive() {
        isLiving = true
        /**
         * 使用协程
         */
        scope.launch {
            supervisorScope {
                launch {
                    audioRecord?.startRecording()
                    var bytes = ByteArray(inputSample)
                    while (isLiving) {
                        PLog.d("pushAudio")
                        var len = audioRecord?.read(bytes, 0, bytes.size)
                        if (len!! > 0) {
                            mLivePusher?.pushAudio(bytes)
                        }
                    }
                    audioRecord?.stop()
                }
            }
        }
    }

    /**
     * 停止推流
     */
    fun stopLive() {
        isLiving = false
    }

    /**
     * 释放推流
     */
    fun release() {
        scope.cancel()
        audioRecord?.release()
    }

}