#define _CRT_SECURE_NO_WARNINGS

#pragma once


#include "wavHeaderExceptions.cpp"

// Got from
// https://audiocoding.ru/article/2008/05/22/wav-file-structure.html
//
// See also:
// http://soundfile.sapp.org/doc/WaveFormat/
//


// Структура, описывающая заголовок WAV файла.
class wav_header_s
{
public:
	struct Date{
		// WAV-формат начинается с RIFF-заголовка:

		// Содержит символы "RIFF" в ASCII кодировке
		// (0x52494646 в big-endian представлении)
		char chunkId[4];

		// 36 + subchunk2Size, или более точно:
		// 4 + (8 + subchunk1Size) + (8 + subchunk2Size)
		// Это оставшийся размер цепочки, начиная с этой позиции.
		// Иначе говоря, это размер файла - 8, то есть,
		// исключены поля chunkId и chunkSize.
		unsigned long chunkSize;

		// Содержит символы "WAVE"
		// (0x57415645 в big-endian представлении)
		char format[4];

		// Формат "WAVE" состоит из двух подцепочек: "fmt " и "data":
		// Подцепочка "fmt " описывает формат звуковых данных:

		// Содержит символы "fmt "
		// (0x666d7420 в big-endian представлении)
		char subchunk1Id[4];

		// 16 для формата PCM.
		// Это оставшийся размер подцепочки, начиная с этой позиции.
		unsigned long subchunk1Size;

		// Аудио формат, полный список можно получить здесь http://audiocoding.ru/wav_formats.txt
		// Для PCM = 1 (то есть, Линейное квантование).
		// Значения, отличающиеся от 1, обозначают некоторый формат сжатия.
		unsigned short audioFormat;

		// Количество каналов. Моно = 1, Стерео = 2 и т.д.
		unsigned short numChannels;

		// Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
		unsigned long sampleRate;

		// sampleRate * numChannels * bitsPerSample/8
		unsigned long byteRate;

		// numChannels * bitsPerSample/8
		// Количество байт для одного сэмпла, включая все каналы.
		unsigned short blockAlign;

		// Так называемая "глубиная" или точность звучания. 8 бит, 16 бит и т.д.
		unsigned short bitsPerSample;

		// Подцепочка "data" содержит аудио-данные и их размер.

		// Содержит символы "data"
		// (0x64617461 в big-endian представлении)
		char subchunk2Id[4];

		// numSamples * numChannels * bitsPerSample/8
		// Количество байт в области данных.
		unsigned long subchunk2Size;
	};
	// Далее следуют непосредственно Wav данные.

	Date date;

	// Fills header with zeroes.
	void nullHeader()
	{
		memset(&date, 0, sizeof(wav_header_s::Date));
	}

	// does this function refer to wav header?
	// Reads file 'filename' and puts header's data to 'header_ptr' address.
	// Also checks header validity, returns 'WAV_OK' on success.

	wav_errors_e readHeader(const char *filename)
	{
		printf(">>>> read_header( %s )\n", filename);
		nullHeader(); // Fill header with zeroes.

		FILE* f = fopen(filename, "rb");
		if (!f) {
			return IO_ERROR;
		}

		size_t blocks_read = fread(&date, sizeof(wav_header_s::Date), 1, f);
		if (blocks_read != 1) {
			// can't read header, because the file is too small.
			return BAD_FORMAT;
		}

		fseek(f, 0, SEEK_END); // seek to the end of the file
		size_t file_size = ftell(f); // current position is a file size!
		fclose(f);

		if (checkHeader(file_size) != HEADER_OK) {
			return BAD_FORMAT;
		}
		else {
			return WAV_OK;
		}
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
