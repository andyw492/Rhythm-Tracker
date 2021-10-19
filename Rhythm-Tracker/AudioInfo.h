#ifndef AUDIOINFO_H
#define AUDIOINFO_H

#include <SFML/Audio.hpp>
#include <vector>

using namespace std;

class AudioInfo
{
public:

	AudioInfo() {}

	void setBuffer(sf::SoundBuffer buffer)
	{
		this->buffer = buffer;
	}

	void setSampleCount(long long sampleCount)
	{
		this->sampleCount = sampleCount;
	}

	void setSampleRate(int sampleRate)
	{
		this->sampleRate = sampleRate;
	}

	void setChannelCount(int channelCount)
	{
		cout << "set channel count to " << channelCount << endl;;
		this->channelCount = channelCount;
	}

	void setFileName(string fileName)
	{
		this->fileName = fileName;
	}

	void setBpm(int bpm)
	{
		this->bpm = bpm;
	}

	void setInputDevice(int inputDevice)
	{
		this->inputDevice = inputDevice;
	}

	void setShortestNote(double shortestNote)
	{
		this->shortestNote = shortestNote;
	}

	void setNoteLocations(vector<int> noteLocations)
	{
		this->noteLocations = noteLocations;
	}

	void setSamplesBeforeFirstNote(int samplesBeforeFirstNote)
	{
		this->samplesBeforeFirstNote = samplesBeforeFirstNote;
	}

	sf::SoundBuffer getBuffer()
	{
		return buffer;
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

	string getFileName()
	{
		return fileName;
	}

	int getBpm()
	{
		return bpm;
	}

	int getInputDevice()
	{
		return inputDevice;
	}

	double getShortestNote()
	{
		return shortestNote;
	}

	vector<int> getNoteLocations()
	{
		return noteLocations;
	}

	int getSamplesBeforeFirstNote()
	{
		return samplesBeforeFirstNote;
	}

private:

	sf::SoundBuffer buffer;

	long long sampleCount;
	int sampleRate;
	int channelCount;

	string fileName;
	int bpm;
	int inputDevice;
	double shortestNote;

	vector<int> noteLocations;

	int samplesBeforeFirstNote;
};

#endif