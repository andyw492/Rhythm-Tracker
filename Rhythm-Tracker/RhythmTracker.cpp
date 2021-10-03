#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
#include <thread>

#include "WindowMaker.cpp"
#include "AudioInfo.h"

using namespace std;

class RhythmTracker
{
public:

	RhythmTracker()
	{
	}

	void playMetronome(AudioStream &stream, int bpm)
	{
		stream.play();
	}

	void stopMetronome(AudioStream &stream)
	{
		stream.stop();
	}

	bool enableTesting = false;
	bool enableTesting2 = false;
	bool testing()
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

			return true;

		}

		if (enableTesting2)
		{
			return true;
		}

		return false;
	}



	void loadBeatDifferences()
	{

	}

	void displayWindow()
	{
		WindowMaker windowMaker;
		windowMaker.createWindow();
	}

private:

	AudioInfo audioInfo;
	RhythmAccuracy rhythmAccuracy;
	sf::SoundBuffer buffer;
	vector<double> beatDifferences;

};