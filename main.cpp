#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>

#include "AudioAnalyzer.cpp"
#include "AudioStream.cpp"
#include "WindowMaker.cpp"

using namespace std;

int main()
{
	//make these an input from window later
	int bpm = 150; 
	string shortestNote = "eighth";

	string fileName = "Recording (99).wav";

	AudioAnalyzer analyzer(fileName);

	//int sampleCount = analyzer.getTrimmedSampleCount(fileName);
	int sampleCount = analyzer.getTrimmedSampleCount(fileName);
	//short* samples;// = new short[sampleCount];
	cout << "sampleCount is " << sampleCount << endl;

	sf::SoundBuffer buffer(analyzer.getBufferAndSamples(fileName)); //samples is passed by reference

	
	analyzer.loadRhythmAccuracy(bpm, shortestNote);

	// initialize our custom stream
	AudioStream stream;
	stream.load(buffer);


	cout << "te6gkbbfgldbkjffgghhgfjk,gjmigrrjbfhfd2" << endl;
	WindowMaker windowMaker;

	int displaySamplesAmount = sampleCount / 25000;
	short* displaySamples = new short[displaySamplesAmount]; //samples will be displayed in intervals of approx. 100000
	windowMaker.getDisplaySamples(analyzer.getSamples(), sampleCount, displaySamples); //samples is passed by value, displaySamples is passed by reference





	windowMaker.display(displaySamples, displaySamplesAmount, stream);


	return 0;
}