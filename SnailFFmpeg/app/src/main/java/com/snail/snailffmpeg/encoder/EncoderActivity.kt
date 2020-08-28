package com.snail.snailffmpeg.encoder


import android.app.Activity
import android.content.Intent
import android.os.Environment
import android.view.Surface
import com.snail.snailffmpeg.R
import com.snail.snailffmpeg.base.*
import com.snail.snailffmpeg.decoder.AudioDecoder
import com.snail.snailffmpeg.decoder.DeCodeStateListener
import com.snail.snailffmpeg.decoder.Frame
import com.snail.snailffmpeg.decoder.VideoDecoder
import kotlinx.android.synthetic.main.activity_encoder.*
import java.util.concurrent.Executors


/**
 * 视频硬编码
 */
class EncoderActivity : BaseActivity(), MMuxer.IMuxerStateListener {

    companion object {

        @JvmStatic
        fun start(activity: Activity) {
            activity.startActivity(Intent(activity, EncoderActivity::class.java))
        }
    }

    private var path = Environment.getExternalStorageDirectory().absolutePath + "/snailText.mp4"
    private lateinit var muxer: MMuxer
    private var audioDecoder: IDecoder? = null
    private var videoDecoder: IDecoder? = null
    private var videoEncoder: VideoEncoder? = null
    private var audioEncoder: AudioEncoder? = null
    private val threadPool = Executors.newFixedThreadPool(10)

    override var getView: Int = R.layout.activity_encoder

    override fun initView() {
        muxer = MMuxer("newSnailText.mp4")
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
        videoDecoder = VideoDecoder(path, sfv, sf)
        videoDecoder!!.setStateListener(object : DeCodeStateListener {
            override fun decoderOneFrame(decode: BaseDecoder?, frame: Frame) {
                videoEncoder?.encodeOneFrame(frame)
            }

            override fun decoderFinish(decode: BaseDecoder?) {
                videoEncoder?.endOfStream()
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
                audioEncoder?.encodeOneFrame(frame)
            }

            override fun decoderFinish(decode: BaseDecoder?) {
                audioEncoder?.endOfStream()
            }
        })
        audioDecoder?.resume()
        threadPool.execute(audioDecoder)
    }

    /**
     * 编码完成
     */
    override fun onMuxerFinish() {
        audioDecoder?.stop()
        audioDecoder = null

        videoDecoder?.stop()
        videoDecoder = null
    }

}