package com.snail.snailffmpeg.base

interface IEncodeStateListener {
    fun encoderStart(encoder: BaseEncoder)

    fun encoderProgress(encoder: BaseEncoder)

    fun encoderFinish(encoder: BaseEncoder)
}