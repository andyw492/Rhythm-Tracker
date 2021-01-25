#include <SFML/Audio.hpp>
#include <iostream>
#include "RhythmAccuracy.cpp"

using namespace std;

class AudioAnalyzer
{


public:

	AudioAnalyzer() {}

	AudioAnalyzer(string fileName)
	{
		this->fileName = fileName;
		setSampleInfo(fileName); // get sample count, sample rate, and channel count

		// array to hold the audio samples
		samples = new short[sampleCount];
		loadSamples();
	}

	~AudioAnalyzer()
	{
		delete[] samples;
	}

	void setSampleInfo(string fileName)
	{
		// load an audio buffer from a sound file and get the sample count, sample rate, and channel count
		buffer.loadFromFile(fileName);
		sampleCount = buffer.getSampleCount();
		sampleRate = buffer.getSampleRate();
		channelCount = buffer.getChannelCount();
	}

	short* getSamples()
	{
		return samples;
	}

	long long getSampleCount()
	{
		return sampleCount;
	}

	int getSampleRate()
	{
		return sampleRate;
	}

	int getChannelCount()
	{
		return channelCount;
	}

	sf::SoundBuffer getBuffer(string fileName)
	{
		buffer.loadFromFile(fileName);

		return buffer;
	}



	RhythmAccuracy loadRhythmAccuracy(int bpm, string shortestNote)
	{
		RhythmAccuracy rAcc;
		rAcc.setSamples(getSamples());
		rAcc.setSampleInfo(sampleCount, sampleRate, channelCount);
		rAcc.setBpm(bpm);
		rAcc.setShortestNote(shortestNote);

		return rAcc;

	}

private:

	short* samples;
	long long sampleCount;
	int sampleRate;
	int channelCount;

	sf::SoundBuffer buffer;

	string fileName;
	int leadingEmptySamples;
	int trailingEmptySamples;
	int trimmedSampleCount;

	// fill the local samples array with the audio samples from the SoundBuffer
	void loadSamples()
	{
		const short* tempSamples = buffer.getSamples();
		for (int i = 0; i < sampleCount; i++)
		{
			//load samples from buffer into samples[]
			samples[i] = tempSamples[i];
		}
	}

};