#pragma once
#include <exception>

class headerRIFFException : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header RIFF error\n";
	}
};

class  inputOutputException : public std::exception {
	const char* what() const throw()
	{
		return "File was not found\n";
	}
};

class unsupportedFormatException : public std::exception {
	virtual const char* what() const throw()
	{
		return "Unsupported format\n";
	}
};

class badParametrsException : public std::exception {
	virtual const char* what() const throw()
	{
		return "Bad parametrs\n";
	}
};

class headerFileSizeException : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header file size error\n";
	}
};

class headerWaveException : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header wave error\n";
	}
};

class headerFMTException : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header FMT error\n";
	}
};

class headerNotPCMException : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header not PCM error\n";
	}
};

class headerSubchunk1Exception : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header subchunk1 error\n";
	}
};

class headerSubchunk2SizeException : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header subchunk2 size error\n";
	}
};

class headerBytesRateException : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header bytes rate error\n";
	}
};

class headerBlockAlignException : public std::exception {
	virtual const char* what() const throw()
	{
		return "Header block align error\n";
	}
};

class badFormatException : public std::exception {
	virtual const char* what() const throw()
	{
		return "Bad file format\n";
	}
};

class dataSizeException : public std::exception {
	virtual const char* what() const throw()
	{
		return "Data size exception\n";
	}
};