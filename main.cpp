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
	fileName = "Recording (150).wav"; cout << endl;

	int bpm;
	cout << "Enter the BPM: ";
	bpm = 120; cout << endl;

	string shortestNote;
	cout << "Enter the shortest note length (quarter/eighth/sixteenth): ";
	shortestNote = "eighth"; cout << endl;

	AudioAnalyzer analyzer(fileName);

	int sampleCount = analyzer.getSampleCount();
	cout << "main.cpp: sampleCount is " << sampleCount << endl;

	// get the buffer for the audio to be played back to the user later
	sf::SoundBuffer buffer(analyzer.getBuffer(fileName));

	
	RhythmAccuracy rAcc(analyzer.loadRhythmAccuracy(bpm, shortestNote));
	vector<double> beatDifferences(rAcc.findBeatDifferences());

	for (int i = 0; i < beatDifferences.size(); i++)
	{
		if (beatDifferences[i] == INT_MAX)
		{
			cout << "there was no note at beat " << i << endl;

		}
		else
		{
			// beat differences converted from seconds to beats
			cout << "beat " << i << " is " << beatDifferences[i] * (120 / 60.0) << " beats off" << endl;
		}

	}



	double samplesPerSecond = analyzer.getSampleRate() * analyzer.getChannelCount(); //for debugging

	vector<double> beatDifference;



	// initialize our custom stream
	AudioStream stream;
	stream.load(buffer);

	// for some reason, a change needs to be made somewhere in main.cpp for each new compile
	string compileChange = "q77778777778777777777777";
	
	WindowMaker windowMaker;

	windowMaker.display(beatDifferences, stream);


	return 0;
}