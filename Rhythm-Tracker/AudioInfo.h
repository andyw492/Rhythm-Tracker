#ifndef AUDIOINFO_H
#define AUDIOINFO_H

#include <vector>

using namespace std;

class AudioInfo
{
public:

	AudioInfo() {}

	AudioInfo(const AudioInfo &audioInfo)
	{
		sampleCount = audioInfo.sampleCount;
		sampleRate = audioInfo.sampleRate;
		channelCount = audioInfo.channelCount;
		bpm = audioInfo.bpm;
		shortestNote = audioInfo.shortestNote;
		noteLocations = audioInfo.noteLocations;
		samplesBeforeFirstNote = audioInfo.samplesBeforeFirstNote;
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
		this->channelCount = channelCount;
	}

	void setBpm(int bpm)
	{
		this->bpm = bpm;
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

	int getBpm()
	{
		return bpm;
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

	long long sampleCount;
	int sampleRate;
	int channelCount;

	int bpm;
	double shortestNote;

	vector<int> noteLocations;

	int samplesBeforeFirstNote;
};

#endif