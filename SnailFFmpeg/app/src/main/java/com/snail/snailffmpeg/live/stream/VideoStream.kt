package com.snail.snailffmpeg.live.stream

import android.app.Activity
import android.hardware.Camera
import android.util.Log
import android.view.SurfaceHolder
import com.snail.snailffmpeg.live.LivePusher
import com.snail.snailffmpeg.utils.PLog


/**
 * 视频流
 */
class VideoStream : Camera.PreviewCallback {
    private var mLivePusher: LivePusher
    private lateinit var mCameraHelper: CameraHelper
    private var mBitRate: Int = 0
    private var mFps: Int = 0
    private var isLiving: Boolean = false

    constructor(
        livePusher: LivePusher,
        activity: Activity,
        width: Int,
        height: Int,
        bitrate: Int,
        fps: Int,
        cameraId: Int
    ) {
        mLivePusher = livePusher
        mBitRate = bitrate
        mFps = fps
        mCameraHelper = CameraHelper(activity, cameraId, width, height)
        mCameraHelper.setPreviewCallback(this)
        mCameraHelper.setOnChangedSizeListener { width, height ->
            mLivePusher.setVideoCodecInfo(width, height, mFps, mBitRate)
        }
    }

    /**
     * 设置预览
     */
    fun setPreviewDisplay(surfaceHolder: SurfaceHolder) {
        mCameraHelper.setPreviewDisplay(surfaceHolder)
    }

    /**
     * 切换摄像头
     */
    fun switchCamera() {
        mCameraHelper.switchCamera()
    }

    /**
     * 开始直播
     */
    fun startLive() {
        isLiving = true
    }

    /**
     * 停止直播
     */
    fun stopLive() {
        isLiving = false
    }

    /**
     * 释放
     */
    fun release() {
        mCameraHelper.release()
    }

    override fun onPreviewFrame(data: ByteArray, camera: Camera?) {
        PLog.d("onPreviewFrame")
        if (isLiving) {
            mLivePusher.pushVideo(data)
        }
    }
}