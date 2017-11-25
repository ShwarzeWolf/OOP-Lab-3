#include "wav_header.h"
using namespace std;
#include <iostream>

// Fills header with zeroes.
// what about put this in constructor?
void wav_header_s::nullHeader()
{
	memset(&date, 0, sizeof(wav_header_s::Date));
}

// Reads file 'filename' and puts header's data to 'header'
//here i need to solve a problem with working with file in case it was not read
//on of ways - to return bool
// second way - to make special flag that will signalyse whether or not file was filled 
//the same problem appeared with other functions. what should i do?
// use try/ catch in main code, may be

void wav_header_s::readHeader(const char *filename)
{
	cout << " reading header from " << filename << "file" << endl;
	nullHeader(); // Fill header with zeroes.

	FILE* f = fopen(filename, "rb");

	try{
		if (!f)
			throw inputOutputException();

		size_t blocks_read = fread(&date, sizeof(wav_header_s::Date), 1, f);
		try{
			// can't read header, because the file is too small.
			if (blocks_read != 1) 
				throw badFormatException();
	
		fseek(f, 0, SEEK_END); // seek to the end of the file
		size_t file_size = ftell(f); // current position is a file size!

			if (!checkHeader(file_size))
				throw badFormatException();

		}
		catch (exception error){
			cout << error.what() << endl;
			fclose(f);
		};

		fclose(f);
	}
	catch (exception &error){
		cout << error.what() << endl;
	}

}

// Checks header validity.
bool wav_header_s::checkHeader(size_t file_size_bytes) const
{
	// Go to wav_header.h for details
	try{
		if (date.chunkId[0] != 0x52 ||
			date.chunkId[1] != 0x49 ||
			date.chunkId[2] != 0x46 ||
			date.chunkId[3] != 0x46)
			throw headerRIFFError();

		if (date.chunkSize != file_size_bytes - 8)
			throw headerFileSizeError();

		if (date.format[0] != 0x57 ||
			date.format[1] != 0x41 ||
			date.format[2] != 0x56 ||
			date.format[3] != 0x45)
			throw headerWaveError();

		if (date.subchunk1Id[0] != 0x66 ||
			date.subchunk1Id[1] != 0x6d ||
			date.subchunk1Id[2] != 0x74 ||
			date.subchunk1Id[3] != 0x20)
			throw headerFMTError();

		if (date.audioFormat != 1)
			throw headerNotPCMError();

		if (date.subchunk1Size != 16)
			throw headerSubchunk1Error();

		if (date.byteRate != date.sampleRate * date.numChannels * date.bitsPerSample / 8)
			throw headerBytesRateError();

		if (date.blockAlign != date.numChannels * date.bitsPerSample / 8)
			throw headerBlockAlignError();

		if (date.subchunk2Id[0] != 0x64 ||
			date.subchunk2Id[1] != 0x61 ||
			date.subchunk2Id[2] != 0x74 ||
			date.subchunk2Id[3] != 0x61)
			throw headerFMTError();

		if (date.subchunk2Size != file_size_bytes - 44)
			throw headerSubchunk2SizeError();
	}
	catch (exception & error){
		cout << error.what() << endl;
		return false;
	};

	return true;
}

// Fills header information, using input parameters. This function calls prefill_header() itself.
void wav_header_s::fillHeader(int chan_count, int bits_per_sample, int sample_rate, int samples_count_per_chan)
{
	try{
		if (bits_per_sample != 16)
			throw unsupportedFormatException();

		if (chan_count < 1)
			throw badParametrsException();

		prefillHeader();

		int file_size_bytes = 44 + chan_count * (bits_per_sample / 8) * samples_count_per_chan;

		date.sampleRate = sample_rate;
		date.numChannels = chan_count;
		date.bitsPerSample = 16;

		date.chunkSize = file_size_bytes - 8;
		date.subchunk2Size = file_size_bytes - 44;

		date.byteRate = date.sampleRate * date.numChannels * date.bitsPerSample / 8;
		date.blockAlign = date.numChannels * date.bitsPerSample / 8;
	}
	catch (exception & error){
		cout << error.what() << endl;
	};
}

// Fills 'header_ptr' with default values.
void wav_header_s::prefillHeader()
{
	// Go to wav_header.h for details

	date.chunkId[0] = 0x52;
	date.chunkId[1] = 0x49;
	date.chunkId[2] = 0x46;
	date.chunkId[3] = 0x46;

	date.format[0] = 0x57;
	date.format[1] = 0x41;
	date.format[2] = 0x56;
	date.format[3] = 0x45;

	date.subchunk1Id[0] = 0x66;
	date.subchunk1Id[1] = 0x6d;
	date.subchunk1Id[2] = 0x74;
	date.subchunk1Id[3] = 0x20;

	date.subchunk2Id[0] = 0x64;
	date.subchunk2Id[1] = 0x61;
	date.subchunk2Id[2] = 0x74;
	date.subchunk2Id[3] = 0x61;

	date.audioFormat = 1;
	date.subchunk1Size = 16;
	date.bitsPerSample = 16;

}
