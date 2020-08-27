package com.snail.snailffmpeg.base

import com.snail.snailffmpeg.decoder.Frame

interface IDecodeStateListener {

    fun decoderPrepare(decode: BaseDecoder?)

    fun decoderReady(decode: BaseDecoder?)

    fun decoderRunning(decode: BaseDecoder?)

    fun decoderPause(decode: BaseDecoder?)

    fun decoderOneFrame(decode: BaseDecoder?, frame: Frame)

    fun decoderFinish(decode: BaseDecoder?)

    fun decoderDestroy(decode: BaseDecoder?)

    fun decoderError(decode: BaseDecoder?, msg: String)
}