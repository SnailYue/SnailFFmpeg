package com.snail.snailffmpeg.base

interface IEncodeStateListener {
    fun encodeStart(encoder: BaseEncoder)

    fun encodeProgress(encoder: BaseEncoder)

    fun encodeFinish(encoder: BaseEncoder)
}