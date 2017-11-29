#pragma once

#include <iostream>
#include "wavExceptions.h"

class wavHeader{
public:

	wavHeader(){
		memset(&parametrs, 0, sizeof(wavHeader::Parametrs));
	};

	void readHeader(const char *filename);

	bool checkHeader(const size_t fileSize) const;

	void wavHeader::fillHeader(int chanCount, int bitsPerSample, int sampleRate, int samplesCountPerChan);

	struct Parametrs{
		__int32 chunkId;
		__int32 chunkSize;
		__int32 format;
		__int32 subchunk1Id;
		__int32 subchunk1Size;
		__int16 audioFormat;
		__int16 numChannels;
		__int32 sampleRate;
		__int32 byteRate;
		__int16 blockAlign;
		__int16 bitsPerSample;
		__int32 subchunk2Id;
		__int32 subchunk2Size;
	};

	Parametrs parametrs;

	enum numberOfChanels{
		NOC_MONO = 1,
		NOC_STEREO = 2
	};

private:

	static const __int32 RIFFSYMBOLS = 0x46464952; //chunckID
	static const __int32 WAVESYMBOLS = 0x45564157; //format
	static const __int32 FMTSYMBOLS = 0x20746d66;//subchunck1Id
	static const __int32 DATESYMBOLS = 0x61746164; //subchunck2Id

	static const __int32 BITSINBYTE = 8;

	void prefillHeader();
};