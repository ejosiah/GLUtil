#pragma once

#include <string>
#include <fstream>
#include <iostream>

namespace ncl {
	namespace audio {
		namespace wav {
			inline WaveReadResult read(std::string path) {
				std::ifstream fin{ path, std::ios_base::binary };

				if (!fin.good()) {
					return WaveReadResult{ "unable to open file path: " + path };
				}

				byte_t* data = new byte_t[WAVE_FILE_HEADER_SIZE];
				fin.read(data, WAVE_FILE_HEADER_SIZE); // TODO check if we should check for read error as well

				WaveMetadata meta = *reinterpret_cast<WaveMetadata*>(data);
				if (meta.header.ChunkId != RIFF && meta.header.ChunkId != RIFF_BE) {
					return WaveReadResult{ path + " is not a valid WAVE file format" };
				}

				delete[] data;

				data = new byte_t[meta.dataHeader.Subchunk2Size];

				fin.read(data, meta.dataHeader.Subchunk2Size); // TODO check if we should check for read error as well

				fin.close();

				return WaveReadResult{ std::make_tuple(meta, data) };

			}

			inline WaveWriteResult write(std::string path, byte_t* data, int size, int numChannels, int sampleRate, int bitsPerSample) {
				WaveMetadata meta;
				meta.header.ChunkId = RIFF;
				meta.header.ChunkSize = CHUNK_HEADER_SIZE + size;
				meta.header.Format = WAVE;

				meta.fmt.Subchunk1ID = fmt;
				meta.fmt.Subchunk1Size = 16;
				meta.fmt.AudioFormat = PCM;
				meta.fmt.NumChannels = numChannels;
				meta.fmt.SampleRate = sampleRate;
				meta.fmt.ByteRate = sampleRate * numChannels * (bitsPerSample / ONE_BYTE);
				meta.fmt.BlockAlign = numChannels * (bitsPerSample / ONE_BYTE);
				meta.fmt.BitsPerSample = bitsPerSample;

				meta.dataHeader.Subchunk2ID = DATA;
				meta.dataHeader.Subchunk2Size = size;

				return write(path, std::make_tuple(meta, data));
			}

			inline WaveWriteResult write(std::string path, WaveFile file) {

				std::ofstream fout;
				fout.open(path, std::ios_base::binary);

				if (!fout) {
					return WaveWriteResult(" unable to open file: " + path);
				}

				auto [meta, data] = file;

				char* header = reinterpret_cast<char*>(&meta);

				fout.write(header, WAVE_FILE_HEADER_SIZE);
				fout.write(data, meta.dataHeader.Subchunk2Size);

				fout.flush();
				fout.close();

				return {};
			}
		}
	}
}