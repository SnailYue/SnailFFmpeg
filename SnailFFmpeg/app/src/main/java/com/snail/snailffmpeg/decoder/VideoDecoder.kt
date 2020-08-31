package com.snail.snailffmpeg.decoder

import android.media.MediaCodec
import android.media.MediaFormat
import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import com.snail.snailffmpeg.base.BaseDecoder
import com.snail.snailffmpeg.base.IExtractor
import java.nio.ByteBuffer


/**
 * 视频解码器类
 */
class VideoDecoder(path: String, private var sfv: SurfaceView?, private var surface: Surface?) :
    BaseDecoder(path) {

    private val TAG = "VideoDecoder"

    override fun check(): Boolean {
        if (sfv == null && surface == null) {
            Log.w(TAG, "SurfaceView和Surface都为空，至少需要一个不为空")
            mStateListener?.decoderError(this, "显示器为空")
            return false
        }
        return true
    }

    override fun initExtractor(path: String): IExtractor {
        return VideoExtractor(path)
    }

    override fun initSpecParams(format: MediaFormat) {
    }

    override fun configCodec(codec: MediaCodec, format: MediaFormat): Boolean {
        codec.configure(format, null, null, 0)
        return true
    }

    override fun initRender(): Boolean {
        return true
    }

    override fun render(
        outputBuffer: ByteBuffer,
        bufferInfo: MediaCodec.BufferInfo
    ) {
    }

    override fun doneDecode() {
    }

}