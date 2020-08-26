package com.snail.snailffmpeg.base

import android.media.MediaCodec
import android.media.MediaFormat
import android.media.MediaMuxer
import android.os.Environment
import java.nio.ByteBuffer

class MMuxer {

    private var mPath: String? = null

    private var mMediaMuxer: MediaMuxer? = null

    private var mVideoTrackIndex = -1

    private var mAudioTrackIndex = -1

    private var mIsAudioTrackAdd = false

    private var mIsVideoTrackAdd = false

    private var mIsAudioEnd = false

    private var mIsVideoEnd = false

    private var mIsStart = false

    private var mStateListener: IMuxerStateListener? = null

    init {
        val fileName = "Snail_Text.mp4"
        val filePath = Environment.getExternalStorageDirectory().absolutePath.toString() + "/"
        mPath = filePath + fileName
        mMediaMuxer = MediaMuxer(mPath!!, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4)
    }

    fun addVideoTrack(mediaFormat: MediaFormat) {
        if (mIsVideoTrackAdd) return
        if (mMediaMuxer != null) {
            mVideoTrackIndex = mMediaMuxer!!.addTrack(mediaFormat)
            mIsVideoTrackAdd = true
            startMuxer()
        }
    }

    fun addAudioTrack(mediaFormat: MediaFormat) {
        if (mIsAudioTrackAdd) return
        if (mMediaMuxer != null) {
            mAudioTrackIndex = mMediaMuxer!!.addTrack(mediaFormat)
            mIsAudioTrackAdd = true
            startMuxer()
        }
    }

    fun setNoVideo() {
        if (mIsVideoTrackAdd) return
        mIsVideoTrackAdd = true
        mIsVideoEnd = true
        startMuxer()
    }

    fun setNoAudio() {
        if (mIsAudioTrackAdd) return
        mIsVideoTrackAdd = true
        mIsAudioEnd = true
        startMuxer()
    }

    /**
     * 写视频数据
     */
    fun writeVideoData(byteBuffer: ByteBuffer, bufferInfo: MediaCodec.BufferInfo) {
        if (mIsStart) {
            mMediaMuxer?.writeSampleData(mVideoTrackIndex, byteBuffer, bufferInfo)
        }
    }

    /**
     * 写音频数据
     */
    fun writeAudioData(byteBuffer: ByteBuffer, bufferInfo: MediaCodec.BufferInfo) {
        if (mIsStart) {
            mMediaMuxer?.writeSampleData(mAudioTrackIndex, byteBuffer, bufferInfo)
        }
    }

    /**
     * 开始封装
     */
    private fun startMuxer() {
        if (mIsAudioTrackAdd && mIsVideoTrackAdd) {
            mMediaMuxer?.start()
            mIsStart = true
            mStateListener?.onMuxerStart()
        }
    }

    fun releaseVideoTrack() {
        mIsVideoEnd = true
        release()
    }

    fun releaseAudioTrack() {
        mIsAudioEnd = true
        release()
    }

    /**
     * 释放封装相关
     */
    private fun release() {
        if (mIsAudioEnd && mIsVideoEnd) {
            mIsVideoTrackAdd = false
            mIsAudioTrackAdd = false
            mMediaMuxer?.stop()
            mMediaMuxer?.release()
            mMediaMuxer = null
            mStateListener?.onMuxerFinish()
        }
    }

    /**
     * 设置监听事件
     */
    fun setStateListener(listener: IMuxerStateListener) {
        mStateListener = listener
    }


    interface IMuxerStateListener {
        fun onMuxerStart() {}

        fun onMuxerFinish() {}
    }

}