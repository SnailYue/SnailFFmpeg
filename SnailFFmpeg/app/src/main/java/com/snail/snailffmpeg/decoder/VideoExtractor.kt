package com.snail.snailffmpeg.decoder

import android.media.MediaFormat
import com.snail.snailffmpeg.base.IExtractor
import com.snail.snailffmpeg.base.MMExtractor
import java.nio.ByteBuffer

/**
 * 视频数据提取器
 */
class VideoExtractor(path: String) : IExtractor {

    private val mMediaExtractor = MMExtractor(path)

    /**
     * 视频通道参数
     */
    override fun getFormat(): MediaFormat? {
        return mMediaExtractor.getVideoFormat()
    }

    /**
     * 读取视频数据
     */
    override fun readBuffer(byteBuffer: ByteBuffer): Int {
        return mMediaExtractor.readBuffer(byteBuffer)
    }

    /**
     * 获取当前采样时间
     */
    override fun getCurrentTimeStamp(): Long {
        return mMediaExtractor.getCurrentTimeStamp()
    }

    /**
     * 获取当前采样标志
     */
    override fun getSampleFlag(): Int {
        return mMediaExtractor.getSampleFlag()
    }

    /**
     * seek到指定位置
     */
    override fun seek(position: Long): Long {
        return mMediaExtractor.seek(position)
    }

    /**
     * 设置开始位置
     */
    override fun setStartPosition(position: Long) {
        mMediaExtractor.setStartPos(position)
    }

    /**
     * 停止
     */
    override fun stop() {
        mMediaExtractor.stop()
    }

    /**
     * 获取视频通道索引
     */
    fun getVideoTrack(): Int {
        return mMediaExtractor.getVideoTrack()
    }
}