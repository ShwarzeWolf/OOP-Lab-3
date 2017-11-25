#pragma once

#include "wavHeaderExceptions.cpp"

// Got from
// https://audiocoding.ru/article/2008/05/22/wav-file-structure.html
//
// See also:
// http://soundfile.sapp.org/doc/WaveFormat/
//


// ���������, ����������� ��������� WAV �����.
class wav_header_s
{
public:
	struct Date{
		// WAV-������ ���������� � RIFF-���������:

		// �������� ������� "RIFF" � ASCII ���������
		// (0x52494646 � big-endian �������������)
		char chunkId[4];

		// 36 + subchunk2Size, ��� ����� �����:
		// 4 + (8 + subchunk1Size) + (8 + subchunk2Size)
		// ��� ���������� ������ �������, ������� � ���� �������.
		// ����� ������, ��� ������ ����� - 8, �� ����,
		// ��������� ���� chunkId � chunkSize.
		unsigned long chunkSize;

		// �������� ������� "WAVE"
		// (0x57415645 � big-endian �������������)
		char format[4];

		// ������ "WAVE" ������� �� ���� ����������: "fmt " � "data":
		// ���������� "fmt " ��������� ������ �������� ������:

		// �������� ������� "fmt "
		// (0x666d7420 � big-endian �������������)
		char subchunk1Id[4];

		// 16 ��� ������� PCM.
		// ��� ���������� ������ ����������, ������� � ���� �������.
		unsigned long subchunk1Size;

		// ����� ������, ������ ������ ����� �������� ����� http://audiocoding.ru/wav_formats.txt
		// ��� PCM = 1 (�� ����, �������� �����������).
		// ��������, ������������ �� 1, ���������� ��������� ������ ������.
		unsigned short audioFormat;

		// ���������� �������. ���� = 1, ������ = 2 � �.�.
		unsigned short numChannels;

		// ������� �������������. 8000 ��, 44100 �� � �.�.
		unsigned long sampleRate;

		// sampleRate * numChannels * bitsPerSample/8
		unsigned long byteRate;

		// numChannels * bitsPerSample/8
		// ���������� ���� ��� ������ ������, ������� ��� ������.
		unsigned short blockAlign;

		// ��� ���������� "��������" ��� �������� ��������. 8 ���, 16 ��� � �.�.
		unsigned short bitsPerSample;

		// ���������� "data" �������� �����-������ � �� ������.

		// �������� ������� "data"
		// (0x64617461 � big-endian �������������)
		char subchunk2Id[4];

		// numSamples * numChannels * bitsPerSample/8
		// ���������� ���� � ������� ������.
		unsigned long subchunk2Size;
	};
	// ����� ������� ��������������� Wav ������.

	Date date;

	// Fills header with zeroes.
	void nullHeader()
	{
		memset(&date, 0, sizeof(wav_header_s::Date));
	}


	// Checks header validity.
	// Returns 'WAV_OK' on success.
	wav_headers_errors_e checkHeader(size_t file_size_bytes) const
	{
		// Go to wav_header.h for details

		if (date.chunkId[0] != 0x52 ||
			date.chunkId[1] != 0x49 ||
			date.chunkId[2] != 0x46 ||
			date.chunkId[3] != 0x46)
		{
			printf("HEADER_RIFF_ERROR\n");
			return HEADER_RIFF_ERROR;
		}

		if (date.chunkSize != file_size_bytes - 8) {
			printf("HEADER_FILE_SIZE_ERROR\n");
			return HEADER_FILE_SIZE_ERROR;
		}

		if (date.format[0] != 0x57 ||
			date.format[1] != 0x41 ||
			date.format[2] != 0x56 ||
			date.format[3] != 0x45)
		{
			printf("HEADER_WAVE_ERROR\n");
			return HEADER_WAVE_ERROR;
		}

		if (date.subchunk1Id[0] != 0x66 ||
			date.subchunk1Id[1] != 0x6d ||
			date.subchunk1Id[2] != 0x74 ||
			date.subchunk1Id[3] != 0x20)
		{
			printf("HEADER_FMT_ERROR\n");
			return HEADER_FMT_ERROR;
		}

		if (date.audioFormat != 1) {
			printf("HEADER_NOT_PCM\n");
			return HEADER_NOT_PCM;
		}

		if (date.subchunk1Size != 16) {
			printf("HEADER_SUBCHUNK1_ERROR\n");
			return HEADER_SUBCHUNK1_ERROR;
		}

		if (date.byteRate != date.sampleRate * date.numChannels * date.bitsPerSample / 8) {
			printf("HEADER_BYTES_RATE_ERROR\n");
			return HEADER_BYTES_RATE_ERROR;
		}

		if (date.blockAlign != date.numChannels * date.bitsPerSample / 8) {
			printf("HEADER_BLOCK_ALIGN_ERROR\n");
			return HEADER_BLOCK_ALIGN_ERROR;
		}

		if (date.subchunk2Id[0] != 0x64 ||
			date.subchunk2Id[1] != 0x61 ||
			date.subchunk2Id[2] != 0x74 ||
			date.subchunk2Id[3] != 0x61)
		{
			printf("HEADER_FMT_ERROR\n");
			return HEADER_FMT_ERROR;
		}

		if (date.subchunk2Size != file_size_bytes - 44)
		{
			printf("HEADER_SUBCHUNK2_SIZE_ERROR\n");
			return HEADER_SUBCHUNK2_SIZE_ERROR;
		}

		return HEADER_OK;
	}



	// Fills header information, using input parameters. This function calls prefill_header() itself.
	wav_errors_e fillHeader(int chan_count, int bits_per_sample, int sample_rate, int samples_count_per_chan)
	{
		if (bits_per_sample != 16) {
			return UNSUPPORTED_FORMAT;
		}

		if (chan_count < 1) {
			return BAD_PARAMS;
		}
		prefillHeader();

		int file_size_bytes = 44 + chan_count * (bits_per_sample / 8) * samples_count_per_chan;

		date.sampleRate = sample_rate;
		date.numChannels = chan_count;
		date.bitsPerSample = 16;

		date.chunkSize = file_size_bytes - 8;
		date.subchunk2Size = file_size_bytes - 44;

		date.byteRate = date.sampleRate * date.numChannels * date.bitsPerSample / 8;
		date.blockAlign = date.numChannels * date.bitsPerSample / 8;

		return WAV_OK;
	}

	// Fills 'header_ptr' with default values.
	void prefillHeader()
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

};




