package com.snail.snailffmpeg.extends

import android.view.View

/**
 * 简化View的VISIBLE方法
 */
fun View.show() {
    this.visibility = View.VISIBLE
}

/**
 * 简化View的INVISIBLE方法
 */
fun View.hide() {
    this.visibility = View.INVISIBLE
}

/**
 * 简化View的GONE方法
 */
fun View.gone() {
    this.visibility = View.GONE
}