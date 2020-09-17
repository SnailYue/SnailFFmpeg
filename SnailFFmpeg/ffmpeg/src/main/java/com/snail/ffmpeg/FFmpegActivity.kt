package com.snail.ffmpeg

import android.app.Activity
import android.content.Intent
import android.os.Environment
import com.snail.ffmpeg.base.BaseActivity
import com.snail.ffmpeg.transform.MediaTransFormUtil
import kotlinx.android.synthetic.main.activity_ffmpeg.*

class FFmpegActivity : BaseActivity() {

    companion object {
        @JvmStatic
        fun start(act: Activity) {
            act.startActivity(Intent(act, FFmpegActivity::class.java))
        }
    }

    override var getView: Int = R.layout.activity_ffmpeg

    override fun initView() {
        var inPath =
            Environment.getExternalStorageDirectory().absolutePath + "/langzhongzhilian.mp4"
        var outPath =
            Environment.getExternalStorageDirectory().absolutePath + "/langzhongzhilian.avi"
        bt_transform?.setOnClickListener {
            MediaTransFormUtil.transformMedia(
                com.snail.ffmpeg.constant.MediaForm.MP4TOAVI,
                inPath,
                outPath
            )
            bt_transform?.isClickable = false
        }

        bt_screen_shot?.setOnClickListener {
            MediaTransFormUtil.screenshotFromStream(
                inPath,
                Environment.getExternalStorageDirectory().absolutePath + "/screenshot.jpg"
            )
        }

        bt_add_logo?.setOnClickListener {
            MediaTransFormUtil.addPictureLogo(
                inPath,
                Environment.getExternalStorageDirectory().absolutePath + "/youbo.png",
                Environment.getExternalStorageDirectory().absolutePath + "/addLogoVideo.mp4"
            )
        }

        bt_player?.setOnClickListener {
            PlayerActivity.start(this)
        }

        bt_filter?.setOnClickListener {
            FilterActivity.start(this)
        }
    }
}
