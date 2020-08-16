package com.snail.snailffmpeg.native

class NativePlayer {

    init {
        System.loadLibrary("liveFFmpeg")
    }

    companion object {

        /**
         * 推流
         */
        external fun startPush(url: String)

        /**
         * 停止推流
         */
        external fun stopPush()

        /**
         * 释放推流相关的内存空间
         */
        external fun release()


        /**
         * 设置视频相关的参数
         */
        external fun setVideoOptions(width: Int, height: Int, bitrate: Int, fps: Int)


        /**
         * 设置音频的参数
         */
        external fun setAudioOptions(sampleRateInHz: Int, channel: Int)

        /**
         * 发送视频数据
         */
        external fun sendVideoPacket(data: ByteArray)

        /**
         * 发送音频数据
         */
        external fun sendAudioPacket(data: ByteArray, len: Int)
    }

}