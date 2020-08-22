#pragma once

#include "WaveFile.h"
#include <string>

namespace ncl {
	namespace audio {
		namespace wav {

			WaveReadResult read(std::string path);

			WaveWriteResult write(std::string path, byte_t* data, int size, int numChannels, int sampleRate = DEFAULT_SAMPLE_RATE, int bitsPerSample = DEFAULT_BITS_PER_SAMPLE);

			WaveWriteResult write(std::string path, WaveFile file);
		}
	} 
}

#include "detail/wave_io.inl"