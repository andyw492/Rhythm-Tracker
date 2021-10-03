#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
#include <thread>

#include "AudioAnalyzer.cpp"
#include "RhythmAccuracy.cpp"
#include "WindowMaker.cpp"
#include "AudioInfo.h"

using namespace std;

class RhythmTracker
{
public:

	RhythmTracker(){}

	void playMetronome(AudioStream &stream, int bpm)
	{
		stream.play();
	}

	void stopMetronome(AudioStream &stream)
	{
		stream.stop();
	}

	void testing(bool enableTesting, bool enableTesting2)
	{
		if (enableTesting)
		{
			sf::Clock clock;
			sf::Clock clock2;

			sf::SoundBuffer test_mainBuffer;
			test_mainBuffer.loadFromFile("Recording (228).wav");

			AudioStream test_mainStream;
			test_mainStream.load(test_mainBuffer);
			test_mainStream.play();

			sf::SoundBuffer test_buffer;
			test_buffer.loadFromFile("metronome.ogg");

			AudioStream test_stream;
			test_stream.load(test_buffer);
			playMetronome(test_stream, 120);

			while (test_stream.getStatus() != sf::SoundSource::Status::Stopped)
			{
				sf::Time audioDuration2 = test_stream.getBuffer().getDuration();
				if (clock2.getElapsedTime() == audioDuration2)
				{
					stopMetronome(test_stream);
				}
			}

			while (test_mainStream.getStatus() != sf::SoundSource::Status::Stopped)
			{
				sf::Time audioDuration = test_mainStream.getBuffer().getDuration();
				if (clock.getElapsedTime() == audioDuration)
				{
					test_mainStream.stop();
				}
			}

			return;

		}

		if (enableTesting2)
		{
			// (e.g. for 10/2/2021 7:30pm -> d20211002t1930)
			return;
		}
	}

	// temporary; do this in WindowMaker.cpp later
	void loadSoundBuffer(bool record, bool fromFile, string fileName, int bpm, string shortestNoteString)
	{
		audioInfo.setBpm(120);
		if (shortestNoteString == "quarter") { audioInfo.setShortestNote(1.0); }
		if (shortestNoteString == "eighth") { audioInfo.setShortestNote(0.5); }
		if (shortestNoteString == "sixteenth") { audioInfo.setShortestNote(0.25); }

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

		if (record) { fileName = ""; }
		AudioAnalyzer analyzer(fileName);
		if (record)
		{
			analyzer.setBuffer(buffer);
		}
		else
		{
			buffer = analyzer.getBuffer(fileName);
		}

		// audioInfo passed by reference
		analyzer.setAudioInfo(audioInfo);
	}

	void loadBeatDifferences()
	{
		RhythmAccuracy rAcc(audioInfo);
		beatDifferences = rAcc.findBeatDifferences();

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
	}

	void displayWindow()
	{
		WindowMaker windowMaker(audioInfo);
		windowMaker.display(beatDifferences, buffer);
	}

private:

	AudioInfo audioInfo;
	sf::SoundBuffer buffer;
	vector<double> beatDifferences;

};