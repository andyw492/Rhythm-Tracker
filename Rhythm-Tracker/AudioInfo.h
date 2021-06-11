#ifndef AUDIOINFO_H
#define AUDIOINFO_H

#include <vector>

using namespace std;

static class AudioInfo
{
public:

	static long long sampleCount;
	static int sampleRate;
	static int channelCount;

	static int bpm;
	static double shortestNote;

	static vector<int> noteLocations;

	static int samplesBeforeFirstNote;
};

#endif