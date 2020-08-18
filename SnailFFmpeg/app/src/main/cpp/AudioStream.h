//
// Created by surface on 2020/8/16.
//

#ifndef SNAILFFMPEG_AUDIOSTREAM_H
#define SNAILFFMPEG_AUDIOSTREAM_H

#include <sys/types.h>
#include "include/rtmp/rtmp.h"
#include "include/faac/faac.h"
#include "LogUtil.h"

class AudioStream {
    typedef void (*AudioCallback)(RTMPPacket *packet);

public:
    AudioStream();

    ~AudioStream();

    void setAudioEncInfo(int samplesInHz, int channels);

    void setAudioCallback(AudioCallback audioCallback);

    int getInputSamples();

    void encodeData(int8_t *data);

    RTMPPacket *getAudioTag();

private:
    AudioCallback audioCallback;
    int mChannels;
    faacEncHandle audioCodec = 0;
    u_long inputSamples;
    u_long maxOutputBytes;
    u_char *buffer = 0;
};


#endif //SNAILFFMPEG_AUDIOSTREAM_H
