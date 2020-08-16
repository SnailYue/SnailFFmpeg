package com.snail.snailffmpeg.live

import android.app.Activity
import android.view.SurfaceHolder
import com.snail.snailffmpeg.live.model.AudioBean
import com.snail.snailffmpeg.live.model.VideoBean
import com.snail.snailffmpeg.live.stream.AudioStream
import com.snail.snailffmpeg.live.stream.VideoStream

class LivePusher {

    init {
        System.loadLibrary("live")
    }

    var audioStream: AudioStream
    var videoStream: VideoStream
    var activity: Activity? = null

    constructor(activity: Activity, videoBean: VideoBean, audioBean: AudioBean) {
        this.activity = activity
        native_init()
        videoStream = VideoStream(
            this,
            activity,
            videoBean.width,
            videoBean.height,
            videoBean.bitRate,
            videoBean.frameRate,
            videoBean.cameraId
        )
        audioStream = AudioStream(this, audioBean)
    }

    /**
     * 设置预览
     */
    fun setPreviewDisplay(surfaceHolder: SurfaceHolder) {
        videoStream.setPreviewDisplay(surfaceHolder)
    }


    /**
     * 切换摄像头
     */
    fun switchCamera() {
        videoStream.switchCamera()
    }

    /**
     * 设置是否静音
     */
    fun setMute(isMute: Boolean) {
        audioStream.isMute = isMute
    }

    fun startPush(path: String) {
        native_start(path)
        videoStream.startLive()
        audioStream.startLive()
    }

    fun stopPush() {
        videoStream.stopLive()
        audioStream.stopLive()
        native_stop()
    }

    fun release() {
        videoStream.release()
        audioStream.release()
        native_release()
    }

    fun setVideoCodecInfo(width: Int, height: Int, fps: Int, bitrate: Int) {
        native_setVideoCodecInfo(width, height, fps, bitrate)
    }

    fun setAudioCodecInfo(sampleRate: Int, channels: Int) {
        native_setAudioCodecInfo(sampleRate, channels)
    }

    fun start(path: String) {
        native_start(path)
    }

    fun getInputSample(): Int {
        return getInputSamples()
    }

    fun pushAudio(data: ByteArray) {
        native_pushAudio(data)
    }

    fun pushVideo(data: ByteArray) {
        native_pushVideo(data)
    }


    external fun native_init()

    external fun native_start(path: String)

    external fun native_setVideoCodecInfo(width: Int, height: Int, fps: Int, bitrate: Int)

    external fun native_setAudioCodecInfo(sampleRate: Int, channels: Int)

    external fun getInputSamples(): Int

    external fun native_pushAudio(data: ByteArray)

    external fun native_pushVideo(data: ByteArray)

    external fun native_stop()

    external fun native_release()

}