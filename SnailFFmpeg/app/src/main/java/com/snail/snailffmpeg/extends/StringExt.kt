package com.snail.snailffmpeg.extends


/**
 * 用于判断是够字符串是否为空
 */
fun String.isEmpty(): Boolean {
    return null == this || "".equals(this)
}