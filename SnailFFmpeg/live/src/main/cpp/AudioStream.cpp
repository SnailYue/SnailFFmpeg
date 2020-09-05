//
// Created by surface on 2020/8/16.
//

#include "AudioStream.h"
#include <cstring>

AudioStream::AudioStream() {
    LOGI("AudioStream");
}

AudioStream::~AudioStream() {
    LOGI("~AudioStream");
    if (audioCodec) {
        faacEncClose(audioCodec);
        audioCodec = 0;
    }
}

void AudioStream::setAudioCallback(AudioCallback audioCallback) {
    LOGI("setAudioCallback");
    this->audioCallback = audioCallback;
}

void AudioStream::setAudioEncInfo(int samplesInHz, int channels) {
    LOGI("setAudioEncInfo");
    mChannels = channels;
    audioCodec = faacEncOpen(static_cast<unsigned long>(samplesInHz),
                             static_cast<unsigned int>(channels),
                             &inputSamples,
                             &maxOutputBytes
    );
    faacEncConfigurationPtr config = faacEncGetCurrentConfiguration(audioCodec);
    config->mpegVersion = MPEG4;
    config->aacObjectType = LOW;
    //量化格式或采样深度
    config->inputFormat = FAAC_INPUT_16BIT;
    config->outputFormat = 0;
    faacEncSetConfiguration(audioCodec, config);
    buffer = new u_char[maxOutputBytes];
}

int AudioStream::getInputSamples() {
    LOGI("getInputSamples");
    return static_cast<int>(inputSamples);
}

/**
 *
 * @return
 */
RTMPPacket *AudioStream::getAudioTag() {
    LOGI("getAudioTag");
    u_char *buf;
    u_long len;
    faacEncGetDecoderSpecificInfo(audioCodec, &buf, &len);
    int bodySize = static_cast<int>(2 + len);
    RTMPPacket *packet = new RTMPPacket;
    RTMPPacket_Alloc(packet, bodySize);
    packet->m_body[0] = 0xAF;
    if (mChannels == 1) {
        packet->m_body[0] = 0xAE;
    }
    packet->m_body[1] = 0x00;

    memcpy(&packet->m_body[2], buf, len);
    packet->m_hasAbsTimestamp = 0;
    packet->m_nBodySize = bodySize;
    packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
    packet->m_nChannel = 0x11;
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    return packet;

}

/**
 * 编码数据
 * @param data
 */
void AudioStream::encodeData(int8_t *data) {
    LOGI("encodeData");
    int byteLen = faacEncEncode(audioCodec, reinterpret_cast<int32_t *>(data),
                                static_cast<unsigned int>(inputSamples),
                                buffer,
                                static_cast<unsigned int>(maxOutputBytes));
    if (byteLen > 0) {
        int bodySize = 2 + byteLen;
        RTMPPacket *packet = new RTMPPacket;
        RTMPPacket_Alloc(packet, bodySize);
        packet->m_body[0] = 0xAF;
        if (mChannels == 1) {
            packet->m_body[0] = 0xAE;
        }
        packet->m_body[1] = 0x01;
        memcpy(&packet->m_body[2], buffer, static_cast<size_t>(byteLen));

        packet->m_hasAbsTimestamp = 0;
        packet->m_nBodySize = bodySize;
        packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
        packet->m_nChannel = 0x11;
        packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
        audioCallback(packet);
    }
}