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

	//make these an input from window later
	string fileName;
	cout << "Enter the file name: ";
	getline(cin, fileName);

	int bpm;
	cout << "Enter the BPM: ";
	cin >> bpm;

	string shortestNote;
	cout << "Enter the shortest note length (quarter/eighth/sixteenth): ";
	cin >> shortestNote;

	AudioAnalyzer analyzer(fileName);

	//int sampleCount = analyzer.getTrimmedSampleCount(fileName);
	int sampleCount = analyzer.getTrimmedSampleCount(fileName);
	//short* samples;// = new short[sampleCount];
	//cout << "sampleCount is " << sampleCount << endl;

	sf::SoundBuffer buffer(analyzer.getBufferAndSamples(fileName)); //samples is passed by reference

	
	RhythmAccuracy rAcc(analyzer.loadRhythmAccuracy(bpm, shortestNote));
	vector<int> noteLocations(rAcc.findNoteLocations());
	vector<int> expectedNoteLocations(rAcc.findExpectedNoteLocations());

	if (noteLocations.size() != expectedNoteLocations.size())
	{
		cout << "ERROR: noteLocations and expectedNoteLocations have different sizes" << endl;
		return 1;
	}

	double samplesPerSecond = analyzer.sampleRate * analyzer.channelCount; //for debugging

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


	cout << "te6gkbbfgldbkjffghh,vvnvfb vdcffvfbhvvffll;fbbjjllcvvnvvvvvvccvcbbcccghhdfffgbhhgfjk,fffbgjmigrrjblfhfd2" << endl;
	WindowMaker windowMaker;

	int displaySamplesAmount = sampleCount / 25000;
	short* displaySamples = new short[displaySamplesAmount]; //samples will be displayed in intervals of approx. 100000
	windowMaker.getDisplaySamples(analyzer.getSamples(), sampleCount, displaySamples); //samples is passed by value, displaySamples is passed by reference

	windowMaker.display(displaySamples, displaySamplesAmount, stream);


	return 0;
}