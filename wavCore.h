#pragma once

#include <vector>
#include "wavHeader.h"

class WAV{
public:
	friend std::ostream & operator << (std::ostream & os, const WAV & wav);
	void getHeaderInformation() const;
	void getWAVDate() const;

	void readFromFile(const char * fiename);
	void createWav(const char* filename);

	void makeMono();
	void makeReverb(double delay_seconds, float decay);

private:
	wavHeader header;
	std::vector <std::vector <short >> date;

	void putInfoToItsVector(const std::vector<short> & allChanels, int chan_count, int samples_per_chan);
	std::vector<short> readAllInformation(int chanCount, int samplesPerChan, const char* filename) const;
	void updateHeader();
};