#define _CRT_SECURE_NO_WARNINGS

#pragma once
#include <iostream>

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
	void nullHeader();

	// Reads file 'filename' and puts header's data to 'header_ptr' address.
	// Also checks header validity, returns 'WAV_OK' on success.

	void readHeader(const char *filename);

	// Checks header validity.
	// Returns 'WAV_OK' on success.
	bool checkHeader(size_t file_size_bytes) const;

	// Fills header information, using input parameters. This function calls prefill_header() itself.
	void fillHeader(int chan_count, int bits_per_sample, int sample_rate, int samples_count_per_chan);

	// Fills 'header_ptr' with default values.
	void prefillHeader();

};




