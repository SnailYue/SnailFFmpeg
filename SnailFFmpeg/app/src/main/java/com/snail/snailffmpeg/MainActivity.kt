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
        needCameraPermissionWithPermissionCheck()
    }

    override fun loadData() {

    }


    @NeedsPermission(
        Manifest.permission.CAMERA,
        Manifest.permission.RECORD_AUDIO,
        Manifest.permission.WRITE_EXTERNAL_STORAGE
    )
    fun needCameraPermission() {
        LiveActivity.start(this)
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