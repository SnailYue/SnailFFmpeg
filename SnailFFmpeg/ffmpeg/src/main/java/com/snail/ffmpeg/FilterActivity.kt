package com.snail.ffmpeg

import android.app.Activity
import android.content.Intent
import android.os.Environment
import android.view.SurfaceHolder
import com.snail.ffmpeg.base.BaseActivity
import com.snail.ffmpeg.transform.MediaTransFormUtil
import kotlinx.android.synthetic.main.activity_player.*
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch

class FilterActivity : BaseActivity(), SurfaceHolder.Callback {

    companion object {
        @JvmStatic
        fun start(act: Activity) {
            act.startActivity(Intent(act, FilterActivity::class.java))
        }
    }

    var inPath =
        Environment.getExternalStorageDirectory().absolutePath + "/langzhongzhilian.mp4"

    var surfaceHolder: SurfaceHolder? = null

    override var getView: Int = R.layout.activity_filter

    override fun initView() {
        surfaceHolder = sv?.holder
        surfaceHolder?.addCallback(this)
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        GlobalScope.launch {
            MediaTransFormUtil.startFilter(inPath, surfaceHolder?.surface!!)
        }
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
    }
}