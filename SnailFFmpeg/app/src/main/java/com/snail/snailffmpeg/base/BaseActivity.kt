package com.snail.snailffmpeg.base

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.snail.snailffmpeg.utils.PLog

abstract class BaseActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(getView)
        initView()
        PLog.init(this::class.java.simpleName)
    }

    abstract var getView: Int

    abstract fun initView()

    open fun loadData() {}

}