#include <iostream>
#include <fstream>

#include "wavHeader.h"
#include "wavExceptions.h"

using namespace std;

void wavHeader::readHeader(const char *filename){
	cout << ">>>reading header from " << filename << " file" << endl;

	try{
		ifstream inputFile(filename, ios_base::binary);
		if (!inputFile.good())
			throw inputOutputException();

		try
		{
			inputFile.seekg(0, std::ios::end);
			const streamoff fileSize = inputFile.tellg();
			inputFile.seekg(0, std::ios::beg);

			if ((size_t)fileSize < sizeof(Parametrs) * BITSINBYTE)
				throw badFormatException();

			inputFile.read((char *)&parametrs, sizeof(Parametrs));

			if (!checkHeader(size_t(fileSize)))
				throw badFormatException();

		}
		catch (exception & error){
			cout << error.what() << endl;
			inputFile.close();
		};

		inputFile.close();

	}
	catch (exception & error){
		throw inputOutputException();
	};

}

bool wavHeader::checkHeader(size_t fileSize) const{

	try{
		if (parametrs.chunkId != RIFFSYMBOLS)
			throw headerRIFFException();
		
		if (parametrs.chunkSize != fileSize - sizeof(parametrs.chunkId) - sizeof(parametrs.chunkSize))
			throw headerFileSizeException();

		if (parametrs.format != WAVESYMBOLS)
			throw headerWaveException();

		if (parametrs.subchunk1Id != FMTSYMBOLS)
			throw headerFMTException();

		if (parametrs.audioFormat != 1)
			throw headerNotPCMException();

		if (parametrs.subchunk1Size != 16)
			throw headerSubchunk1Exception();

		if (parametrs.byteRate * BITSINBYTE != parametrs.sampleRate * parametrs.numChannels * parametrs.bitsPerSample)
			throw headerBytesRateException();

		if (parametrs.blockAlign * BITSINBYTE != parametrs.numChannels * parametrs.bitsPerSample)
			throw headerBlockAlignException();

		if (parametrs.subchunk2Id != DATESYMBOLS)
			throw headerFMTException();

		if (parametrs.subchunk2Size != fileSize - sizeof(Parametrs))
			throw headerSubchunk2SizeException();
	}
	catch (exception & error){
		cout << error.what() << endl;
		return false;
	};

	return true;
}

void wavHeader::fillHeader(int chanCount, int bitsPerSample, int sampleRate, int samplesCountPerChan)
{
	try{
		if (bitsPerSample != 16)
			throw unsupportedFormatException();

		if (chanCount < 1)
			throw badParametrsException();
		prefillHeader();

		const int fileSizeBytes = sizeof(Parametrs) + chanCount * (bitsPerSample / BITSINBYTE) * samplesCountPerChan;

		parametrs.sampleRate = sampleRate;
		parametrs.numChannels = chanCount;
		parametrs.bitsPerSample = 16;

		parametrs.chunkSize = fileSizeBytes - sizeof(parametrs.chunkId) - sizeof(parametrs.chunkSize);
		parametrs.subchunk2Size = fileSizeBytes - sizeof(Parametrs);

		parametrs.byteRate = parametrs.sampleRate * parametrs.numChannels * parametrs.bitsPerSample / BITSINBYTE;
		parametrs.blockAlign = parametrs.numChannels * parametrs.bitsPerSample / BITSINBYTE;
	}
	catch (exception & error){
		cout << error.what() << endl;
	};
}

void wavHeader::prefillHeader(){
	parametrs.chunkId = RIFFSYMBOLS;
	parametrs.format = WAVESYMBOLS;
	parametrs.subchunk1Id = FMTSYMBOLS;
	parametrs.subchunk2Id = DATESYMBOLS;

	parametrs.audioFormat = 1;
	parametrs.subchunk1Size = 16;
	parametrs.bitsPerSample = 16;
}