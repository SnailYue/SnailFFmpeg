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
 * 视频解码
 */
class VideoDecoder(path: String, sfv: SurfaceView?, surface: Surface?) : BaseDecoder(path) {

    private val mSurfaceView = sfv
    private var mSurface = surface

    override fun check(): Boolean {
        /**
         * 检查SurfaceView是否为空
         */
        if (mSurfaceView == null && mSurface == null) {
            mStateListener?.decoderError(this, "surfaceView为空")
            return false
        }
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
        if (mSurface != null) {
            codec.configure(format, mSurface, null, 0)
            notifyDecode()
        } else if (mSurfaceView?.holder?.surface != null) {
            mSurface = mSurfaceView?.holder?.surface
            configCodec(codec, format)
        } else {
            mSurfaceView?.holder?.addCallback(object : SurfaceHolder.Callback2 {
                override fun surfaceRedrawNeeded(holder: SurfaceHolder?) {
                }

                override fun surfaceChanged(
                    holder: SurfaceHolder?,
                    format: Int,
                    width: Int,
                    height: Int
                ) {
                }

                override fun surfaceDestroyed(holder: SurfaceHolder?) {
                }

                override fun surfaceCreated(holder: SurfaceHolder?) {
                    mSurface = holder?.surface
                    configCodec(codec, format)
                }

            })
            return false
        }
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

    override fun doneDecode() {
    }

    override fun getTrack(): Int {
        return (mExtractor as VideoExtractor).getVideoTrack()
    }

}