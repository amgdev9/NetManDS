/**
 * @file WaveAudio.cpp
 * @brief WAV loader and player
 */

// Includes C/C++
#include <stdexcept>
#include <string.h>

// Own includes
#include "WaveAudio.h"

namespace NetMan {

/**
 * @brief Load a WAV audio file
 * @param path  WAV file path
 */
WaveAudio::WaveAudio(const std::string &path) {

	FILE* f = fopen(std::string("romfs:/snd/" + path + ".wav").c_str(), "rb");
	if(f == NULL) {
        throw std::runtime_error(path + " not found");
    }
	
	char signature[5];
	fread(signature, 1, 4, f);
    signature[4] = '\0';
	
	if(strcmp(signature, "RIFF") != 0) {
		fclose(f);
        throw std::runtime_error("Wrong WAV file: " + path);
	}
	
    u16 channels;
    u16 bitsPerSample;

	fseek(f, 40, SEEK_SET);
	fread(&dataSize, 4, 1, f);
	fseek(f, 22, SEEK_SET);
	fread(&channels, 2, 1, f);
	fseek(f, 24, SEEK_SET);
	fread(&sampleRate, 4, 1, f);
	fseek(f, 34, SEEK_SET);
	fread(&bitsPerSample, 2, 1, f);
	
	if(dataSize == 0 || (channels != 1 && channels != 2) || (bitsPerSample != 8 && bitsPerSample != 16)) {
		fclose(f);
        throw std::runtime_error("Corrupted WAV file: " + path);
	}

    try {
        sampleData = std::shared_ptr<u8>((u8*)linearAlloc(dataSize), [=](u8 *buffer) { linearFree(buffer); });
    } catch (const std::bad_alloc &e) {
        fclose(f);
        throw;
    }
	
	fseek(f, 44, SEEK_SET);
	fread(sampleData.get(), 1, dataSize, f);
	fclose(f);

	// Set audio format
	if(bitsPerSample == 8) {
		ndspFormat = (channels == 1) ? NDSP_FORMAT_MONO_PCM8 : NDSP_FORMAT_STEREO_PCM8;
	} else {
		ndspFormat = (channels == 1) ? NDSP_FORMAT_MONO_PCM16 : NDSP_FORMAT_STEREO_PCM16;
	}

    // Prepare audio buffer handler
    memset(&waveBuf, 0, sizeof(ndspWaveBuf));
	
	waveBuf.data_vaddr = sampleData.get();
	waveBuf.nsamples = dataSize / (bitsPerSample >> 3) / channels;
	waveBuf.looping = false;        // No looping
	waveBuf.status = NDSP_WBUF_FREE;
}

/**
 * @brief Play a WaveAudio
 * @param channel Channel to use
 */
void WaveAudio::play(int channel) {

    if(ndspChnIsPlaying(channel)) return;
    
    ndspChnReset(channel);
    ndspChnWaveBufClear(channel);
	ndspChnSetInterp(channel, NDSP_INTERP_POLYPHASE);
	ndspChnSetRate(channel, sampleRate);
	ndspChnSetFormat(channel, ndspFormat);
	
	DSP_FlushDataCache(sampleData.get(), dataSize);
	
    // Begin playback
	ndspChnWaveBufAdd(channel, &waveBuf);
}

/**
 * @brief Destructor for a WaveAudio
 */
WaveAudio::~WaveAudio() { }

}
