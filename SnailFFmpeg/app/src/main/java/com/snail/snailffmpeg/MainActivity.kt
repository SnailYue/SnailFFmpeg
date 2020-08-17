package com.snail.snailffmpeg


import android.Manifest
import com.snail.snailffmpeg.base.BaseActivity
import com.snail.snailffmpeg.extends.hide
import com.snail.snailffmpeg.live.LiveActivity
import kotlinx.android.synthetic.main.activity_main.*
import permissions.dispatcher.NeedsPermission
import permissions.dispatcher.RuntimePermissions

@RuntimePermissions
class MainActivity : BaseActivity() {

    companion object {
        val TAG = this::class.simpleName
    }

    override var getView: Int = R.layout.activity_main

    override fun initView() {
        tv_text.text = "This Is My First FFmpeg Project"
        tv_text.hide()
        needCameraPermission()
    }

    override fun loadData() {

    }


    @NeedsPermission(Manifest.permission.CAMERA)
    fun needCameraPermission() {
        LiveActivity.start(this)
    }


    @NeedsPermission(Manifest.permission.CAMERA)
    fun deniedPermission() {
        System.exit(0)
    }

}