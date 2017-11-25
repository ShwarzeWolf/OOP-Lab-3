#include <iostream>

#include "wav_core.h"

using namespace std;

int main()
{
	cout << "************** | WavCore | **************" << endl;


	// ################  Tests for WavCore  ################

	const char* input_fname = "wavSample.wav";
	const char* output_fname = "out.wav";

	wav_errors_e err;
	wav_core wav;


	// #### Opening WAV file, checking header.
	err = wav.readHeader(input_fname);
	if (err != WAV_OK) {
		cerr << "read_header() error: " << (int)err << endl;
		wav.getInformation();
		return err;
	}


	// #### Printing header.
	wav.getInformation();


	// #### Reading PCM data from file.
	vector< vector<short> > chans_data;

	err = wav.readDate(input_fname, chans_data);
	if (err != WAV_OK) {
		cerr << "extract_data_int16() error: " << (int)err << endl;
		return err;
	}
	cout << endl << "********************" << endl;

	
	// #### Make several changes to PCM data.

	// # Making signal mono from stereo.
	vector< vector<short> > edited_data;
	err = wav.makeMono(chans_data, edited_data);
	if (err != WAV_OK) {
		cerr << "make_mono() error: " << (int)err << endl;
		return err;
	}
	
	// # Add a reverberation
	//wav.make_reverb(edited_data, wav.getSampleRate(), 0.5, 0.6f);
	
	wav_core newWav;

	// #### Making new WAV file using edited PCM data.
	err = newWav.createWav(output_fname, wav.getSampleRate(), edited_data);
	if (err != WAV_OK) {
		cerr << "make_wav_file() error: " << (int)err << endl;
		newWav.getInformation();
		return err;
	}

	wav_core checkWav;
	// #### Reading the file just created to check its header corectness.
	err = checkWav.readHeader(output_fname);
	if (err != WAV_OK) {
		cerr << "read_header() error: " << (int)err << endl;
		checkWav.getInformation();
		return err;
	}
	checkWav.getInformation();

	system("pause");

	return 0;
}
