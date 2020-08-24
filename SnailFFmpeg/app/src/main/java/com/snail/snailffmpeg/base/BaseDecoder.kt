package com.snail.snailffmpeg.base

import android.media.MediaCodec
import android.media.MediaFormat
import com.snail.snailffmpeg.constant.DecodeState
import com.snail.snailffmpeg.decoder.Frame
import java.io.File
import java.nio.ByteBuffer

abstract class BaseDecoder : IDecoder {

    private var mIsRunning = false

    private var mLock = Object()

    private var mReadForDecode = false

    private var mCodec: MediaCodec? = null

    private var mExtractor: IExtractor? = null

    private var mInputBuffers: Array<ByteBuffer>? = null

    private var mOutputBuffers: Array<ByteBuffer>? = null

    private var mBufferInfo = MediaCodec.BufferInfo()

    private var mState = DecodeState.STOP

    protected var mStateListener: IDecodeStateListener? = null

    private var mIsEOS = false

    protected var mVideoWidth = 0

    protected var mVideoHeight = 0

    private var mDuration: Long = 0

    private var mEndPos: Long = 0

    private var mStartTimeForSync = -1L

    private var mSYncRender = true

    private var mFilePath: String? = null

    constructor(filePath: String) {
        mFilePath = filePath
    }

    final override fun run() {
        if (mState == DecodeState.STOP) {
            mState = DecodeState.START
        }
        mStateListener?.decoderPrepare(this)
        //初始化
        if (!init()) {
            return
        }
        while (mIsRunning) {
            if (mState != DecodeState.START && mState != DecodeState.DECODEING && mState != DecodeState.SEEKING) {
                waitDecode()
                //恢复同步的起始时间，即去除等待流失的时间
                mStartTimeForSync = System.currentTimeMillis() - getCurTimeStamp()
            }
            if (!mIsRunning || mState == DecodeState.STOP) {
                mIsRunning = false
                break
            }
            if (mStartTimeForSync == -1L) {
                mStartTimeForSync = System.currentTimeMillis()
            }
            // 如果数据没有解码完毕，将数据推入解码器解码
            if (!mIsEOS) {
                //将数据压入解码器输入缓冲
                mIsEOS = pushBufferToDecoder()
            }
            // 将解码器中好的数据冲缓冲区拉取出来
            val index = pullBufferFromDecoder()
            if (index >= 0) {
                //音视频同步
                if (mSYncRender && mState == DecodeState.DECODEING) {
                    sleepRender()
                }
                //渲染
                if (mSYncRender) {
                    render(mOutputBuffers!![index], mBufferInfo)
                }
                //将解码数据传出去
                val frame = Frame()
                frame.buffer = mOutputBuffers!![index]
                frame.setBufferInfo(mBufferInfo)
                mStateListener?.decoderOneFrame(this, frame)
                //释放输入缓冲
                mCodec!!.releaseOutputBuffer(index, true)
                if (mState == DecodeState.START) {
                    mState = DecodeState.PAUSE
                }
            }
            //判断解码是否完成
            if (mBufferInfo.flags == MediaCodec.BUFFER_FLAG_END_OF_STREAM) {
                mState = DecodeState.FINISH
                mStateListener?.decoderFinish(this)
            }
        }
        doneDecode()
        release()
    }

    /**
     * 判断是否初始化
     */
    private fun init(): Boolean {
        if (mFilePath!!.isEmpty() || !File(mFilePath).exists()) {
            mStateListener?.decoderError(this, "文件路径为空")
            return false
        }
        if (!check()) {
            return false
        }
        mExtractor = initExtractor(mFilePath!!)
        if (mExtractor == null || mExtractor!!.getFormat() == null) {
            return false
        }

        if (!initParams()) {
            return false
        }
        if (!initRender()) {
            return false
        }
        if (!initCodec()) {
            return false
        }
        return true
    }

    private fun initParams(): Boolean {
        val format = mExtractor!!.getFormat()!!
        mDuration = format.getLong(MediaFormat.KEY_DURATION) / 1000
        if (mEndPos == 0L) {
            mEndPos = mDuration
        }
        initSpecParams(mExtractor!!.getFormat()!!)
        return true
    }

    private fun initCodec(): Boolean {
        val type = mExtractor!!.getFormat()!!.getString(MediaFormat.KEY_MIME)
        mCodec = MediaCodec.createDecoderByType(type)
        if (!configCodec(mCodec!!, mExtractor!!.getFormat()!!)) {
            waitDecode()
        }
        mCodec!!.start()
        mInputBuffers = mCodec?.inputBuffers
        mOutputBuffers = mCodec?.outputBuffers
        return true
    }

    private fun pushBufferToDecoder(): Boolean {
        var inputBufferIndex = mCodec!!.dequeueInputBuffer(1000)
        var isEndOfStream = false

        if (inputBufferIndex >= 0) {
            val inputBuffer = mInputBuffers!![inputBufferIndex]
            val sampleSize = mExtractor!!.readBuffer(inputBuffer)
            if (sampleSize < 0) {
                mCodec!!.queueInputBuffer(
                    inputBufferIndex,
                    0,
                    0,
                    0,
                    MediaCodec.BUFFER_FLAG_END_OF_STREAM
                )
                isEndOfStream = true
            } else {
                mCodec!!.queueInputBuffer(
                    inputBufferIndex,
                    0,
                    sampleSize,
                    mExtractor!!.getCurrentTimeStamp(),
                    0
                )
            }
        }
        return isEndOfStream
    }

    private fun pullBufferFromDecoder(): Int {
        var index = mCodec!!.dequeueOutputBuffer(mBufferInfo, 1000)
        when (index) {
            MediaCodec.INFO_OUTPUT_FORMAT_CHANGED -> {

            }
            MediaCodec.INFO_TRY_AGAIN_LATER -> {

            }
            MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED -> {

            }
            else -> {
                return index
            }
        }
        return -1
    }

    private fun sleepRender() {
        val passTime = System.currentTimeMillis() - mStartTimeForSync
        val curTime = getCurTimeStamp()
        if (curTime > passTime) {
            Thread.sleep(curTime - passTime)
        }
    }

    private fun release() {
        mState = DecodeState.STOP
        mIsEOS = false
        mExtractor?.stop()
        mCodec?.stop()
        mCodec?.release()
        mStateListener?.decoderDestroy(this)
    }

    private fun waitDecode() {
        if (mState == DecodeState.PAUSE) {
            mStateListener?.decoderPause(this)
        }
        synchronized(mLock) {
            mLock.wait()
        }
    }

    protected fun notifyDecode() {
        synchronized(mLock) {
            mLock.notifyAll()
        }
        if (mState == DecodeState.DECODEING) {
            mStateListener?.decoderRunning(this)
        }
    }

    override fun pause() {
        mState = DecodeState.DECODEING
    }

    override fun seekTo(position: Long): Long {
        return 0
    }

    override fun resume() {
        mState = DecodeState.DECODEING
        notifyDecode()
    }

    override fun seekAndPlay(position: Long): Long {
        return 0
    }

    override fun stop() {
        mState = DecodeState.STOP
        mIsRunning = false
        notifyDecode()
    }

    override fun isDecoding(): Boolean {
        return mState == DecodeState.DECODEING
    }

    override fun isSeeking(): Boolean {
        return mState == DecodeState.SEEKING
    }

    override fun isStop(): Boolean {
        return mState == DecodeState.STOP
    }

    override fun setSizeListener(listener: IDecoderProgress) {

    }

    override fun setStateListener(listener: IDecodeStateListener?) {
        mStateListener = listener
    }

    override fun getWidth(): Int {
        return mVideoWidth
    }

    override fun getHeight(): Int {
        return mVideoHeight
    }

    override fun getCurTimeStamp(): Long {
        return mBufferInfo.presentationTimeUs / 1000
    }

    override fun getRotationAngle(): Int {
        return 0
    }

    override fun getMediaFormat(): MediaFormat? {
        return mExtractor?.getFormat()
    }

    override fun getFilePath(): String {
        return mFilePath!!
    }

    override fun withoutSync(): IDecoder {
        mSYncRender = false
        return this
    }


    abstract fun check(): Boolean

    abstract fun initExtractor(path: String): IExtractor

    abstract fun initSpecParams(format: MediaFormat)

    abstract fun configCodec(codec: MediaCodec, format: MediaFormat): Boolean

    abstract fun initRender(): Boolean

    abstract fun render(outputBuffer: ByteBuffer, bufferInfo: MediaCodec.BufferInfo)

    abstract fun doneDecode()
}