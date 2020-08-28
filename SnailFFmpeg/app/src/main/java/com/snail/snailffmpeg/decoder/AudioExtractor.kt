package com.snail.snailffmpeg.decoder

import android.media.MediaFormat
import com.snail.snailffmpeg.base.IExtractor
import com.snail.snailffmpeg.base.MMExtractor
import java.nio.ByteBuffer


/**
 *   音频数据提取器
 */
class AudioExtractor(path: String) : IExtractor {

    private val mMediaExtractor = MMExtractor(path)

    /**
     * 获取音频轨道参数
     */
    override fun getFormat(): MediaFormat? {
        return mMediaExtractor.getAudioFormat()
    }

    /**
     * 读取数据
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
     * 获取音频轨道索引
     */
    fun getAudioTrack(): Int {
        return mMediaExtractor.getAudioTrack()
    }
}