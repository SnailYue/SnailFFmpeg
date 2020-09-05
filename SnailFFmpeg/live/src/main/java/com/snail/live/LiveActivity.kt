package com.snail.live

import android.app.Activity
import android.content.Intent
import android.hardware.Camera
import android.media.AudioFormat
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.snail.live.base.BaseActivity
import kotlinx.android.synthetic.main.activity_live.*

class LiveActivity : BaseActivity() {

    companion object {

        @JvmStatic
        fun start(activity: Activity) {
            activity.startActivity(Intent(activity, LiveActivity::class.java))
        }
    }

    var url = ""
    lateinit var livePusher: LivePusher

    override var getView = R.layout.activity_live

    override fun initView() {

        bt_start_live.setOnClickListener {
            livePusher?.startPush(url)
        }
        bt_stop_live.setOnClickListener {
            livePusher?.stopPush()
        }
        bt_switch.setOnClickListener {
            livePusher?.switchCamera()
        }
        initPusher()
    }

    /**
     * 初始化
     */
    private fun initPusher() {
        var width = 640
        var height = 480
        var videoBitRate = 800_000
        var videoFrameRate = 20
        var videoBean = com.snail.live.model.VideoBean(
            width,
            height,
            Camera.CameraInfo.CAMERA_FACING_BACK,
            videoBitRate,
            videoFrameRate
        )
        var sampleRate = 44100
        var channelConfig = AudioFormat.CHANNEL_IN_STEREO
        var audioFormat = AudioFormat.ENCODING_PCM_16BIT
        var numChannels = 2
        var audioBean =
            com.snail.live.model.AudioBean(channelConfig, sampleRate, audioFormat, numChannels)
        livePusher = com.snail.live.LivePusher(this, videoBean, audioBean)
        livePusher.setPreviewDisplay(sv_video.holder)
    }
}
