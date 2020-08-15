#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>

#include "AudioAnalyzer.cpp"
#include "AudioStream.cpp"
#include "WindowMaker.cpp"

using namespace std;

int main()
{
	string fileName = "Recording (91).wav";

	AudioAnalyzer analyzer;

	//int sampleCount = analyzer.getTrimmedSampleCount(fileName);
	int sampleCount = analyzer.getTrimmedSampleCount(fileName);
	short* samples = new short[sampleCount];
	cout << "sampleCount is " << sampleCount << endl;

	sf::SoundBuffer buffer(analyzer.getBufferAndSamples(fileName, samples)); //samples is passed by reference

	// initialize our custom stream
	AudioStream stream;
	stream.load(buffer);


	cout << "test123456781234567" << endl;
	WindowMaker windowMaker;

	int displaySamplesAmount = sampleCount / 25000;
	short* displaySamples = new short[displaySamplesAmount]; //samples will be displayed in intervals of approx. 100000
	windowMaker.getDisplaySamples(samples, sampleCount, displaySamples); //samples is passed by value, displaySamples is passed by reference





	windowMaker.display(displaySamples, displaySamplesAmount, stream);



	delete[] samples;

	return 0;
}