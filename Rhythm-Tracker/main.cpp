#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>

#include "AudioAnalyzer.cpp"
#include "AudioStream.cpp"
#include "RhythmAccuracy.cpp"
#include "WindowMaker.cpp"
#include "AudioInfo.h"
#include "Settings.h"

using namespace std;

long long AudioInfo::sampleCount = -1;
int AudioInfo::sampleRate = -1;
int AudioInfo::channelCount = -1;

int AudioInfo::bpm = -1;
double AudioInfo::shortestNote = -1;

vector<int> AudioInfo::noteLocations;

int main()
{
	cout << "Please make sure the audio file is a .wav file" << endl;

	// TODO: make fileName, bpm, and shortestNote user input
	string fileName;
	cout << "Enter the file name: ";
	fileName = "Recording (150).wav"; cout << endl;

	cout << "Enter the BPM: ";
	AudioInfo::bpm = 120; cout << endl;

	string shortestNoteString;
	cout << "Enter the shortest note length (quarter/eighth/sixteenth): ";
	shortestNoteString = "eighth"; cout << endl;

	AudioAnalyzer analyzer(fileName);
	analyzer.setAudioInfo(AudioInfo::bpm, shortestNoteString);
	analyzer.setSampleInfo();
	analyzer.findNoteLocations();


	// get the buffer for the audio to be played back to the user later
	sf::SoundBuffer buffer(analyzer.getBuffer(fileName));

	
	RhythmAccuracy rAcc;
	vector<double> beatDifferences(rAcc.findBeatDifferences());
	
	// debugging
	for (int i = 0; i < beatDifferences.size(); i++)
	{
		if (beatDifferences[i] == INT_MAX)
		{
			cout << "there was no note at beat " << i << endl;

		}
		else
		{
			// beat differences converted from seconds to beats
			cout << "beat " << i << " is " << beatDifferences[i] << " beats off" << endl;
		}

	}

	// initialize our custom stream
	AudioStream stream;
	stream.load(buffer);

	// for some reason, a change needs to be made somewhere in main.cpp for each new compile
	string compileChange = "q777787777777777777777777777777777777777777777777777777777778777777777777";

	WindowMaker windowMaker;

	windowMaker.display(beatDifferences, stream);
	

	return 0;
}