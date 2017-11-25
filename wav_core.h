#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <vector>

#include "wav_header.h"

// TODO: Make support for 8-bit PSM, not just 16-bit.
// TODO: Write a function to change the tone of the voice. (Interestingly, it's not too difficult?)

class wav_core{
public:

	void getInformation() const;

	void readHeader(const char *filename){
		header.readHeader(filename);
	};

	// Reads file 'filename' and puts PCM data (raw sound data) to 'channels_data'.
	// Also checks header validity, returns 'WAV_OK' on success.
	void readDate(const char* filename, std::vector<std::vector<short>>& channels_data);

	// Creates a new WAV file 'filename', using 'sample_rate' and PCM data from 'channels_data'.
	// Returns 'WAV_OK' on success.
	void createWav(const char* filename, int sample_rate, const std::vector<std::vector<short>>& channels_data);

	// ************************************************************************
	// * Functions for working with sound PCM data - Digital Signal Processing
	// ************************************************************************

	// Makes mono PCM data from stereo 'source'.
	// Returns 'WAV_OK' on success.
	void makeMono(const std::vector<std::vector<short>>& source, std::vector<std::vector<short>>& dest_mono);

	// Adds a reverberation (it's like an echo).
	// Returns 'WAV_OK' on success.
	void makeReverb(std::vector<std::vector<short>>& sounds, int sample_rate, double delay_seconds, float decay);

	unsigned long getSampleRate(){
		return header.date.sampleRate;
	}

private:

	wav_header_s header;

};
