package com.snail.snailffmpeg.decoder

import android.media.MediaFormat
import com.snail.snailffmpeg.base.IExtractor
import java.nio.ByteBuffer

class VideoExtractor(path: String) : IExtractor {


    override fun getFormat(): MediaFormat? {

    }

    override fun readBuffer(byteBuffer: ByteBuffer): Int {

    }

    override fun getCurrentTimeStamp(): Long {

    }

    override fun getSampleFlag(): Int {

    }

    override fun seek(position: Long): Long {
    }

    override fun setStartPosition(position: Long) {
    }

    override fun stop() {
    }
}