#include <SFML/Audio.hpp>
#include <iostream>

#include "AudioInfo.h"

using namespace std;

class AudioAnalyzer
{


public:

	AudioAnalyzer() {}

	AudioAnalyzer(string fileName)
	{
		this->fileName = fileName;

	}

	~AudioAnalyzer()
	{
		delete[] samples;
	}

	// set buffer if recording live (not from file)
	void setBuffer(sf::SoundBuffer inputBuffer)
	{
		buffer = inputBuffer;
	}

	void setAudioInfo(int inputtedBpm, string inputtedShortestNote)
	{
		AudioInfo::bpm = inputtedBpm;

		if (inputtedShortestNote == "quarter") { AudioInfo::shortestNote = 1.0; }
		if (inputtedShortestNote == "eighth") { AudioInfo::shortestNote = 0.5; }
		if (inputtedShortestNote == "sixteenth") { AudioInfo::shortestNote = 0.25; }
	}

	void setSampleInfo()
	{
		// load an audio buffer from a sound file and get the sample count, sample rate, and channel count
		if (fileName.length() > 0) { buffer.loadFromFile(fileName); }
		AudioInfo::sampleCount = buffer.getSampleCount();
		AudioInfo::sampleRate = buffer.getSampleRate();
		AudioInfo::channelCount = buffer.getChannelCount();

		loadSamples();
	}

	short* getSamples()
	{
		return samples;
	}

	long long getSampleCount()
	{
		return AudioInfo::sampleCount;
	}

	int getSampleRate()
	{
		return AudioInfo::sampleRate;
	}

	int getChannelCount()
	{
		return AudioInfo::channelCount;
	}

	sf::SoundBuffer getBuffer(string fileName)
	{
		buffer.loadFromFile(fileName);

		return buffer;
	}


	// returns a vector of the sample index of each note found
	// (sample index can easily be converted to time or beat number)
	void findNoteLocations()
	{

		/*
		algorithm for finding new notes:

		1) find first note by looking for the first sample that is >100

		2) wait 1/2 of the length between the start of the last found note and the expected start of the next note
		(based on bpm and shortest note length)

		3) look for the next note:

			3a) record the next 1000 samples and find the average of those samples' absolute values
			(1000 samples = approx. 0.01 seconds = approx. 1/50 of a quarter note at 120 bpm)

			3b) if the current average is >5x the previous average AND is >100
					mark the previous average value as a previousAverage
					mark the current average value location (the first of the 1000 samples)
					as an averageSpikeLocation and go to (3c)

					record the next two 1000 sample averages
					if both of those averages are each >50x the previousAverage value
						mark the averageSpikeLocation as a newNoteLocation
					else
						go to (3a)
				else
					go to (3a)



		4) repeat from (2) until the end of the non zero samples has been reached
		*/


		vector<short> noteLocationSampleValues; //for debugging

		short newNoteSampleThreshold = 100; //the minimum value a sample can have to be counted as a new note

		//------------------(1) find first note-------------------------------------------

		int currentSampleIndex = 0;
		// the for loop will break before the count reaches sampleCount
		for (int i = 0; i < AudioInfo::sampleCount; i++)
		{
			if (abs(samples[i]) > newNoteSampleThreshold)
			{
				// the first note is found when the sample is greater than newNoteSampleThreshold
				// currentSampleIndex is the sample index of the first note
				currentSampleIndex = i;
				AudioInfo::noteLocations.push_back(currentSampleIndex);
				noteLocationSampleValues.push_back(samples[currentSampleIndex]);

				// set this value for playing the audio in WindowMaker.cpp
				AudioInfo::samplesBeforeFirstNote = currentSampleIndex;

				break;
			}
		}

		bool endOfSamplesReached = false;

		while (!endOfSamplesReached)
		{

			//----------------(2) skip distance to next note------------------------------

			// wait 1/2 of the length between the start of the last found note and the 
			// expected start of the next note (based on bpm and shortest note length) using the formula:
			// waitLength = 0.5 * (seconds per beat * shortest note length) * (sample rate * channel count)

			// waitLength is the number of samples to wait
			int waitLength = 0.5 * ((60.0 / AudioInfo::bpm) * AudioInfo::shortestNote) * (AudioInfo::sampleRate * AudioInfo::channelCount);
			currentSampleIndex += waitLength;


			//----------------(3) look for the next note------------------------------------

			bool nextNoteCandidateFound = false;

			int firstIncreasingAverageLocation = 0;
			int increasingAverageCount = 0;
			int increasingAverageCountThreshold = 3;

			double previousAverage = 0;
			double currentAverage = 0;
			double prev_avg_abs_sampleValue = INT_MAX; //set to INT_MAX so that the first 1000 samples
													   //isnt automatically interpreted as an increase

			// 3a) record the next 1000 samples and find the maximum sample value
			// (1000 samples = approx. 0.01 seconds = 1 / 50 of a quarter note at 120 bpm)


			// 3b) find the sample at location x, where x is the location of the start of
			//	   three consecutive increases of sample averages by a factor of 1.5
			//
			//      if the current average is >5x the previous average
			// 	    mark the previous average value as a previousAverage
			//		mark the current average value location(the first of the 1000 samples)
			//		as a firstIncreasingAverageLocation and go to(3c)
			//
			//		record the next two 1000 sample averages
			//		if both of those averages are each >50x the previousAverage value AND
			//		the sample value at firstIncreasingAverageLocation is >100
			//			mark the firstIncreasingAverageLocation as a newNoteLocation
			//		else
			//			go to (3a)
			//	else
			//		go to(3a)


			// if there are less than 1000 non zero samples remaining (i.e. the next average cant be computed)
			// then the end of the samples has been reached
			if (currentSampleIndex >= AudioInfo::sampleCount - 1000)
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

			// exit the while loop of the next note candidate was found or the end of the samples was reached
			while (!nextNoteCandidateFound && !endOfSamplesReached)
			{
				// (3a)
				int sumOfNextThousandSamples = 0;
				for (int i = currentSampleIndex; i < (currentSampleIndex + 1000); i++)
				{
					sumOfNextThousandSamples += abs(samples[i]);
				}
				currentAverage = sumOfNextThousandSamples / 1000.0;
				currentSampleIndex += 1000;

				// (3b)
				if (currentAverage > (previousAverage * 1.5))
				{
					increasingAverageCount++;

					if (!firstIncreasingAverageLocation)
					{
						firstIncreasingAverageLocation = currentSampleIndex - 1000;
						//currentSampleIndex - 1000 is the location of the first sample in the last group of 1000 samples
					}

					if (increasingAverageCount == increasingAverageCountThreshold)
					{
						nextNoteCandidateFound = true;

						if (currentAverage > newNoteSampleThreshold)
						{
							AudioInfo::noteLocations.push_back(firstIncreasingAverageLocation);
							noteLocationSampleValues.push_back(samples[firstIncreasingAverageLocation]); //for debugging
						}
					}

				}
				else // reset the increasing average count
				{
					firstIncreasingAverageLocation = 0;
					increasingAverageCount = 0;
				}

				//if there are less than 1000 non zero samples remaining
				//i.e. the next average cant be computed
				if (currentSampleIndex + 1000 >= AudioInfo::sampleCount)
				{
					endOfSamplesReached = true;
				}

				if (increasingAverageCount == 0)
				{

					previousAverage = currentAverage;
				}


			}

			//4) repeat from (2) until the end of the non zero samples has been reached
		}

	}

private:

	short* samples;

	sf::SoundBuffer buffer;

	string fileName;

	// fill the local samples array with the audio samples from the SoundBuffer
	void loadSamples()
	{
		// array to hold the audio samples
		samples = new short[AudioInfo::sampleCount];

		const short* tempSamples = buffer.getSamples();
		for (int i = 0; i < AudioInfo::sampleCount; i++)
		{
			//load samples from buffer into samples[]
			samples[i] = tempSamples[i];
		}
	}

};