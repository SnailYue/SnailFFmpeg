package com.snail.snailffmpeg.base

import android.app.Application
import android.content.Context
import androidx.multidex.MultiDex

class BaseApplication : Application() {

    override fun attachBaseContext(base: Context?) {
        super.attachBaseContext(base)
        MultiDex.install(this)
    }
}