#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>

#include "AudioAnalyzer.cpp"
#include "AudioStream.cpp"
#include "WindowMaker.cpp"

using namespace std;

int main()
{
	cout << "Please make sure the audio file is a .wav file" << endl;

	// TODO: make fileName, bpm, and shortestNote user input
	string fileName;
	cout << "Enter the file name: ";
	fileName = "Recording (99).wav"; cout << endl;

	int bpm;
	cout << "Enter the BPM: ";
	bpm = 150; cout << endl;

	string shortestNote;
	cout << "Enter the shortest note length (quarter/eighth/sixteenth): ";
	shortestNote = "eighth"; cout << endl;

	AudioAnalyzer analyzer(fileName);

	int sampleCount = analyzer.getSampleCount();
	cout << "main.cpp: sampleCount is " << sampleCount << endl;

	// get the buffer for the audio to be played back to the user later
	sf::SoundBuffer buffer(analyzer.getBuffer(fileName));

	
	RhythmAccuracy rAcc(analyzer.loadRhythmAccuracy(bpm, shortestNote));
	vector<int> noteLocations(rAcc.findNoteLocations());
	vector<int> expectedNoteLocations(rAcc.findExpectedNoteLocations());


	if (noteLocations.size() != expectedNoteLocations.size())
	{
		cout << "ERROR: noteLocations and expectedNoteLocations have different sizes" << endl;
		return 1;
	}

	double samplesPerSecond = analyzer.getSampleRate() * analyzer.getChannelCount(); //for debugging

	vector<double> timeDifference;

	for (int i = 0; i < noteLocations.size(); i++)
	{
		//cout << "note " << i << ": expected at " << expectedNoteLocations[i] / samplesPerSecond << " and found at " << noteLocations[i] / samplesPerSecond << endl;
		timeDifference.push_back((expectedNoteLocations[i] / samplesPerSecond) - (noteLocations[i] / samplesPerSecond));
	}

	cout << endl;

	for (int i = 0; i < timeDifference.size(); i++)
	{
		cout << "note " << i << " is " << timeDifference[i] * (150 / 60.0) << " beats off" << endl;
	}

	
	while(true){}

	// initialize our custom stream
	AudioStream stream;
	stream.load(buffer);

	// for some reason, a change needs to be made somewhere in main.cpp for each new compile
	string compileChange = "q8";
	
	WindowMaker windowMaker;

	int displaySamplesAmount = sampleCount / 25000;
	short* displaySamples = new short[displaySamplesAmount]; //samples will be displayed in intervals of approx. 100000
	windowMaker.getDisplaySamples(analyzer.getSamples(), sampleCount, displaySamples); //samples is passed by value, displaySamples is passed by reference

	windowMaker.display(displaySamples, displaySamplesAmount, stream);


	return 0;
}