package com.snail.snailffmpeg


import com.snail.snailffmpeg.base.BaseActivity
import com.snail.snailffmpeg.extends.hide
import com.snail.snailffmpeg.native.KotlinToJNI.Companion.initFFmpeg
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : BaseActivity() {

    companion object {
        val TAG = this::class.simpleName
    }

    override var getView: Int = R.layout.activity_main

    override fun initView() {
        tv_text.text = "This Is My First FFmpeg Project"
        tv_text.hide()
        loadData()
    }

    override fun loadData() {
        initFFmpeg()
    }
}