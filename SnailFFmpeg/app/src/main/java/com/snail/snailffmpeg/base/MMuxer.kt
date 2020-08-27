package com.snail.snailffmpeg.base

import android.media.MediaCodec
import android.media.MediaFormat
import android.media.MediaMuxer
import android.os.Environment
import java.lang.IllegalArgumentException
import java.nio.ByteBuffer


/**
 * 音视频封装器
 * 1.addTrack() 添加音视频轨道
 * 2.start() 开始写入数据
 * 3.writeSampleData() 想Muxer写入数据
 * 4.stop() 停止写入数据
 * 5.release() 释放Muxer资源
 *
 */
class MMuxer(fileName: String) {

    private var mPath: String? = null

    /**
     * 媒体合成器
     */
    private var mMediaMuxer: MediaMuxer? = null

    /**
     * 视频轨道索引
     */
    private var mVideoTrackIndex = -1

    /**
     * 音频轨道索引
     */
    private var mAudioTrackIndex = -1

    /**
     * 判断音频轨道是否添加
     */
    private var mIsAudioTrackAdd = false

    /**
     * 判断视频轨道是否添加
     */
    private var mIsVideoTrackAdd = false

    private var mIsAudioEnd = false

    private var mIsVideoEnd = false

    private var mIsStart = false

    private var mStateListener: IMuxerStateListener? = null

    init {
        if (fileName.isEmpty()) {
            throw IllegalArgumentException("fileName can not empty")
        }
        val filePath = Environment.getExternalStorageDirectory().absolutePath.toString() + "/"
        mPath = filePath + fileName
        /**
         * 设置输出路径及格式
         */
        mMediaMuxer = MediaMuxer(mPath!!, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4)
    }

    /**
     * 添加视频轨道
     */
    fun addVideoTrack(mediaFormat: MediaFormat) {
        if (mIsVideoTrackAdd) return
        if (mMediaMuxer != null) {
            mVideoTrackIndex = mMediaMuxer!!.addTrack(mediaFormat)
            mIsVideoTrackAdd = true
            startMuxer()
        }
    }

    /**
     * 添加音频轨道
     */
    fun addAudioTrack(mediaFormat: MediaFormat) {
        if (mIsAudioTrackAdd) return
        if (mMediaMuxer != null) {
            mAudioTrackIndex = mMediaMuxer!!.addTrack(mediaFormat)
            mIsAudioTrackAdd = true
            startMuxer()
        }
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

    /**
     * 释放视频轨道相关
     */
    fun releaseVideoTrack() {
        mIsVideoEnd = true
        release()
    }

    /**
     * 释放视频轨道相关
     */
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

    /**
     * 合成器状态监听接口
     */
    interface IMuxerStateListener {
        fun onMuxerStart() {}

        fun onMuxerFinish() {}
    }

}