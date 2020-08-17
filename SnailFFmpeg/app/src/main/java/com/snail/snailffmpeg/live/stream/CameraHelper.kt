package com.snail.snailffmpeg.live.stream

import android.app.Activity
import android.graphics.ImageFormat
import android.hardware.Camera
import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder

/**
 * helper类，用于操作Camera
 */
class CameraHelper : SurfaceHolder.Callback, Camera.PreviewCallback {

    private var mActivity: Activity
    private var mHeight: Int = 0
    private var mWidth: Int = 0
    private var mCameraId: Int = 0
    private var mCamera: Camera? = null
    private lateinit var buffer: ByteArray
    private lateinit var mSurfaceHolder: SurfaceHolder
    private lateinit var mPreviewCallback: Camera.PreviewCallback
    private var mRotation: Int = 0
    private lateinit var bytes: ByteArray
    private lateinit var mOnChangedSizeListener: (width: Int, height: Int) -> Unit


    constructor(activity: Activity, cameraId: Int, width: Int, height: Int) {
        mActivity = activity
        mCameraId = cameraId
        mWidth = width
        mHeight = height
    }

    /**
     * 切换摄像头
     */
    fun switchCamera() {
        if (mCameraId == Camera.CameraInfo.CAMERA_FACING_BACK) {
            mCameraId = Camera.CameraInfo.CAMERA_FACING_FRONT
        } else {
            mCameraId = Camera.CameraInfo.CAMERA_FACING_BACK
        }
        stopPreview()
        startPreview()
    }

    /**
     * 停止预览
     */
    private fun stopPreview() {
        mCamera?.apply {
            setPreviewCallback(null)
            stopFaceDetection()
            release()
        }
    }

    /**
     * 开始预览
     */
    private fun startPreview() {
        mCamera = Camera.open(mCameraId)
        var parameters = mCamera?.parameters
        parameters?.previewFormat = ImageFormat.NV21
//        parameters?.focusMode = Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE
        setPreviewSize(parameters!!)
        setPreviewOrientation(parameters)
        buffer = ByteArray(mWidth * mHeight * 3 / 2)
        bytes = ByteArray(buffer.size)
        mCamera?.parameters = parameters
        mCamera?.addCallbackBuffer(buffer)
        mCamera?.setPreviewCallbackWithBuffer(this)
        mCamera?.setPreviewDisplay(mSurfaceHolder)
        mCamera?.startPreview()
//        mCamera?.cancelAutoFocus()
    }

    /**
     * 设置预览画面的角度
     */
    private fun setPreviewOrientation(paramters: Camera.Parameters) {
        var info = Camera.CameraInfo()
        Camera.getCameraInfo(mCameraId, info)
        mRotation = mActivity.windowManager?.defaultDisplay?.rotation!!
        var degress = 0
        when (mRotation) {
            Surface.ROTATION_0 -> {
                degress = 0
                mOnChangedSizeListener.invoke(mHeight, mWidth)
            }
            Surface.ROTATION_90 -> {
                degress = 90
                mOnChangedSizeListener.invoke(mHeight, mWidth)
            }
            Surface.ROTATION_180 -> {
                degress = 180
                mOnChangedSizeListener.invoke(mHeight, mWidth)
            }
            Surface.ROTATION_270 -> {
                degress = 270
                mOnChangedSizeListener.invoke(mHeight, mWidth)
            }
        }
        var result: Int = 0
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            result = (info.orientation + degress) % 360
            result = (360 - result) % 360
        } else {
            result = (info.orientation - degress + 360) % 360
        }
        mCamera?.setDisplayOrientation(result)
    }


    private fun setPreviewSize(parameters: Camera.Parameters) {
        var supportedPreviewSizes: MutableList<Camera.Size> = parameters.supportedPreviewSizes
        var size = supportedPreviewSizes.get(0)
        var m = Math.abs(size.height * size.width - mWidth * mHeight)
        supportedPreviewSizes.removeAt(0)
        supportedPreviewSizes.forEach {
            var n = Math.abs(it.height * it.width - mWidth * mHeight)
            if (n < m) {
                m = n
                size = it
            }
        }
        mWidth = size.width
        mHeight = size.height
        parameters.setPreviewSize(mWidth, mHeight)
    }

    /**
     * 设置预览
     */
    fun setPreviewDisplay(surfaceHolder: SurfaceHolder) {
        mSurfaceHolder = surfaceHolder
        mSurfaceHolder.addCallback(this)
    }

    /**
     * 设置预览回调
     */
    fun setPreviewCallback(previewCallback: Camera.PreviewCallback) {
        mPreviewCallback = previewCallback
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        Log.d("surfaceChanged", "width = " + width + ", height = " + height)
        stopPreview()
        startPreview()
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        stopPreview()
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
    }

    override fun onPreviewFrame(data: ByteArray?, camera: Camera?) {
        when (mRotation) {
            Surface.ROTATION_0 -> {
                rotation90(data)
            }
            Surface.ROTATION_90 -> {

            }
            Surface.ROTATION_180 -> {

            }
            Surface.ROTATION_270 -> {

            }
        }
    }

    /**
     * 旋转90度
     */
    private fun rotation90(data: ByteArray?) {
        var index = 0
        var ySize = mWidth * mHeight
        var uvHeight = mHeight / 2
        if (mCameraId == Camera.CameraInfo.CAMERA_FACING_BACK) {
            for (i in 0 until mWidth) {
                for (j in mHeight - 1 downTo 0) {
                    bytes[index++] = data?.get(mWidth * j + i)!!
                }
            }
            for (i in 0 until mWidth step 2) {
                for (j in uvHeight - 1 downTo 0) {
                    //v
                    bytes[index++] = data?.get(ySize + mWidth * j + i)!!
                    //u
                    bytes[index++] = data?.get(ySize + mWidth * j + i + 1)
                }
            }
        } else {
            for (i in 0 until mWidth) {
                var nPos: Int = mWidth - 1
                for (j in 0 until mHeight) {
                    bytes[index++] = data?.get(nPos - i)!!
                    nPos += mWidth
                }
            }
            //u v
            for (i in 0 until mWidth step 2) {
                var nPos: Int = ySize + mWidth - 1
                for (j in 0 until uvHeight) {
                    bytes[index++] = data?.get(nPos - i - 1)!!
                    bytes[index++] = data?.get(nPos - i)
                    nPos += mWidth
                }
            }
        }
    }

    /**
     * 设置监听
     */
    fun setOnChangedSizeListener(listener: (width: Int, height: Int) -> Unit) {
        mOnChangedSizeListener = listener
    }

    /**
     * 释放
     */
    fun release() {
        mSurfaceHolder.removeCallback(this)
        stopPreview()
    }
}