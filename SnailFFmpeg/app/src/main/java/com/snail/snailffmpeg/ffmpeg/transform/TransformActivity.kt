package com.snail.snailffmpeg.ffmpeg.transform

import android.app.Activity
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import com.snail.snailffmpeg.R
import com.snail.snailffmpeg.base.BaseActivity
import com.snail.snailffmpeg.constant.MediaForm
import kotlinx.android.synthetic.main.activity_transform.*

class TransformActivity : BaseActivity() {

    companion object {
        @JvmStatic
        fun start(act: Activity) {
            act.startActivity(Intent(act, TransformActivity::class.java))
        }
    }

    override var getView: Int = R.layout.activity_transform

    override fun initView() {
        var inPath =
            Environment.getExternalStorageDirectory().absolutePath + "/langzhongzhilian.mp4"
        var outPath =
            Environment.getExternalStorageDirectory().absolutePath + "/langzhongzhilian.avi"
        bt_transform?.setOnClickListener {
            MediaTransFormUtil.transformMedia(MediaForm.MP4TOAVI, inPath, outPath)
            bt_transform?.isClickable = false
        }
    }
}