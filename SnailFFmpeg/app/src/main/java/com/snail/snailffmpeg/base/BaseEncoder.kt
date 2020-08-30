package com.snail.snailffmpeg.base

import android.media.MediaCodec
import android.media.MediaFormat
import android.nfc.Tag
import android.util.Log
import com.snail.snailffmpeg.decoder.Frame
import java.nio.ByteBuffer


/**
 * 基类编码器
 */
abstract class BaseEncoder(muxer: MMuxer, width: Int = -1, height: Int = -1) : Runnable {

    val TAG = BaseEncoder::class.java.simpleName

    protected var mWidth: Int = width

    protected var mHeight: Int = height

    /**
     * 合成器
     */
    private var mMuxer: MMuxer = muxer

    /**
     * 线程是否运行
     */
    private var mRunning = true

    /**
     * 编码帧序列
     */
    private var mFrames = mutableListOf<Frame>()

    /**
     * 编码器
     */
    private lateinit var mCodec: MediaCodec

    /**
     * 当前编码帧信息
     */
    private val mBufferInfo = MediaCodec.BufferInfo()

    /**
     * 编码输出缓冲区
     */
    private var mOutputBuffers: Array<ByteBuffer>? = null

    /**
     * 编码输入缓冲区
     */
    private var mInputBuffers: Array<ByteBuffer>? = null

    private var mLock = Object()

    /**
     * 是否编码结束
     */
    private var mIsEOS = false

    /**
     * 设置状态监听
     */
    private var mStateListener: IEncodeStateListener? = null

    init {
        initCodec()
    }

    /**
     * 初始化编码器
     */
    private fun initCodec() {
        mCodec = MediaCodec.createEncoderByType(encodeType())
        configEncoder(mCodec)
        mCodec.start()
        mOutputBuffers = mCodec.outputBuffers
        mInputBuffers = mCodec.inputBuffers
    }

    override fun run() {
        loopEncode()
        done()
    }

    /**
     * 循环编码
     */
    private fun loopEncode() {
        while (mRunning && !mIsEOS) {
            val empty = synchronized(mFrames) {
                mFrames.isEmpty()
            }
            if (empty) {
                justWait()
            } else {
                val frame = synchronized(mFrames) {
                    mFrames.removeAt(0)
                }
                if (encodeManually()) {
                    encode(frame)
                } else if (frame.buffer == null) {
                    //发送结束标志位
                    mCodec.signalEndOfInputStream()
                    mIsEOS = true
                }
            }
            drain()
        }
    }

    /**
     * 编码
     */
    private fun encode(frame: Frame) {
        val index = mCodec.dequeueInputBuffer(-1)
        if (index >= 0) {
            val inputBuffer = mInputBuffers!![index]
            inputBuffer.clear()
            if (frame.buffer != null) {
                inputBuffer.put(frame.buffer)
            }
            if (frame.buffer == null || frame.bufferInfo.size <= 0) {
                mCodec.queueInputBuffer(
                    index,
                    0,
                    0,
                    frame.bufferInfo.presentationTimeUs,
                    MediaCodec.BUFFER_FLAG_END_OF_STREAM
                )
            } else {
                frame.buffer?.flip()
                frame?.buffer?.mark()
                mCodec.queueInputBuffer(
                    index,
                    0,
                    frame.bufferInfo.size,
                    frame.bufferInfo.presentationTimeUs,
                    0
                )
            }
            frame.buffer?.clear()
        }
    }

    /**
     * 取出编码输出数据
     */
    private fun drain() {
        loop@ while (!mIsEOS) {
            val index = mCodec.dequeueOutputBuffer(mBufferInfo, 1000)
            when (index) {
                MediaCodec.INFO_TRY_AGAIN_LATER -> break@loop
                MediaCodec.INFO_OUTPUT_FORMAT_CHANGED -> {
                    addTrack(mMuxer, mCodec.outputFormat)
                }
                MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED -> {
                    mOutputBuffers = mCodec.outputBuffers
                }
                else -> {
                    if (mBufferInfo.flags == MediaCodec.BUFFER_FLAG_END_OF_STREAM) {
                        mIsEOS = true
                        mBufferInfo.set(0, 0, 0, mBufferInfo.flags)
                        Log.d(TAG, "编码结束")
                    }
                    if (mBufferInfo.flags == MediaCodec.BUFFER_FLAG_CODEC_CONFIG) {
                        mCodec.releaseOutputBuffer(index, false)
                        continue@loop
                    }
                    if (!mIsEOS) {
                        writeData(mMuxer, mOutputBuffers!![index], mBufferInfo)
                    }
                    mCodec.releaseOutputBuffer(index, false)
                }
            }
        }
    }

    /**
     * 结束编码，释放资源
     */
    private fun done() {
        release(mMuxer)
        mCodec.stop()
        mCodec.release()
        mRunning = false
        mStateListener?.encodeFinish(this)
    }

    /**
     * 进入等待
     */
    private fun justWait() {
        synchronized(mLock) {
            mLock.wait(1000)
        }
    }

    /**
     * 通知继续编码
     */
    private fun notifyGo() {
        synchronized(mLock) {
            mLock.notify()
        }
    }

    /**
     * 将帧数据压入队列，等待编码
     */
    fun encodeOneFrame(frame: Frame) {
        synchronized(mFrames) {
            mFrames.add(frame)
        }
        notifyGo()
        Thread.sleep(frameWaitTimeMs())
    }

    /**
     * 结束编码
     */
    fun endOfStream() {
        synchronized(mFrames) {
            val frame = Frame()
            frame.buffer = null
            mFrames.add(frame)
            notifyGo()
        }
    }

    /**
     * 设置状态监听
     */
    fun setStateListener(listener: IEncodeStateListener) {
        mStateListener = listener
    }

    abstract fun encodeType(): String

    abstract fun configEncoder(codec: MediaCodec)

    abstract fun addTrack(muxer: MMuxer, mediaFormat: MediaFormat)

    abstract fun writeData(muxer: MMuxer, byteBuffer: ByteBuffer, bufferInfo: MediaCodec.BufferInfo)

    abstract fun release(muxer: MMuxer)

    open fun frameWaitTimeMs() = 20L

    open fun encodeManually() = true
}