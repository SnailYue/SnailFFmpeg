package com.snail.snailffmpeg.encoder


import android.app.Activity
import android.content.Intent
import android.os.Environment
import android.util.Log
import android.view.Surface
import com.snail.snailffmpeg.R
import com.snail.snailffmpeg.base.*
import com.snail.snailffmpeg.decoder.AudioDecoder
import com.snail.snailffmpeg.decoder.DeCodeStateListener
import com.snail.snailffmpeg.decoder.Frame
import com.snail.snailffmpeg.decoder.VideoDecoder
import java.io.File
import java.util.concurrent.Executors


/**
 * 视频硬编码
 */
class EncoderActivity : BaseActivity(), MMuxer.IMuxerStateListener {
    val TAG = EncoderActivity::class.java.simpleName

    companion object {

        @JvmStatic
        fun start(activity: Activity) {
            activity.startActivity(Intent(activity, EncoderActivity::class.java))
        }
    }

    private var path =
        Environment.getExternalStorageDirectory().absolutePath + "/langzhongzhilian.mp4"
    private lateinit var muxer: MMuxer
    private var audioDecoder: IDecoder? = null
    private var videoDecoder: IDecoder? = null
    private var videoEncoder: VideoEncoder? = null
    private var audioEncoder: AudioEncoder? = null
    private val threadPool = Executors.newFixedThreadPool(10)

    override var getView: Int = R.layout.activity_encoder

    override fun initView() {
        Log.d(TAG, path)
        muxer = MMuxer("new_test.mp4")
        muxer.setStateListener(this)
        initAudioDecoder()
        initVideoDecoder(path, null)
        initAudioEncoder()
        initVideoEncoder()
    }

    /**
     * 初始化并启动视频编码器
     */
    private fun initVideoEncoder() {
        videoEncoder = VideoEncoder(muxer, 1920, 1080)
        videoEncoder?.setStateListener(object : DeEncoderStateListener {
            override fun encodeFinish(encoder: BaseEncoder) {
                super.encodeFinish(encoder)
            }
        })
        threadPool.execute(videoEncoder)
    }

    /**
     * 初始化并启动视频解码器
     */
    private fun initVideoDecoder(path: String, sf: Surface?) {
        videoDecoder?.stop()
        videoDecoder = VideoDecoder(path, null, null).withoutSync()
        videoDecoder!!.setStateListener(object : DeCodeStateListener {
            override fun decoderOneFrame(decode: BaseDecoder?, frame: Frame) {
                Log.d(TAG, "encodeOneFrame Video")
                videoEncoder?.encodeOneFrame(frame)
            }

            override fun decoderFinish(decode: BaseDecoder?) {
                videoEncoder?.endOfStream()
            }

            override fun decoderError(decode: BaseDecoder?, msg: String) {
                Log.d(TAG, "视频编码错误信息: " + msg)
            }
        })
        videoDecoder?.resume()
        threadPool.execute(videoDecoder)
    }

    /**
     * 初始化并启动音频编码器
     */
    private fun initAudioEncoder() {
        audioEncoder = AudioEncoder(muxer)
        threadPool.execute(audioEncoder)
    }

    /**
     * 初始化并启动音频解码器
     */
    private fun initAudioDecoder() {
        audioDecoder?.stop()
        audioDecoder = AudioDecoder(path).withoutSync()
        audioDecoder?.setStateListener(object : DeCodeStateListener {
            override fun decoderOneFrame(decode: BaseDecoder?, frame: Frame) {
                Log.d(TAG, "encodeOneFrame Audio")
                audioEncoder?.encodeOneFrame(frame)
            }

            override fun decoderFinish(decode: BaseDecoder?) {
                audioEncoder?.endOfStream()
            }

            override fun decoderError(decode: BaseDecoder?, msg: String) {
                Log.d(TAG, "音频编码错误信息: " + msg)
            }
        })
        audioDecoder?.resume()
        threadPool.execute(audioDecoder)
    }

    /**
     * 编码完成
     */
    override fun onMuxerFinish() {
        Log.d(TAG, "编码完成")
        audioDecoder?.stop()
        audioDecoder = null

        videoDecoder?.stop()
        videoDecoder = null
    }

}