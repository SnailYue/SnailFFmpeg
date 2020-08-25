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

    override fun getFormat(): MediaFormat? {
        return mMediaExtractor.getAudioFormat()
    }

    override fun readBuffer(byteBuffer: ByteBuffer): Int {
        return mMediaExtractor.readBuffer(byteBuffer)
    }

    override fun getCurrentTimeStamp(): Long {
        return mMediaExtractor.getCurrentTimeStamp()
    }

    override fun getSampleFlag(): Int {
        return mMediaExtractor.getSampleFlag()
    }

    override fun seek(position: Long): Long {
        return mMediaExtractor.seek(position)
    }

    override fun setStartPosition(position: Long) {
        mMediaExtractor.setStartPos(position)
    }

    override fun stop() {
        mMediaExtractor.stop()
    }
}