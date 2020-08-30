package com.snail.snailffmpeg.decoder

import android.media.MediaCodec
import android.media.MediaFormat
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import com.snail.snailffmpeg.base.BaseDecoder
import com.snail.snailffmpeg.base.IExtractor
import java.nio.ByteBuffer


/**
 * 视频解码器类
 */
class VideoDecoder(path: String, sfv: SurfaceView?, surface: Surface?) : BaseDecoder(path) {

    override fun check(): Boolean {
        return true
    }

    /**
     * 初始化视频数据提取器
     */
    override fun initExtractor(path: String): IExtractor {
        return VideoExtractor(path)
    }

    /**
     * 初始化特殊参数
     */
    override fun initSpecParams(format: MediaFormat) {

    }

    /**
     * 配置视频解码器参数
     */
    override fun configCodec(codec: MediaCodec, format: MediaFormat): Boolean {
        codec.configure(format, null, null, 0)
        return true
    }

    /**
     * 初始化渲染
     */
    override fun initRender(): Boolean {
        return true
    }

    /**
     * 渲染器
     */
    override fun render(outputBuffer: ByteBuffer, bufferInfo: MediaCodec.BufferInfo) {

    }

    /**
     * 完成解码
     */
    override fun doneDecode() {
    }

    /**
     * 获取通道索引
     */
    override fun getTrack(): Int {
        return (mExtractor as VideoExtractor).getVideoTrack()
    }

}