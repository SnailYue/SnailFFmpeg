package com.snail.snailffmpeg.native

class KotlinToJNI {

    init {
        System.loadLibrary("native_lib")
    }

    companion object {
        external fun initFFmpeg()
    }

}