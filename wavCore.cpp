#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
#include "wavExceptions.h"
#include "wavHeader.h"
#include "wavCore.h"

using namespace std;

ostream & operator << (ostream & os, const WAV & wav){
	os << ">>>wav header information:" << endl;
	os << " audioFormat \t" << wav.header.parametrs.audioFormat << "\n";
	os << " numChannels \t" << wav.header.parametrs.numChannels << "\n";
	os << " sampleRate \t" << wav.header.parametrs.sampleRate << "\n";
	os << " bitsPerSample \t" << wav.header.parametrs.bitsPerSample << "\n";
	os << " byteRate \t" << wav.header.parametrs.byteRate << "\n";
	os << " blockAlign \t" << wav.header.parametrs.blockAlign << "\n";
	os << " chunkSize \t" << wav.header.parametrs.chunkSize << "\n";
	os << " subchunk1Size \t" << wav.header.parametrs.subchunk1Size << "\n";
	os << " subchunk2Size \t" << wav.header.parametrs.subchunk2Size << "\n";
	return os;
}
void WAV::getHeaderInformation() const {
	cout << ">>>wav header information:" << endl;
	cout << " audioFormat \t" << header.parametrs.audioFormat << "\n";
	cout << " numChannels \t" << header.parametrs.numChannels << "\n";
	cout << " sampleRate \t" << header.parametrs.sampleRate << "\n";
	cout << " bitsPerSample \t" << header.parametrs.bitsPerSample << "\n";
	cout << " byteRate \t" << header.parametrs.byteRate << "\n";
	cout << " blockAlign \t" << header.parametrs.blockAlign << "\n";
	cout << " chunkSize \t" << header.parametrs.chunkSize << "\n";
	cout << " subchunk1Size \t" << header.parametrs.subchunk1Size << "\n";
	cout << " subchunk2Size \t" << header.parametrs.subchunk2Size << "\n";
	cout << endl;
}
void WAV::getWAVDate()const{
	cout << "wav date is:\n";
	for (auto i = date.begin(); i != date.end(); ++i){
		for (auto j = i->begin(); j != i->end(); ++j)
			cout << *j << " ";
		cout << endl;
	};
};

vector<short> WAV::readAllInformation(int chanCount, int samplesPerChan, const char* fileName) const{

	FILE* inputFile = fopen(fileName, "rb");
	if (!inputFile) throw
		inputOutputException();

	fseek(inputFile, sizeof(wavHeader::Parametrs), SEEK_SET);

	vector<short> allChannels;

	allChannels.resize(chanCount * samplesPerChan);
	size_t readBytes = fread(allChannels.data(), 1, header.parametrs.subchunk2Size, inputFile);
	if (readBytes != header.parametrs.subchunk2Size)
		throw inputOutputException();

	fclose(inputFile);
	return allChannels;
};

void WAV::putInfoToItsVector( const vector<short> & allChanels, int chanCount, int samplesPerChan){
	date.resize(chanCount);
	for (size_t ch = 0; ch < date.size(); ++ch)
		date[ch].resize(samplesPerChan);

	for (int ch = 0; ch < chanCount; ++ch) {
		vector<short>& chdata = date[ch];
		for (size_t i = 0; i < samplesPerChan; ++i)
			chdata[i] = allChanels[chanCount * i + ch];
	};
}
void WAV::updateHeader(){
	const int chanCount = (int)date.size();
	int samplesCountPerChan = (int)date[0].size();
	for (size_t ch = 0; ch < chanCount; ++ch)
		if (date[ch].size() != (size_t)samplesCountPerChan)
			throw badParametrsException();
	header.fillHeader(chanCount, 16, header.parametrs.sampleRate, samplesCountPerChan);
}
void WAV::readFromFile(const char *fileName){
	try {
		header.readHeader(fileName);

		cout << ">>>reading data from " << fileName << "\n";
		
		if (header.parametrs.bitsPerSample != 16)
			throw unsupportedFormatException();

		const int chanCount = header.parametrs.numChannels;
		const int samplesPerChan = (header.parametrs.subchunk2Size / sizeof(short)) / chanCount;

		const vector<short> allChanels = readAllInformation(chanCount, samplesPerChan, fileName);

		putInfoToItsVector(allChanels, chanCount, samplesPerChan);

	}
	catch (exception & error){
		cout << error.what();
		throw exception();
	};
};

void WAV::createWav(const char* fileName){
	cout << "make wav file " << fileName << "\n";
	try{
		const int chanCount = (int)date.size();
		const int samplesCountPerChan = (int)date[0].size();
		std::vector<short> allChannels;
		allChannels.resize(chanCount * samplesCountPerChan);

		for (int ch = 0; ch < chanCount; ++ch){
			const std::vector<short>& chdata = date[ch];
			for (size_t i = 0; i < samplesCountPerChan; i++)
				allChannels[chanCount * i + ch] = chdata[i];
		}

		FILE* outputFile = fopen(fileName, "wb");
		fwrite(&header, sizeof(wavHeader::Parametrs), 1, outputFile);
		fwrite(allChannels.data(), sizeof(short), allChannels.size(), outputFile);
		if (!outputFile)
			throw inputOutputException();
		fclose(outputFile);
	}
	catch (exception & error){
		cout << error.what() << endl;
	};
}

void WAV::makeMono(){
	cout << ">>>making mono \n";
	vector <vector <short >> destMono;
	try {
		const int chanCount = (int)date.size();

		if (chanCount != wavHeader::NOC_STEREO)
			throw badParametrsException();

		const int samplesCountPerChan = (int)date[0].size();

		for (size_t ch = 0; ch < chanCount; ++ch)
			if (date[ch].size() != (size_t)samplesCountPerChan)
				throw badParametrsException();

		destMono.resize(1);
		std::vector<short>& mono = destMono[0];
		mono.resize(samplesCountPerChan);

		for (size_t i = 0; i < samplesCountPerChan; ++i)
			mono[i] = (date[0][i] + date[1][i]) / 2;

		date = destMono;
	}
	catch (exception & error){
		cout << error.what();
		cout << "Operation failed\n";
	};
	updateHeader();
}

void WAV::makeReverb(double delaySeconds, float decay){
	try{
		const int chanCount = (int)date.size();

		if (chanCount < 1)
			throw badParametrsException();

		const int samplesCountPerChan = (int)date[0].size();

		for (size_t ch = 0; ch < chanCount; ++ch)
			if (date[ch].size() != (size_t)samplesCountPerChan)
				throw badParametrsException();

		const int delaySamples = (int)(delaySeconds * header.parametrs.sampleRate);

		for (size_t ch = 0; ch < chanCount; ++ch) {
			std::vector<float> tmp;
			tmp.resize(date[ch].size());

			for (size_t i = 0; i < samplesCountPerChan; ++i)
				tmp[i] = date[ch][i];

			cout << samplesCountPerChan << " " << delaySamples << "\n";
			for (int i = 0; i < samplesCountPerChan - delaySamples; ++i)
				tmp[i + delaySamples] += decay * tmp[i];

			float maxMagnitude = 0.0f;
			for (int i = 0; i < samplesCountPerChan - delaySamples; ++i)
				if (abs(tmp[i]) > maxMagnitude)
					maxMagnitude = abs(tmp[i]);

			float normCoef = 30000.0f / maxMagnitude;
			cout << "max magnitude = " << maxMagnitude << " coef = " << normCoef << "\n";

			for (size_t i = 0; i < samplesCountPerChan; i++)
				date[ch][i] = (short)(normCoef * tmp[i]);
		};
	}
	catch (exception & error){
		cout << error.what() << endl;
	};
	updateHeader();
};
