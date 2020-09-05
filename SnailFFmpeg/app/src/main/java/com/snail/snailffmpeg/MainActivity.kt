package com.snail.snailffmpeg


import android.Manifest
import com.snail.ffmpeg.FFmpegActivity
import com.snail.live.LiveActivity
import com.snail.snailffmpeg.base.BaseActivity
import com.snail.snailffmpeg.encoder.EncoderActivity
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
        needCameraPermissionWithPermissionCheck()
    }

    @NeedsPermission(
        Manifest.permission.CAMERA,
        Manifest.permission.RECORD_AUDIO,
        Manifest.permission.WRITE_EXTERNAL_STORAGE
    )
    fun needCameraPermission() {
        clickEnvent()
    }


    fun clickEnvent() {
        bt_live.setOnClickListener {
            LiveActivity.start(this)
        }
        bt_mediacodec.setOnClickListener {
            EncoderActivity.start(this)
        }
        bt_transform.setOnClickListener {
            FFmpegActivity.start(this)
        }
    }

    @NeedsPermission(
        Manifest.permission.CAMERA,
        Manifest.permission.RECORD_AUDIO,
        Manifest.permission.WRITE_EXTERNAL_STORAGE
    )
    fun deniedPermission() {
        System.exit(0)
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        onRequestPermissionsResult(requestCode, grantResults)
    }
}