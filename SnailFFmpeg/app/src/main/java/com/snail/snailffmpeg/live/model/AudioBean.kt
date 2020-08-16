package com.snail.snailffmpeg.live.model

data class AudioBean(
    val channelConfig: Int,
    val sampleRate: Int,
    val audioFormat: Int,
    val numChannels: Int
)