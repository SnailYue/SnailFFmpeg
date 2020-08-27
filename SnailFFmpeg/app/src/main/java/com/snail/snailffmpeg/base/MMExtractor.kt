package com.snail.snailffmpeg.base

import android.media.MediaExtractor
import android.media.MediaFormat
import java.nio.ByteBuffer


/**
 * 音视频分离器
 */
class MMExtractor(path: String?) {

    /**
     * 音视频分离器
     */
    private var mExtractor: MediaExtractor? = null

    /**
     * 音频通道索引
     */
    private var mAudioTrack = -1

    /**
     * 视频通道索引
     */
    private var mVideoTrack = -1

    /**
     * 当前帧时间戳
     */
    private var mCurSampleTime: Long = 0

    /**
     * 当前帧标志
     */
    private var mCurSampleFlags: Int = 0

    /**
     * 开始解码时间点
     */
    private var mStartPos: Long = 0

    /**
     * 初始化
     */
    init {
        mExtractor = MediaExtractor()
        mExtractor?.setDataSource(path!!)
    }


    /**
     * 获取视频格式参数
     */
    fun getVideoFormat(): MediaFormat? {
        for (i in 0..mExtractor!!.trackCount) {
            val mediaFormat = mExtractor!!.getTrackFormat(i)
            var mime = mediaFormat.getString(MediaFormat.KEY_MIME)
            if (mime.startsWith("video/")) {
                mVideoTrack = i
                break
            }
        }
        return if (mVideoTrack >= 0) {
            mExtractor!!.getTrackFormat(mVideoTrack)
        } else null
    }

    /**
     * 获取音频格式参数
     */
    fun getAudioFormat(): MediaFormat? {
        for (i in 0..mExtractor!!.trackCount) {
            var mediaFormat = mExtractor!!.getTrackFormat(i)
            var mime = mediaFormat.getString(MediaFormat.KEY_MIME)
            if (mime.startsWith("audio/")) {
                mAudioTrack = i
                break
            }
        }
        return if (mAudioTrack >= 0) {
            mExtractor!!.getTrackFormat(mAudioTrack)
        } else null
    }

    /**
     * 读取视频数据
     */
    fun readBuffer(byteBuffer: ByteBuffer): Int {
        byteBuffer.clear()
        selectSourceTrack()
        var readSampleCount = mExtractor!!.readSampleData(byteBuffer, 0)
        if (readSampleCount < 0) {
            return -1
        }
        mCurSampleTime = mExtractor!!.sampleTime
        mCurSampleFlags = mExtractor!!.sampleFlags
        mExtractor!!.advance()
        return readSampleCount
    }

    /**
     * 选择通道
     */
    fun selectSourceTrack() {
        if (mVideoTrack >= 0) {
            mExtractor!!.selectTrack(mVideoTrack)
        } else if (mAudioTrack >= 0) {
            mExtractor!!.selectTrack(mAudioTrack)
        }
    }

    /**
     * seek到指定位置
     */
    fun seek(pos: Long): Long {
        mExtractor!!.seekTo(pos, MediaExtractor.SEEK_TO_PREVIOUS_SYNC)
        return mExtractor!!.sampleTime
    }

    /**
     * 释放音视频分离器
     */
    fun stop() {
        mExtractor!!.release()
        mExtractor = null
    }

    fun getVideoTrack(): Int {
        return mVideoTrack
    }

    fun getAudioTrack(): Int {
        return mAudioTrack
    }

    fun setStartPos(pos: Long) {
        mStartPos = pos
    }

    /**
     * 当前帧时间
     */
    fun getCurrentTimeStamp(): Long {
        return mCurSampleTime
    }

    fun getSampleFlag(): Int {
        return mCurSampleFlags
    }
}