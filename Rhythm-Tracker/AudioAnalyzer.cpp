#include <SFML/Audio.hpp>
#include <iostream>
#include <queue>

#include "AudioInfo.h"

using namespace std;

// takes in a partially filled in AudioInfo and an sf::SoundBuffer, and
// uses those to fill in the rest of AudioInfo
class AudioAnalyzer
{

public:

	AudioAnalyzer() {}

	// rule of three...?
	~AudioAnalyzer()
	{
		delete[] samples;
	}

	// if audio came from live recording
	void setAudioInfo(AudioInfo& audioInfo, sf::SoundBuffer inputBuffer)
	{
		buffer = inputBuffer;
		audioInfo.setBuffer(buffer);
		setSampleInfo(audioInfo);
		findNoteLocations(audioInfo);
	}

	// if audio came from file
	void setAudioInfo(AudioInfo& audioInfo, string fileName)
	{
		buffer.loadFromFile(fileName);
		audioInfo.setBuffer(buffer);
		setSampleInfo(audioInfo);
		findNoteLocations(audioInfo);
	}

private:

	short* samples;
	sf::SoundBuffer buffer;

	void setSampleInfo(AudioInfo& audioInfo)
	{
		// load an audio buffer from a sound file and get the sample count, sample rate, and channel count
		audioInfo.setSampleCount(buffer.getSampleCount());
		audioInfo.setSampleRate(buffer.getSampleRate());
		audioInfo.setChannelCount(buffer.getChannelCount());

		loadSamples(audioInfo.getSampleCount());
	}

	// fill the local samples array with the audio samples from the SoundBuffer
	void loadSamples(long long sampleCount)
	{
		// array to hold the audio samples
		samples = new short[sampleCount];

		const short* tempSamples = buffer.getSamples();
		for (int i = 0; i < sampleCount; i++)
		{
			//load samples from buffer into samples[]
			samples[i] = tempSamples[i];
		}
	}

	// returns a vector of the sample index of each note found
	// (sample index can easily be converted to time or beat number)
	void findNoteLocations(AudioInfo& audioInfo)
	{
		/*
		algorithm for finding new notes:

		1) find first note by looking for the first sample that is >100

		2) wait 1/2 of the length between the start of the last found note and the expected start of the next note
		(based on bpm and shortest note length)

		3) look for the next note

		4) repeat from (2) until the end of the non zero samples has been reached
		*/

		vector<int> noteLocations;
		long long sampleCount = audioInfo.getSampleCount();
		int bpm = audioInfo.getBpm();
		int sampleRate = audioInfo.getSampleRate();
		int channelCount = audioInfo.getChannelCount();
		double shortestNote = audioInfo.getShortestNote();

		vector<short> noteLocationSampleValues; //for debugging

		short newNoteSampleThreshold = 100; //the minimum value a sample can have to be counted as a new note

		//------------------(1) find first note-------------------------------------------

		int currentSampleIndex = 0;
		// the for loop should break before the count reaches sampleCount
		for (int i = 0; i < audioInfo.getSampleCount(); i++)
		{
			if (abs(samples[i]) > newNoteSampleThreshold)
			{
				// the first note is found when the sample is greater than newNoteSampleThreshold
				// currentSampleIndex is the sample index of the first note
				currentSampleIndex = i;
				noteLocations.push_back(currentSampleIndex);
				noteLocationSampleValues.push_back(samples[currentSampleIndex]);

				// set this value for playing the audio in WindowMaker.cpp
				audioInfo.setSamplesBeforeFirstNote(currentSampleIndex);

				break;
			}
		}

		bool endOfSamplesReached = false;
		int sampleBatchSize = 500;

		while (currentSampleIndex + sampleBatchSize < sampleCount)
		{
			cout << "currentSampleIndex: " << currentSampleIndex << endl;

			//----------------(2) skip distance to next note------------------------------

			// wait 1/2 of the length between the start of the last found note and the 
			// expected start of the next note (based on bpm and shortest note length) using the formula:
			// waitLength = 0.5 * (seconds per beat * shortest note length) * (sample rate * channel count)

			// waitLength is the number of samples to wait
			int waitLength = 0.5 * ((60.0 / bpm) * shortestNote) * (sampleRate * channelCount);
			cout << "bpm: " << bpm << endl;
			cout << "shortestNote: " << shortestNote << endl;
			cout << "sampleRate: " << sampleRate << endl;
			cout << "channelCount: " << channelCount << endl;
			cout << "wait length: " << waitLength << endl;
			currentSampleIndex += waitLength;


			//----------------(3) look for the next note------------------------------------

			bool nextNoteCandidateFound = false;
			queue<int> rejections;
			int rejectionsSize = 5;
			int rejectionFactor = 20;
			int minRejections = INT_MAX;

			int firstIncreasingAverageLocation = 0;
			int increasingAverageCount = 0;
			int increasingAverageCountThreshold = 6;

			double previousAverage = INT_MAX;
			double currentAverage = 0;
			double prev_avg_abs_sampleValue = INT_MAX; 

			if (false)
			{
				// if there are less than 1000 non zero samples remaining (i.e. the next average cant be computed)
				// then the end of the samples has been reached
				if (currentSampleIndex >= sampleCount - 1000)
				{
					endOfSamplesReached = true;
				}

				int sumOfNextThousandSamples = 0;
				for (int i = currentSampleIndex; i < (currentSampleIndex + 1000); i++)
				{
					sumOfNextThousandSamples += abs(samples[i]);
				}
				previousAverage = sumOfNextThousandSamples / 1000.0;
				currentSampleIndex += 1000;

				// a new value is set when the current average >1.5x the previous average and
				// there is no value for firstIncreasingAverageLocation
			}


			//-------------LOOP TO FIND NEXT NOTE------------------

			while (!nextNoteCandidateFound)
			{
				// break from the while loop if the next average cant be computed
				if (currentSampleIndex + sampleBatchSize >= sampleCount) { break; }

				// record the next 1000 samples and find the maximum sample value
				// (1000 samples = approx. 0.01 seconds = 1 / 50 of a quarter note at 120 bpm)
				int sampleBatchSum = 0;
				for (int i = currentSampleIndex; i < (currentSampleIndex + sampleBatchSize); i++)
				{
					sampleBatchSum += abs(samples[i]);
				}
				currentAverage = sampleBatchSum / (double)sampleBatchSize;
				currentSampleIndex += sampleBatchSize;

				// if the currentAverage < 100, then disregard this average and take the next one
				// however, a sample can become a note candidate if 
				// it is [rejectionFactor]x the minimum of the past [rejectionSize] rejections
				if (currentAverage < 100)
				{
					//cout << "rejected currentAverage: " << currentAverage << endl;
					if (rejections.size() >= rejectionsSize) { rejections.pop(); }
					rejections.push(currentAverage);
					minRejections = findMinFromQueue(rejections);
					
					continue;
				}

				if (minRejections != INT_MAX && currentAverage > minRejections * rejectionFactor)
				{
					cout << "minRejections * rejectionFactor: " << (minRejections * rejectionFactor) << endl;
					cout << "current average [" << currentSampleIndex - sampleBatchSize << "-" << currentSampleIndex << "]: " << currentAverage << endl;
					nextNoteCandidateFound = true;
				}

				// we want three consecutive sample averages that are 1.5x an average taken before
				if (currentAverage > (previousAverage * 1.5) || nextNoteCandidateFound)
				{
					increasingAverageCount++;

					if (!firstIncreasingAverageLocation)
					{
						firstIncreasingAverageLocation = currentSampleIndex - sampleBatchSize;
						cout << "current sample index: " << currentSampleIndex << endl;
						cout << "sample batch size: " << sampleBatchSize << endl;
						cout << "firstIncreasingAverageLocation: " << firstIncreasingAverageLocation << endl;
						//currentSampleIndex - 1000 is the location of the first sample in the last group of 1000 samples
					}

					if (increasingAverageCount == increasingAverageCountThreshold) {
						nextNoteCandidateFound = true;
					}

					if(nextNoteCandidateFound)
					{
						cout << "previous average: " << previousAverage << endl;
						cout << "next note candidate: " << firstIncreasingAverageLocation << endl << endl;
						//string s; cin >> s;

						if (currentAverage > newNoteSampleThreshold)
						{
							noteLocations.push_back(firstIncreasingAverageLocation);
							noteLocationSampleValues.push_back(samples[firstIncreasingAverageLocation]); //for debugging
						}
					}

				}
				else // currentAverage was not high enough, so reset the increasing average count
				{
					firstIncreasingAverageLocation = 0;
					increasingAverageCount = 0;
					previousAverage = currentAverage;
				}



			}

			//4) repeat from (2) until the end of the non zero samples has been reached
		}

		for (int i = 0; i < noteLocations.size(); i++)
		{
			//cout << "noteLocations " << i << ": " << AudioInfo::noteLocations[i] << endl;
		}

		// push the found note locations into audioInfo
		audioInfo.setNoteLocations(noteLocations);

	}

	// used for finding note locations
	int findMinFromQueue(queue<int> q)
	{
		queue<int> tempQueue;
		int min = INT_MAX;
		int popped = -1;

		if (q.size() == 0) { return INT_MAX; }

		for (int i = 0; i < q.size(); i++)
		{
			popped = q.front();
			q.pop();
			if (popped < min) { min = popped; }

			tempQueue.push(popped);
		}

		for (int i = 0; i < tempQueue.size(); i++)
		{

		}

		return min;
	}
};