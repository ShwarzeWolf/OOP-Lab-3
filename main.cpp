#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "wavCore.h"
#include "wavHeader.h"

using namespace std;

int main(){
	try{
		const char* inputFileName = "wavSample.wav";
		const char* outputFileName = "newout.wav";
		WAV wav;
		wav.readFromFile(inputFileName);
		cout << wav;
		wav.makeReverb(0.1,0.2);
		cout << wav;
		wav.createWav(outputFileName);

		WAV checkWav;
		checkWav.readFromFile(outputFileName);
		cout << checkWav;
	
	}
	catch (exception & error){};

	system("pause");

	return 0;
}