#include <SFML/Audio.hpp>

#include <iostream>

#include "common.h"

using namespace std;

class RealTimeRecorder : public sf::SoundRecorder
{
public:

	RealTimeRecorder()
	{
		int maxSamplesLength = getMaxRecordingSeconds() * 44100 * 2;
		totalSamples = new sf::Int16[maxSamplesLength];
	}

	~RealTimeRecorder()
	{
		delete[] totalSamples;
	}

	void setChannelCount(int channelCount)
	{
		this->channelCount = channelCount;
	}

	void setSampleRate(int sampleRate)
	{
		this->sampleRate = sampleRate;
	}

	virtual bool onProcessSamples(const sf::Int16* samples, std::size_t sampleCount)
	{

		cout << "real time recorder sample count: " << sampleCount << endl;
		totalSampleCount += sampleCount;
		copy(samples, samples + sampleCount, totalSamples);

		currentSoundBuffer.loadFromSamples(totalSamples, totalSampleCount, channelCount, sampleRate);

		int nonzerosamples = 0;
		for (int i = 0; i < totalSampleCount; i++)
		{
			if (totalSamples[i] != 0) { nonzerosamples++; }
			cout << "sample value: " << totalSamples[i] << endl;
		}
		cout << "non zero samples: " << nonzerosamples << endl;

		return true;
	}

	sf::SoundBuffer getCurrentBuffer()
	{
		return currentSoundBuffer;
	}

private:

	sf::Int16* totalSamples;
	int totalSampleCount = 0;
	int channelCount;
	int sampleRate;
	sf::SoundBuffer currentSoundBuffer;
};