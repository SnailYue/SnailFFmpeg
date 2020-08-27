package com.snail.snailffmpeg.encoder

import com.snail.snailffmpeg.base.BaseEncoder
import com.snail.snailffmpeg.base.IEncodeStateListener

interface DeEncoderStateListener : IEncodeStateListener {
    override fun encodeStart(encoder: BaseEncoder) {
    }

    override fun encodeProgress(encoder: BaseEncoder) {
    }

    override fun encodeFinish(encoder: BaseEncoder) {
    }
}