#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstring>

#include "wav_header.h"
#include "wav_core.h"


// TODO: Remove all 'magic' numbers
// TODO: Make the code more secure. 

void wav_core::getInformation() const {
	printf("-------------------------\n");
	printf(" audioFormat   %u\n", header.date.audioFormat);
	printf(" numChannels   %u\n", header.date.numChannels);
	printf(" sampleRate    %u\n", header.date.sampleRate);
	printf(" bitsPerSample %u\n", header.date.bitsPerSample);
	printf(" byteRate      %u\n", header.date.byteRate);
	printf(" blockAlign    %u\n", header.date.blockAlign);
	printf(" chunkSize     %u\n", header.date.chunkSize);
	printf(" subchunk1Size %u\n", header.date.subchunk1Size);
	printf(" subchunk2Size %u\n", header.date.subchunk2Size);
	printf("-------------------------\n");
}

wav_errors_e wav_core::readDate(const char* filename, std::vector<std::vector<short>>& channels_data)
{
	printf(">>>> extract_data_int16( %s )\n", filename);
	wav_errors_e err;
	err = header.readHeader(filename);
	if (err != WAV_OK) {
		// Problems with reading a header.
		return err;
	}

	if (header.date.bitsPerSample != 16) {
		// Only 16-bit samples is supported.
		return UNSUPPORTED_FORMAT;
	}

	FILE* f = fopen(filename, "rb");
	if (!f) {
		return IO_ERROR;
	}
	fseek(f, 44, SEEK_SET); // Seek to the begining of PCM data.

	int chan_count = header.date.numChannels;
	int samples_per_chan = (header.date.subchunk2Size / sizeof(short)) / chan_count;

	// 1. Reading all PCM data from file to a single vector.
	std::vector<short> all_channels;
	all_channels.resize(chan_count * samples_per_chan);
	size_t read_bytes = fread(all_channels.data(), 1, header.date.subchunk2Size, f);
	if (read_bytes != header.date.subchunk2Size) {
		printf("extract_data_int16() read only %zu of %u\n", read_bytes, header.date.subchunk2Size);
		return IO_ERROR;
	}
	fclose(f);


	// 2. Put all channels to its own vector.
	channels_data.resize(chan_count);
	for (size_t ch = 0; ch < channels_data.size(); ch++) {
		channels_data[ch].resize(samples_per_chan);
	}

	for (int ch = 0; ch < chan_count; ch++) {
		std::vector<short>& chdata = channels_data[ch];
		for (size_t i = 0; i < samples_per_chan; i++) {
			chdata[i] = all_channels[chan_count * i + ch];
		}
	}
	return WAV_OK;
}

wav_errors_e wav_core::createWav(const char* filename, int sample_rate, const std::vector< std::vector<short> > &channels_data)
{
	printf(">>>> make_wav_file( %s )\n", filename);
	wav_errors_e err;

	int chan_count = (int)channels_data.size();

	if (chan_count < 1) {
		return BAD_PARAMS;
	}

	int samples_count_per_chan = (int)channels_data[0].size();

	// Verify that all channels have the same number of samples.
	for (size_t ch = 0; ch < chan_count; ch++) {
		if (channels_data[ch].size() != (size_t)samples_count_per_chan) {
			return BAD_PARAMS;
		}
	}

	err = header.fillHeader(chan_count, 16, sample_rate, samples_count_per_chan);
	if (err != WAV_OK) {
		return err;
	}

	std::vector<short> all_channels;
	all_channels.resize(chan_count * samples_count_per_chan);

	for (int ch = 0; ch < chan_count; ch++) {
		const std::vector<short>& chdata = channels_data[ch];
		for (size_t i = 0; i < samples_count_per_chan; i++) {
			all_channels[chan_count * i + ch] = chdata[i];
		}
	}

	FILE* f = fopen(filename, "wb");
	fwrite(&header, sizeof(wav_header_s::Date), 1, f);
	fwrite(all_channels.data(), sizeof(short), all_channels.size(), f);
	if (!f) {
		return IO_ERROR;
	}

	fclose(f);

	return WAV_OK;
}


wav_errors_e wav_core::makeMono(const std::vector<std::vector<short> > &source, std::vector< std::vector<short> > &dest_mono)
{
	int chan_count = (int)source.size();

	if (chan_count != 2) {
		return BAD_PARAMS;
	}

	int samples_count_per_chan = (int)source[0].size();

	// Verify that all channels have the same number of samples.
	for (size_t ch = 0; ch < chan_count; ch++) {
		if (source[ch].size() != (size_t)samples_count_per_chan) {
			return BAD_PARAMS;
		}
	}

	dest_mono.resize(1);
	std::vector<short>& mono = dest_mono[0];
	mono.resize(samples_count_per_chan);

	// Mono channel is an arithmetic mean of all (two) channels.
	for (size_t i = 0; i < samples_count_per_chan; i++) {
		mono[i] = (source[0][i] + source[1][i]) / 2;
	}

	return WAV_OK;
}

wav_errors_e wav_core::makeReverb(std::vector<std::vector<short> > &sounds, int sample_rate, double delay_seconds, float decay)
{
	int chan_count = (int)sounds.size();

	if (chan_count < 1) {
		return BAD_PARAMS;
	}

	int samples_count_per_chan = (int)sounds[0].size();

	// Verify that all channels have the same number of samples.
	for (size_t ch = 0; ch < chan_count; ch++) {
		if (sounds[ch].size() != (size_t)samples_count_per_chan) {
			return BAD_PARAMS;
		}
	}

	int delay_samples = (int)(delay_seconds * sample_rate);


	for (size_t ch = 0; ch < chan_count; ch++) {
		std::vector<float> tmp;
		tmp.resize(sounds[ch].size());

		// Convert signal from short to float
		for (size_t i = 0; i < samples_count_per_chan; i++) {
			tmp[i] = sounds[ch][i];
		}

		// Add a reverb
		for (size_t i = 0; i < samples_count_per_chan - delay_samples; i++) {
			tmp[i + delay_samples] += decay * tmp[i];
		}

		// Find maximum signal's magnitude
		float max_magnitude = 0.0f;
		for (size_t i = 0; i < samples_count_per_chan - delay_samples; i++) {
			if (abs(tmp[i]) > max_magnitude) {
				max_magnitude = abs(tmp[i]);
			}
		}

		// Signed short can keep values from -32768 to +32767,
		// After reverb, usually there are values large 32000.
		// So we must scale all values back to [ -32768 ... 32768 ]
		float norm_coef = 30000.0f / max_magnitude;
		printf("max_magnitude = %.1f, coef = %.3f\n", max_magnitude, norm_coef);

		// Scale back and transform floats to shorts.
		for (size_t i = 0; i < samples_count_per_chan; i++) {
			sounds[ch][i] = (short)(norm_coef * tmp[i]);
		}
	}

	return WAV_OK;
}

