#include <iostream>
#include <fstream>
#include <portaudio/portaudio.h>
#include <atomic>
#include "../GLUtil/include/ncl/audio/wav/wave_io.h"

#pragma comment(lib, "portaudio_x86.lib")

using namespace std;
using namespace ncl::audio::wav;

struct SoundData {
	int numSamples;
	int samplesRead = 0;
	int numChannels;
	short* data;
};

static int patestCallback(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void* userData)
{
	
	short* out = (short*)outputBuffer;
	
	SoundData* data = (SoundData*)userData;

	(void)timeInfo; /* Prevent unused variable warnings. */
	(void)statusFlags;
	(void)inputBuffer;

	for (int i = 0; i < framesPerBuffer; i++)
	{
		if (data->samplesRead >= data->numSamples) return paComplete;
		for (int j = 0; j < data->numChannels; j++) {
			*out++ = *data->data++;
		//	*out++ = *data->data++;
		}
		data->samplesRead++;
	}

	return paContinue;
}

struct WaveVisitor {

	inline int operator()(std::string error) {
		cout << error << endl;
		return 500;
	}

	inline int operator()(WaveFile file) {
		auto [wave, data] = file;


		cout << "Num Channels: " << wave.fmt.NumChannels << "\n";
		cout << "SampleRate: " << wave.fmt.SampleRate << "\n";
		cout << "ByteRate: " << wave.fmt.ByteRate << "\n";
		cout << "Block Align: " << wave.fmt.BlockAlign << "\n";
		cout << "BitsPerSample: " << wave.fmt.BitsPerSample << "\n";

		int numSamples = wave.dataHeader.Subchunk2Size / (wave.fmt.NumChannels * (wave.fmt.BitsPerSample / 8));
		cout << "numSamples: " << numSamples << endl;

		PaStreamParameters outputParameters;
		PaStream* stream;
		PaError err;

		err = Pa_Initialize();
		if (err != paNoError) {
			cout << "unable to initialize portAudio\n";
			return 1;
		}

		outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
		if (outputParameters.device == paNoDevice) {
			fprintf(stderr, "Error: No default output device.\n");
			return 1;
		}

		outputParameters.channelCount = wave.fmt.NumChannels;
		outputParameters.sampleFormat = paInt16;
		outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
		outputParameters.hostApiSpecificStreamInfo = NULL;

		SoundData sound;
		sound.data = reinterpret_cast<short*>(data);
		sound.numSamples = numSamples;
		sound.numChannels = wave.fmt.NumChannels;

		err = Pa_OpenStream(
			&stream,
			NULL, /* no input */
			&outputParameters,
			wave.fmt.SampleRate,
			64,
			paClipOff,      /* we won't output out of range samples so don't bother clipping them */
			patestCallback,
			&sound);


		err = Pa_StartStream(stream);

		int NUM_SECONDS = numSamples / wave.fmt.SampleRate + 5;
		printf("Play for %d seconds.\n", NUM_SECONDS);
		Pa_Sleep(NUM_SECONDS * 1000);

		err = Pa_StopStream(stream);

		err = Pa_CloseStream(stream);

		Pa_Terminate();
		printf("Test finished.\n");
		return 0;
	}
};

struct CopyVisitor {

	int operator()(std::string error) {
		cout << error;
		return 1;
	}

	int operator()(WaveFile file) {
		auto [meta, data] = file;

		string path = "sound/voice_copy2.wav";
		int size = meta.dataHeader.Subchunk2Size;
		int numChannels = meta.fmt.NumChannels;
		int sampleRate = meta.fmt.SampleRate;
		int bitsPerSample = meta.fmt.BitsPerSample;

		auto error = write(path, data, size, numChannels, sampleRate, bitsPerSample);

		if (error) {
			cout << "unable to copy file, reason: " + *error << endl;
			return 1;
		}
		cout << "copied to " + path;
		return 0;
	}
};

struct stuff_data {
	float* data;
	int read = 0;
	int limit;
	int numChannels = 1;
	std::atomic_bool ready = false;
};

static int stuff_playback(const void* inputBuffer, void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void* userData)
{

	float* out = (float*)outputBuffer;

	stuff_data* data = (stuff_data*)userData;
	if (!data->ready) return paContinue;

	(void)timeInfo; /* Prevent unused variable warnings. */
	(void)statusFlags;
	(void)inputBuffer;

	for (int i = 0; i < framesPerBuffer; i++)
	{
		if (data->read >= data->limit) {
			data->read = 0;
			data->ready = false;
			break;
		}
		for (int j = 0; j < data->numChannels; j++) {
			*out++ = *data->data++;
			//	*out++ = *data->data++;
		}
		data->read++;
	}

	return paContinue;
}

int stuff() {
	PaStreamParameters outputParameters;
	PaStream* stream;
	PaError err;

	const int bufferSize = 44100;
	const float pi = 3.14159265;

	stuff_data data;
	data.data = new float[bufferSize];
	data.read = 0;
	data.limit = bufferSize;
	data.numChannels = 1;

	err = Pa_Initialize();
	if (err != paNoError) {
		cout << "unable to initialize portAudio\n";
		return 1;
	}

	outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
	if (outputParameters.device == paNoDevice) {
		fprintf(stderr, "Error: No default output device.\n");
		return 1;
	}

	outputParameters.channelCount = 1;
	outputParameters.sampleFormat = paFloat32;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

		float dt = 1 / 44100.0;
		//for (int j = 0; j < bufferSize; j++) {
		//	float t = j * dt;
		//	data.data[j] = std::sin(2 * pi * 440 * t);
		//}
		

	err = Pa_OpenStream(
		&stream,
		NULL, /* no input */
		&outputParameters,
		44100,
		512,
		paClipOff,      /* we won't output out of range samples so don't bother clipping them */
		stuff_playback,
		&data);


	err = Pa_StartStream(stream);

	//int NUM_SECONDS = 60;
	//printf("Play for %d seconds.\n", NUM_SECONDS);
	//Pa_Sleep(NUM_SECONDS * 1000);
	//float dt = 1 / 44100.0;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < bufferSize; j++) {
			float t = j * dt;
			data.data[j] = std::sin(2 * pi * 440 * t);
		}
		data.ready = true;
		Pa_Sleep(1000);
	}

	err = Pa_StopStream(stream);

	err = Pa_CloseStream(stream);

	Pa_Terminate();
	printf("Test finished.\n");
	return 0;
}


int main() {

	auto result = read("sound/voice.wav");

	//	return std::visit(WaveVisitor{}, result);
	return stuff();
}