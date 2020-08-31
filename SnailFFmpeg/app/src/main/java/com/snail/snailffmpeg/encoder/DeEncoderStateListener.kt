package com.snail.snailffmpeg.encoder

import com.snail.snailffmpeg.base.BaseEncoder
import com.snail.snailffmpeg.base.IEncodeStateListener

interface DeEncoderStateListener : IEncodeStateListener {
    override fun encoderStart(encoder: BaseEncoder) {
    }

    override fun encoderProgress(encoder: BaseEncoder) {
    }

    override fun encoderFinish(encoder: BaseEncoder) {
    }
}