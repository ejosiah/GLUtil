#pragma once

#include <iostream>
#include <cstdint>
#include <variant>
#include <tuple>
#include <string>
#include <optional>

namespace ncl {
	namespace audio {
		namespace wav {

			constexpr int DEFAULT_SAMPLE_RATE = 44100;
			constexpr int DEFAULT_BITS_PER_SAMPLE = 16;

			using byte_t = char;
			const int32_t RIFF_BE = 0x52494646;
			const int32_t RIFF = 0x46464952;
			const int32_t WAVE = 0x45564157;
			const int32_t fmt = 0x20746D66;
			const int32_t DATA = 0x61746164;
			const int16_t PCM = 0x1;
			const int ONE_BYTE = 8;

			const int HEADER_SIZE = 12;
			const int FMT_SUB_CHUNK_SIZE = 24;
			const int DATA_SUB_CHUNK_SIZE = 8;

			const int WAVE_FILE_HEADER_SIZE = 44;

			const int CHUNK_HEADER_SIZE = 36;

			struct Header {
				int32_t ChunkId = RIFF;
				int32_t ChunkSize;
				int32_t Format = WAVE;
			};

			struct Fmt {
				int32_t Subchunk1ID = fmt;
				int32_t Subchunk1Size = 16;
				int16_t AudioFormat = PCM;
				int16_t NumChannels;
				int32_t SampleRate;
				int32_t ByteRate;
				int16_t BlockAlign;
				int16_t BitsPerSample;
			};

			struct DataHeader {
				int32_t Subchunk2ID = DATA;
				int32_t Subchunk2Size;
			};

			struct WaveMetadata {
				Header header;
				Fmt fmt;
				DataHeader dataHeader;
			};

			using WaveFile = std::tuple<WaveMetadata, byte_t*>;
			using WaveError = std::string;
			using WaveReadResult = std::variant<WaveError, WaveFile>;
			using WaveWriteResult = std::optional<WaveError>;

			inline std::ostream& operator<<(std::ostream& out, const WaveMetadata& meta) {
				out << "Wave File Header:\n";
				out << "\tChunkID: " << meta.header.ChunkId << "\n";
				out << "\tChunkSize: " << meta.header.ChunkSize << "\n";
				out << "\tFormat: " << meta.header.Format << "\n";
				
				out << "fmt SubChunk: \n";
				out << "\tSubchunk1ID: " << meta.fmt.Subchunk1ID << "\n";
				out << "\tSubchunk1Size: " << meta.fmt.Subchunk1Size << "\n";
				out << "\tAudioFormat: " << meta.fmt.AudioFormat << "\n";
				out << "\tNumChannels: " << meta.fmt.NumChannels << "\n";
				out << "\tSampleRate: " << meta.fmt.SampleRate << "\n";
				out << "\tByteRate: " << meta.fmt.ByteRate << "\n";
				out << "\tBlockAlign: " << meta.fmt.BlockAlign << "\n";
				out << "\tBitsPerSample: " << meta.fmt.BitsPerSample << "\n";

				out << "data SubChunk: \n";
				out << "\tSubchunk2ID: " << meta.dataHeader.Subchunk2ID << "\n";
				out << "\tSubchunk2Size: " << meta.dataHeader.Subchunk2Size << "\n";

				return out;
			}

		}
	}
}