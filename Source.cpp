#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include "wav_core.h"

using namespace std;

int main()
{
	cout << "************** | WavCore | **************" << endl;

	// ################  Tests for WavCore  ################

	const char* input_fname = "wavSample.wav";
	const char* output_fname = "out.wav";

	wav_core wav;

	// #### Opening WAV file, checking header.
	wav.readHeader(input_fname);


	// #### Printing header.
	wav.getInformation();

	
	// #### Reading PCM data from file.
	vector< vector<short> > chans_data;

	wav.readDate(input_fname, chans_data);
	
	// #### Make several changes to PCM data.

	// # Making signal mono from stereo.
	vector< vector<short> > edited_data;
	wav.makeMono(chans_data, edited_data);
	
	// # Add a reverberation
	//wav.make_reverb(edited_data, wav.getSampleRate(), 0.5, 0.6f);
	
	wav_core newWav;

	// #### Making new WAV file using edited PCM data.
	newWav.createWav(output_fname, wav.getSampleRate(), edited_data);

	wav_core checkWav;
	// #### Reading the file just created to check its header corectness.
	checkWav.readHeader(output_fname);
	checkWav.getInformation();

	system("pause");

	return 0;
}
