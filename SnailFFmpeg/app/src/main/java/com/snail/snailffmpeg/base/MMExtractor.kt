package com.snail.snailffmpeg.base

import android.media.MediaExtractor
import android.media.MediaFormat
import java.nio.ByteBuffer


/**
 * 音视频分离器  多媒体格式分离
 * 1.setDataSource(path : String) 设置文件路径
 * 2.getTrackCount() 获取通道数
 * 3.getTrackFormat(index : Int) 指定通道的通道格式
 * 4.getSampleTime()  返回当前的事件戳
 * 5.readSampleData(byteBuffer : ByteBuffer,offset : Int) 把指定通道中的数据按偏移量读取到ByteBuffer中
 * 6.advance() 读取下一帧数据
 * 7.release() 释放资源
 */
class MMExtractor(path: String?) {

    /**音视频分离器*/
    private var mExtractor: MediaExtractor? = null

    /**音频通道索引*/
    private var mAudioTrack = -1

    /**视频通道索引*/
    private var mVideoTrack = -1

    /**当前帧时间戳*/
    private var mCurSampleTime: Long = 0

    /**当前帧标志*/
    private var mCurSampleFlag: Int = 0

    /**开始解码时间点*/
    private var mStartPos: Long = 0

    init {
        mExtractor = MediaExtractor()
        mExtractor?.setDataSource(path!!)
    }

    /**
     * 获取视频格式参数
     */
    fun getVideoFormat(): MediaFormat? {
        for (i in 0 until mExtractor!!.trackCount) {
            val mediaFormat = mExtractor!!.getTrackFormat(i)
            val mime = mediaFormat.getString(MediaFormat.KEY_MIME)
            if (mime.startsWith("video/")) {
                mVideoTrack = i
                break
            }
        }
        return if (mVideoTrack >= 0)
            mExtractor!!.getTrackFormat(mVideoTrack)
        else null
    }

    /**
     * 获取音频格式参数
     */
    fun getAudioFormat(): MediaFormat? {
        for (i in 0 until mExtractor!!.trackCount) {
            val mediaFormat = mExtractor!!.getTrackFormat(i)
            val mime = mediaFormat.getString(MediaFormat.KEY_MIME)
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
        //记录当前帧的时间戳
        mCurSampleTime = mExtractor!!.sampleTime
        mCurSampleFlag = mExtractor!!.sampleFlags
        //进入下一帧
        mExtractor!!.advance()
        return readSampleCount
    }

    /**
     * 选择通道
     */
    private fun selectSourceTrack() {
        if (mVideoTrack >= 0) {
            mExtractor!!.selectTrack(mVideoTrack)
        } else if (mAudioTrack >= 0) {
            mExtractor!!.selectTrack(mAudioTrack)
        }
    }

    /**
     * Seek到指定位置，并返回实际帧的时间戳
     */
    fun seek(pos: Long): Long {
        mExtractor!!.seekTo(pos, MediaExtractor.SEEK_TO_PREVIOUS_SYNC)
        return mExtractor!!.sampleTime
    }

    /**
     * 停止读取数据
     */
    fun stop() {
        mExtractor?.release()
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
     * 获取当前帧时间
     */
    fun getCurrentTimeStamp(): Long {
        return mCurSampleTime
    }

    fun getSampleFlag(): Int {
        return mCurSampleFlag
    }
}