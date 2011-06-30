#include "preferences.h"

using namespace std;

Preferences::Preferences(){
	// set defaults
	temporalWindow = 'm';			// PARAMETERISED
	spectrumAnalyser = 'f';		// PARAMETERISED
	fftPostProcessor = 'i';		// PARAMETERISED
	directSkWindow = 'n';			// PROBABLY FINAL
	hopSize = 16384;					// PROBABLY FINAL
	fftFrameSize = 65536;			// PROBABLY FINAL, unless accuracy goes up
	goertzelMinK = 30;				// PROBABLY FINAL
	octaves = 6;							// FINAL
	bps = 1;									// This may have value at some point
	dFactor = 10;							// PROBABLY FINAL
	toneProfile = 2;					// PARAMETERISED
	stFreq = 27.5;						// FINAL
	directSkStretch = 1.3;		// Some experimentation to do, alongside normalisation
	generateBinFreqs();
}

void Preferences::setSpectrumAnalyser(char c){
	if (c == 'f' || c == 'g')
		spectrumAnalyser = c;
}

void Preferences::setFftPostProcessor(char c){
	if (c == 'i' || c == 'c' || c == 'l')
		fftPostProcessor = c;
}

void Preferences::setTemporalWindow(char c){
	if (c == 'm' || c == 'n' || c == 'b')
		temporalWindow = c;
}

void Preferences::setToneProfile(int n){
	if(n >= 0 && n <= 2)
		toneProfile = n;
}

char Preferences::getSpectrumAnalyser()const{return spectrumAnalyser;}
char Preferences::getFftPostProcessor()const{return fftPostProcessor;}
char Preferences::getTemporalWindow()const{return temporalWindow;}
char Preferences::getDirectSkWindow()const{return directSkWindow;}
int Preferences::getHopSize()const{return hopSize;}
int Preferences::getFftFrameSize()const{return fftFrameSize;}
int Preferences::getGoertzelMinK()const{return goertzelMinK;}
int Preferences::getOctaves()const{return octaves;}
int Preferences::getBpo()const{return bps * 12;}
int Preferences::getDFactor()const{return dFactor;}
int Preferences::getToneProfile()const{return toneProfile;}
float Preferences::getBinFreq(int n)const{return binFreqs[n];}
float Preferences::getDirectSkStretch()const{return directSkStretch;}

/*
void Preferences::printOptions(char* argv[])const{
	cerr << endl << "Usage: " << argv[0] << " [options] infile" << endl;
	cerr << "where options are:" << endl;
	cerr << "Force a particular decoder:" << endl;
	cerr << " -c s	: libsndfile (Default for WAV, AIFF, FLAC)" << endl;
	cerr << " -c a	: libavcodec (Default for all other file types)" << endl;
	cerr << "Force a particular downsampler:" << endl;
	cerr << " -d i	: Decimator with hardcoded coefficients (Default for 44100Hz, df 10)" << endl;
	cerr << " -d s	: libsamplerate (Default for all other samplerates and factors)" << endl;
	cerr << " -d d	: Sample discarder (aliases badly)" << endl;
	cerr << "Choice of temporal window function:" << endl;
	cerr << " -w m	: Hamming (Default)" << endl;
	cerr << " -w n	: Hann" << endl;
	cerr << " -w b	: Blackman" << endl;
	cerr << "Choice of transform algorithm:" << endl;
	cerr << " -t f	: FFTW (Default)" << endl;
	cerr << " -t g	: Goertzel" << endl;
	cerr << "Choice of FFT post-processor:" << endl;
	cerr << " -p i	: Direct kernel (Default)" << endl;
	cerr << " -p c	: Constant Q" << endl;
	cerr << " -p l	: Individual logarithmically-spaced bins" << endl;
	cerr << "Configuration options (defaults in parens):" << endl;
	cerr << " -h N	: Hop size (16384)" << endl;
	cerr << " -f N	: FFT frame size (32768)" << endl;
	cerr << " -k N	: Goertzel minimum k (20)" << endl;
	cerr << " -s N	: Starting frequency in Hz (27.5)" << endl;
	cerr << " -o N	: Octaves (6)" << endl;
	cerr << " -b N	: Bands per semitone (1)" << endl;
	cerr << " -df N	: Downsample factor (10)" << endl;
	cerr << " -ks N	: Direct kernel QStretch (1.0)" << endl;
	cerr << " -kw X	: Direct kernel window (Hann)" << endl;
}

void Preferences::procArgs(int argc, char* argv[]){
	// load args
	for(int k=1; k<argc-1; k++){
		// chars
		if(strcmp(argv[k],"-c")==0)
			decoder = *argv[k+1];							// lose this; it can be automatic
		if(strcmp(argv[k],"-d")==0)
			downsampler = *argv[k+1];					// likewise
		if(strcmp(argv[k],"-kw")==0)
			directSkWindow = *argv[k+1];			// experiment but no need to parameterise once a winner's chosen
		// ints
		if(strcmp(argv[k],"-h")==0)
			hopSize = atoi(argv[k+1]);				// No need.
		if(strcmp(argv[k],"-f")==0)
			fftFrameSize = atoi(argv[k+1]);		// Hopefully algo improvements will make this moot at 32768 or lower
		if(strcmp(argv[k],"-k")==0)
			goertzelMinK = atoi(argv[k+1]);		// No need.
		if(strcmp(argv[k],"-o")==0)
			octaves = atoi(argv[k+1]);				// No need.
		if(strcmp(argv[k],"-b")==0){
			int newBps = atoi(argv[k+1]);			// Haven't gotten this working properly yet, once I do it'll be on all the time.
			if(newBps%2 == 1) bps = newBps;
			else cerr << "ERROR: -b must be an odd number. Using default." << endl;
		}
		if(strcmp(argv[k],"-df")==0)
			dFactor = atoi(argv[k+1]);				// No need.
		// floats
		if(strcmp(argv[k],"-s")==0)
			stFreq = atof(argv[k+1]);					// No need.
		if(strcmp(argv[k],"-ks")==0)
			directSkStretch = atof(argv[k+1]);		// Experiments continue.
	}
	setBinFreqs();
}
*/

void Preferences::generateBinFreqs(){
	int bpo = bps * 12;
	binFreqs = std::vector<float>(octaves*bpo);
	float freqRatio = pow(2,(1.0/bpo));
	float octFreq = stFreq;
	float binFreq;
	int concertPitchBin = bps/2;
	for(int i=0; i<octaves; i++){
		binFreq = octFreq;
		// tune down for bins before first concert pitch bin (if bps > 1)
		for(int j=0; j<concertPitchBin; j++){
			binFreqs[(i*bpo)+j] = binFreq / pow(freqRatio,(concertPitchBin-j));
		}
		for(int j=concertPitchBin; j<bpo; j++){
			binFreqs[(i*bpo)+j] = binFreq;
			binFreq *= freqRatio;
		}
		octFreq *= 2;
	}
}
