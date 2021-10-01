#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>

#include "AudioAnalyzer.cpp"
#include "RhythmAccuracy.cpp"
#include "WindowMaker.cpp"
#include "AudioInfo.h"
//#include "Settings.h"

using namespace std;

long long AudioInfo::sampleCount = -1;
int AudioInfo::sampleRate = -1;
int AudioInfo::channelCount = -1;

int AudioInfo::bpm = -1;
double AudioInfo::shortestNote = -1;

vector<int> AudioInfo::noteLocations;

int AudioInfo::samplesBeforeFirstNote = -1;

// TODO: 
// make fileName, bpm, and shortestNote user input
// add start screen
// add settings
// add try catch blocks for edge cases/exception handling

int main()
{
	bool record = false;

	sf::SoundBuffer buffer;

	if (record)
	{
		std::vector<std::string> availableDevices = sf::SoundRecorder::getAvailableDevices();

		for (int i = 0; i < availableDevices.size(); i++)
		{
			cout << "available devices: " << availableDevices[i] << endl;
		}

		cout << "which device?" << endl;
		int deviceNum = 0; cin >> deviceNum;

		// choose a device
		std::string inputDevice = availableDevices[deviceNum];

		// create the recorder
		sf::SoundBufferRecorder recorder;

		// set the device
		if (!recorder.setDevice(inputDevice))
		{
			cout << "couldn't set device" << endl;
		}

		// start the capture
		recorder.start();

		// wait...
		string s; cin >> s;

		// stop the capture
		recorder.stop();

		// retrieve the buffer that contains the captured audio data
		const sf::SoundBuffer& receivedBuffer = recorder.getBuffer();

		buffer = sf::SoundBuffer(receivedBuffer);
	}



	string searchSetting = "Max Rectangle Height";

	fstream settingsFile("Rhythm-Tracker/Settings.txt");
/*
	cout << settingsFile.is_open() << endl;

	bool foundString = false;
	int value = -1;

	while (!foundString && !settingsFile.eof())
	{
		string fileLine;
		getline(settingsFile, fileLine);
		cout << fileLine << endl;

		value++;
		if (value == 10)
		{
			while (true) {}
		}

		if (fileLine.find(searchSetting) != string::npos)
		{
			printf("found the search setting\n");
			value = stoi(fileLine.substr(fileLine.find(":") + 1, fileLine.find("\n")));
			foundString = true;
		}
	}

	if (!foundString)
	{
		printf("ERROR: setting not found");
	}

	settingsFile.close();*/

	cout << "Please make sure the audio file is a .wav file" << endl;

	string fileName;
	cout << "Enter the file name: ";
	fileName = "Recording (227).wav"; cout << endl;

	cout << "Enter the BPM: ";
	AudioInfo::bpm = 120; cout << endl;

	string shortestNoteString;
	cout << "Enter the shortest note length (quarter/eighth/sixteenth): ";
	shortestNoteString = "eighth"; cout << endl;

	// empty string for fileName to specify that we are recording live
	AudioAnalyzer analyzer(fileName);
	if (record) { analyzer.setBuffer(buffer); }
	analyzer.setAudioInfo(AudioInfo::bpm, shortestNoteString);
	analyzer.setSampleInfo();
	analyzer.findNoteLocations();

	if (!record) { buffer = analyzer.getBuffer(fileName); }

	//// get the buffer for the audio to be played back to the user later
	//sf::SoundBuffer buffer(analyzer.getBuffer(fileName));
	
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

	// for some reason, a change needs to be made somewhere in main.cpp for each new compile
	string compileChange = "q7777877777877777777777777777777777777777777777777777788877887777777777";

	cout << sf::SoundBufferRecorder::isAvailable() << endl;

	WindowMaker windowMaker;
	windowMaker.display(beatDifferences, buffer);
	

	return 0;
}