#pragma once 
#include <exception>

enum wav_headers_errors_e {
	HEADER_OK = 0,
	HEADER_RIFF_ERROR,
	HEADER_FILE_SIZE_ERROR,
	HEADER_WAVE_ERROR,
	HEADER_FMT_ERROR,
	HEADER_NOT_PCM,
	HEADER_SUBCHUNK1_ERROR,
	HEADER_BYTES_RATE_ERROR,
	HEADER_BLOCK_ALIGN_ERROR,
	HEADER_SUBCHUNK2_SIZE_ERROR
};

enum wav_errors_e {
	WAV_OK = 0,
	IO_ERROR,
	BAD_FORMAT,
	UNSUPPORTED_FORMAT,
	BAD_PARAMS,
	DATA_SIZE_ERROR
};

class headerRIFFError : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header RIFF Error";
	}
};

class headerFileSizeError : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header File Size Error";
	}
};

class headerWaveError : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header Wave Error";
	}
};

class headerFMTError : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header FMT Error";
	}
};

class headerNotPCMError : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header not PCM Error";
	}
};

class headerSubchunk1Error : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header Subchunk1 Error";
	}
};

class headerBytesRateError : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header Bytes rate Error";
	}
};

class headerBlockAlignError : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header Block Align Error";
	}
};