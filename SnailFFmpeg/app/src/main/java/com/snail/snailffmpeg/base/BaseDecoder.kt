package com.snail.snailffmpeg.base

import android.media.MediaCodec
import android.media.MediaFormat
import android.util.Log
import com.snail.snailffmpeg.constant.DecodeState
import com.snail.snailffmpeg.decoder.Frame
import java.io.File
import java.lang.Exception
import java.nio.ByteBuffer

/**
 * 解码器基类
 */
abstract class BaseDecoder(val mFilePath: String) : IDecoder {

    val TAG = BaseDecoder::class.java.simpleName

    /**
     * 是否正在运行
     */
    private var mIsRunning = true

    /**
     * 线程等待
     */
    private var mLock = Object()

    /**
     * 是否可以进入解码器
     */
    private var mReadyForDecode = false

    /**
     * 音视频解码器
     */
    private var mCodec: MediaCodec? = null

    /**
     * 音视频通道数据提取器
     */
    protected var mExtractor: IExtractor? = null

    /**
     * 解码输入缓冲区
     */
    private var mInputBuffers: Array<ByteBuffer>? = null

    /**
     * 解码输出缓冲区
     */
    private var mOutputBuffers: Array<ByteBuffer>? = null

    /**
     * 解码数据信息
     */
    private var mBufferInfo = MediaCodec.BufferInfo()

    private var mState = DecodeState.STOP

    protected var mStateListener: IDecodeStateListener? = null

    /**
     * 数据流是否结束
     */
    private var mIsEOS = false

    protected var mVideoWidth = 0

    protected var mVideoHeight = 0

    private var mDuration: Long = 0

    private var mEndPos: Long = 0

    /**
     * 开始解码时间，用于音视频同步
     */
    private var mStartTimeForSync = -1L

    /**
     * 是否需要音视频同步
     */
    private var mSyncRender = true

    final override fun run() {
        if (mState == DecodeState.STOP) {
            mState = DecodeState.START
        }
        mStateListener?.decoderPrepare(this)
        //初始化
        if (!init()) {
            return
        }
        Log.d(TAG, "开始解码")
        try {

            while (mIsRunning) {
                Log.d(TAG, "解码中...")
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
                    if (mSyncRender && mState == DecodeState.DECODEING) {
                        sleepRender()
                    }
                    //渲染
                    if (mSyncRender) {
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
                    Log.d(TAG, "解码结束")
                    mState = DecodeState.FINISH
                    mStateListener?.decoderFinish(this)
                }
            }
        } catch (e: Exception) {
            e.printStackTrace()
        } finally {
            doneDecode()
            release()
        }
    }

    /**
     * 判断是否初始化
     */
    private fun init(): Boolean {
        /**
         * 判断路径是否正确
         */
        if (mFilePath.isEmpty() || !File(mFilePath).exists()) {
            mStateListener?.decoderError(this, "file path null")
            return false
        }
        if (!check()) {
            mStateListener?.decoderError(this, "check false")
            return false
        }
        /**
         * 获取音视频通道数据提取器
         */
        mExtractor = initExtractor(mFilePath)

        if (mExtractor == null || mExtractor!!.getFormat() == null) {
            mStateListener?.decoderError(this, "mExtractor null")
            return false
        }

        if (!initParams()) {
            mStateListener?.decoderError(this, "initParams false")
            return false
        }
        if (!initRender()) {
            mStateListener?.decoderError(this, "initParams false")
            return false
        }
        if (!initCodec()) {
            mStateListener?.decoderError(this, "initCodec false")
            return false
        }
        return true
    }

    private fun initParams(): Boolean {
        try {
            val format = mExtractor!!.getFormat()!!
            mDuration = format.getLong(MediaFormat.KEY_DURATION) / 1000
            if (mEndPos == 0L) {
                mEndPos = mDuration
            }
            initSpecParams(mExtractor!!.getFormat()!!)
        } catch (e: Exception) {
            return false
        }
        return true
    }

    /**
     * 设置MediaCodec的MediaFormat信息
     */
    private fun initCodec(): Boolean {
        try {
            val type = mExtractor!!.getFormat()!!.getString(MediaFormat.KEY_MIME)
            mCodec = MediaCodec.createDecoderByType(type)
            if (!configCodec(mCodec!!, mExtractor!!.getFormat()!!)) {
                Log.d(TAG, "MediaFormat configCodec false")
                waitDecode()
            }
            mCodec!!.start()
            mInputBuffers = mCodec?.inputBuffers
            mOutputBuffers = mCodec?.outputBuffers
        } catch (e: Exception) {
            return false
        }
        return true
    }

    /**
     * 从输入缓冲区队列中取出可用缓冲区，并填充数据
     */
    private fun pushBufferToDecoder(): Boolean {
        var inputBufferIndex = mCodec!!.dequeueInputBuffer(1000)
        var isEndOfStream = false

        if (inputBufferIndex >= 0) {
            val inputBuffer = mInputBuffers!![inputBufferIndex]
            val sampleSize = mExtractor!!.readBuffer(inputBuffer)
            if (sampleSize < 0) {
                /**
                 * 进入EOS状态
                 */
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

    /**
     * 从输出缓冲区队列中拿到编解码后的内容
     */
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

    /**
     * 通过线程沉睡来实现同步
     */
    private fun sleepRender() {
        val passTime = System.currentTimeMillis() - mStartTimeForSync
        val curTime = getCurTimeStamp()
        if (curTime > passTime) {
            Thread.sleep(curTime - passTime)
        }
    }

    private fun release() {
        Log.d(TAG, "停止解码")
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

    override fun getDuration(): Long {
        return mDuration
    }

    override fun getMediaFormat(): MediaFormat? {
        return mExtractor?.getFormat()
    }


    override fun getFilePath(): String {
        return mFilePath
    }

    override fun withoutSync(): IDecoder {
        mSyncRender = false
        return this
    }


    /**
     * 检查子类参数
     */
    abstract fun check(): Boolean

    /**
     * 初始化数据提取器
     */
    abstract fun initExtractor(path: String): IExtractor

    /**
     * 初始化子类特有的参数
     */
    abstract fun initSpecParams(format: MediaFormat)

    /**
     * 配置解码器
     */
    abstract fun configCodec(codec: MediaCodec, format: MediaFormat): Boolean

    /**
     * 初始化渲染器
     */
    abstract fun initRender(): Boolean

    /**
     * 渲染
     */
    abstract fun render(outputBuffer: ByteBuffer, bufferInfo: MediaCodec.BufferInfo)

    /**
     * 结束解码
     */
    abstract fun doneDecode()
}