#include <iostream>
#include <cmath>
#include <vector>
#include <string>

using namespace std;

class RhythmAccuracy {

public:

	RhythmAccuracy() {}

	void setSamples(short* samples)
	{
		this->samples = samples;
	}

	void setSampleInfo(long long sampleCount, int sampleRate, int channelCount)
	{
		this->sampleCount = sampleCount;
		this->sampleRate = sampleRate;
		this->channelCount = channelCount;
	}

	void setBpm(int bpm)
	{
		this->bpm = bpm;
	}

	void setShortestNote(string shortestNote)
	{
		if (shortestNote == "quarter") { this->shortestNote = 1.0; }
		if (shortestNote == "eighth") { this->shortestNote = 0.5; }
		if (shortestNote == "sixteenth") { this->shortestNote = 0.25; }
	}

	// function called in main.cpp
	// returns a vector of the differences between found note location and expected note location for each beat
	// if there is no note in a beat, then INT_MIN is stored for that beat
	vector<double> findBeatDifferences()
	{
		vector<double> beatDifferences;
		vector<int> noteLocations(findNoteLocations());
		vector<int> expectedNoteLocations(findExpectedNoteLocations());

		double samplesPerSecond = sampleRate * channelCount;
		for (int i = 0; i < noteLocations.size(); i++)
		{
			// if there was no note at the expected note location
			if (expectedNoteLocations[i] == INT_MAX)
			{
				// a value of INT_MAX in beatDifferences shows that were was no note
				beatDifferences.push_back(INT_MAX);

				// noteLocations needs to insert INT_MAX at i if there is no note to keep it synced with expectedNoteLocations
				noteLocations.insert(noteLocations.begin() + i, INT_MAX);
			}
			else
			{
				// fill beatDifferences with expectedNoteLocations[i] - noteLocations[i] converted from samples to seconds
				beatDifferences.push_back((expectedNoteLocations[i] / samplesPerSecond) - (noteLocations[i] / samplesPerSecond));
			}
			
		}

		return beatDifferences;
	}

	// returns a vector of the sample index of each note found
	// (sample index can easily be converted to time or beat number)
	vector<int> findNoteLocations()
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

		vector<int> noteLocations;
		vector<short> noteLocationSampleValues; //for debugging

		short newNoteSampleThreshold = 100; //the minimum value a sample can have to be counted as a new note

		//------------------(1) find first note-------------------------------------------

		int currentSampleIndex = 0;
		// the for loop will break before the count reaches sampleCount - trailingEmptySamples 
		// (i.e. the last non zero sample)
		for (int i = 0; i < sampleCount; i++)
		{
			if (abs(samples[i]) > newNoteSampleThreshold)
			{
				// the first note is found when the sample is greater than newNoteSampleThreshold
				// currentSampleIndex is the sample index of the first note
				currentSampleIndex = i;
				noteLocations.push_back(currentSampleIndex);
				noteLocationSampleValues.push_back(samples[currentSampleIndex]);
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
			int waitLength = 0.5 * ((60.0 / bpm) * shortestNote) * (sampleRate * channelCount);
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
							noteLocations.push_back(firstIncreasingAverageLocation);
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
				if (currentSampleIndex + 1000 >= sampleCount)
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

		return noteLocations;
	}

	// returns a vector of the expected (i.e. rhythmically correct) locations for each note in noteLocations
	vector<int> findExpectedNoteLocations()
	{

		vector<int> noteLocations(findNoteLocations());
		vector<int> expectedNoteLocations;

		if (noteLocations.size() == 0)
		{
			return expectedNoteLocations;
		}

		int sampleIndex = noteLocations[0];
		expectedNoteLocations.push_back(sampleIndex);

		// sampleIndexIncrementCount is the number of samples for each beat
		double notesPerSecond = 1 / (bpm / 60.0);
		double sampleIndexIncrementCount = notesPerSecond * shortestNote * sampleRate * channelCount;

		// starting at the sample index of the first note, increment sampleIndex by the number of samples per beat
		// until there is a note that corresponds to the current beat
		// when a corresponding note is found (i.e. the next note in noteLocations), 
		// push it into expectedNoteLocations and end the for loop iteration to begin looking for the next note
		for (int i = 1; i < noteLocations.size(); i++)
		{
			bool noteFound = false;

			while (!noteFound)
			{
				if (sampleIndex + sampleIndexIncrementCount >= sampleCount)
				{
					break;
				}

				sampleIndex += sampleIndexIncrementCount;

				int noteSearchStart = sampleIndex - (0.75 * sampleIndexIncrementCount);
				int noteSearchEnd = sampleIndex + (0.75 * sampleIndexIncrementCount);

				if (noteLocations[i] > noteSearchStart && noteLocations[i] < noteSearchEnd)
				{
					noteFound = true;
					expectedNoteLocations.push_back(sampleIndex);
				}
				else
				{
					// a value of INT_MAX in expectedNoteLocations shows that there was no note
					expectedNoteLocations.push_back(INT_MAX);
				}
			}
		}

		return expectedNoteLocations;
	}



private:

	short* samples;
	long long sampleCount;

	int sampleRate;
	int channelCount;

	int bpm;
	double shortestNote;


};