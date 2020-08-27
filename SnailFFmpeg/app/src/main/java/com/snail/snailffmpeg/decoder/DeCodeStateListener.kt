package com.snail.snailffmpeg.decoder

import com.snail.snailffmpeg.base.BaseDecoder
import com.snail.snailffmpeg.base.IDecodeStateListener

interface DeCodeStateListener : IDecodeStateListener {
    override fun decoderDestroy(decode: BaseDecoder?) {
    }

    override fun decoderError(decode: BaseDecoder?, msg: String) {
    }

    override fun decoderFinish(decode: BaseDecoder?) {
    }

    override fun decoderOneFrame(decode: BaseDecoder?, frame: Frame) {
    }

    override fun decoderPause(decode: BaseDecoder?) {
    }

    override fun decoderPrepare(decode: BaseDecoder?) {
    }

    override fun decoderReady(decode: BaseDecoder?) {
    }

    override fun decoderRunning(decode: BaseDecoder?) {
    }
}