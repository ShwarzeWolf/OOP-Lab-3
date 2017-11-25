#pragma once 
#include <exception>

// wav header's exceptions
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

class headerSubchunk2SizeError : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header Subchunk2 size Error";
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

// wav exceptions

class  inputOutputException: public std::exception {
	const char* what() const throw()
	{
		return "file was not found";
	}
};

class badFormatException: public std::exception {
	virtual const char* what() const throw()
	{
		return "Bad file format";
	}
};

class unsupportedFormatException: public std::exception {
	virtual const char* what() const throw()
	{
		return "unsupported format";
	}
};

class badParametrsException: public std::exception {
	virtual const char* what() const throw()
	{
		return "Bad parametrs";
	}
};

class dataSizeException: public std::exception {
	virtual const char* what() const throw()
	{
		return "data size exception";
	}
};
