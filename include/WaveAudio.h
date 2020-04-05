/**
 * @file WaveAudio.h
 * @brief WAV loader and player
 */

#ifndef _WAVE_AUDIO_H_
#define _WAVE_AUDIO_H_

// Includes C/C++
#include <string>
#include <memory>

// Includes 3DS
#include <3ds.h>

namespace NetMan {

/**
 * @class WaveAudio
 */
class WaveAudio {
    private:
        std::shared_ptr<u8> sampleData;
        u32 sampleRate;
        u32 dataSize;
        u16 ndspFormat;
        ndspWaveBuf waveBuf;
    public:
        WaveAudio(const std::string &path);
        void play(int channel);
        virtual ~WaveAudio();
};

}

#endif
