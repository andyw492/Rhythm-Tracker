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

	void setLeadingEmptySamples(int leadingEmptySamples)
	{
		this->leadingEmptySamples = leadingEmptySamples;
	}

	void setTrailingEmptySamples(int trailingEmptySamples)
	{
		this->trailingEmptySamples = trailingEmptySamples;
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

	/*
	algorithm for finding new notes:

	1) find first note by looking for the first sample that is >200

	2) wait 3/4 of the length between the start of the last found note and the expected start of the next note(based on bpm and shortest note length)

	3) look for the next note:

		3a) record the next 1000 samples and find the average of those samples' absolute values
		(1000 samples = approx. 0.01 seconds = 1/50 of a quarter note at 120 bpm)
	
		3b) repeat (3a) until the average increases three times in a row, in which case proceed to (3c); each time
		the average increases after decreasing, mark the first sample of that 1000 sample group
	
		3c) if the sample value at the marked location is >200,
			label the most recent mark as the start of a new note
			else, do nothing

	4) repeat from (2) until the end of the non zero samples has been reached
	*/
	/*
	void findNoteLocations()
	{
		cout << "sampleCount - trailingEmptySamples: " << (sampleCount - trailingEmptySamples) << endl;
		int hi = 0;
		for (int count = 0; count < (sampleCount - trailingEmptySamples); count++)
		{
			hi = samples[count];
		}
	}
	*/
	vector<int> findNoteLocations()
	{
		vector<int> noteLocations;
		vector<short> noteLocationSampleValues; //for debugging

		short newNoteSampleThreshold = 100; //the minimum value a sample can have to be counted as a new note

		cout << "sample count: " << sampleCount << endl;
		cout << "leading: " << leadingEmptySamples << endl;
		cout << "trailing: " << trailingEmptySamples << endl;
		//while(true){}

		//1) find first note
		int currentSampleIndex = 0;
		//the for loop will break before the count reaches sampleCount - trailingEmptySamples 
		//(i.e. the last non zero sample)
		for (int count = leadingEmptySamples; count < (sampleCount - trailingEmptySamples); count++)
		{
			if (abs(samples[count]) > newNoteSampleThreshold)
			{
				//the first note is found when the sample is greater than 200
				//currentSampleIndex is the sample index of the first note
				currentSampleIndex = count;
				noteLocations.push_back(currentSampleIndex);
				noteLocationSampleValues.push_back(samples[currentSampleIndex]);
				break;
			}
		}

		bool endOfSamplesReached = false;

		while (!endOfSamplesReached)
		{

			//2) wait 3/4 of the length between the start of the last found note and the 
			//expected start of the next note (based on bpm and shortest note length) using the formula:
			//waitLength = 0.75 * (seconds per beat * shortest note length) * (sample rate * channel count)
			int waitLength = 0.75 * ((60.0 / bpm) * shortestNote) * (sampleRate * channelCount);
			currentSampleIndex += waitLength;
			
			//if there are less than 1000 non zero samples remaining (i.e. the next average cant be computed)
			//then the end of the samples has been reached
			if (currentSampleIndex + 1000 >= (sampleCount - trailingEmptySamples))
			{
				endOfSamplesReached = true;
			}

			//3a) record the next 1000 samples and find the maximum sample value
			//(1000 samples = approx. 0.01 seconds = 1 / 50 of a quarter note at 120 bpm)

			//3b) repeat (3a) until the average increases three times in a row, 
			//in which case proceed to (3c); each time the average increases after decreasing, 
			//mark the first sample of that 1000 sample group

			bool nextNoteCandidateFound = false;

			int firstIncreasingAverageLocation = 0;
			int increasingAverageCount = 0;
			int increasingAverageCountThreshold = 3;

			double avg_abs_sampleValue = 0;
			double prev_avg_abs_sampleValue = INT_MAX; //set to INT_MAX so that the first 1000 samples
													   //isnt automatically interpreted as an increase

			while (!nextNoteCandidateFound && !endOfSamplesReached) //exit if nextNoteFound OR endOfSamplesReached
			{
				//(3a)
				/*
				for (int count = 1070017; count < 1071017; count++)
				{
					cout << "abs(samples[count]): " << abs(samples[count]) << endl;
				}

				while(true){}
				*/

				for (int count = currentSampleIndex; count < (currentSampleIndex + 1000); count++)
				{
					avg_abs_sampleValue += abs(samples[count]);
				}
				avg_abs_sampleValue /= 1000;
				currentSampleIndex += 1000;

				//cout << "currentSampleIndex: " << currentSampleIndex << endl;
				//cout << "sampleCount - trailingEmptySamples: " << (sampleCount - trailingEmptySamples) << endl;
				//cout << "samples[1000000]: " << samples[1071099] << endl;

				//cin >> increasingAverageCount;

				if (avg_abs_sampleValue > prev_avg_abs_sampleValue)
				{
					increasingAverageCount++;

					if (!firstIncreasingAverageLocation)
					{
						firstIncreasingAverageLocation = currentSampleIndex - 1000;
						//currentSampleIndex - 1000 is the location of the first sample in the last group of 1000
					}

					//3c) label the most recent mark as the start of a new note if the sample's value is >200
					if (increasingAverageCount == increasingAverageCountThreshold)
					{
						nextNoteCandidateFound = true;

						if (abs(samples[currentSampleIndex]) > newNoteSampleThreshold)
						{
							noteLocations.push_back(currentSampleIndex);
							noteLocationSampleValues.push_back(samples[currentSampleIndex]); //for debugging
						}
					}

				}
				else //avg_abs_sampleValue < prev_avg_abs_sampleValue
				{
					firstIncreasingAverageLocation = 0;
				}

				//if there are less than 1000 non zero samples remaining
				//i.e. the next average cant be computed
				if (currentSampleIndex + 1000 >= (sampleCount - trailingEmptySamples))
				{
					endOfSamplesReached = true;
				}

				prev_avg_abs_sampleValue = avg_abs_sampleValue;
				
			}

			//4) repeat from (2) until the end of the non zero samples has been reached
		}

		cout << "noteLocationSampleValues size: " << noteLocationSampleValues.size() << endl;

		for (int count = 0; count < noteLocations.size(); count++)
		{
			cout << "Note location " << count << ": " << noteLocations[count] << ", sample: " << noteLocationSampleValues[count] << endl;
		}

		cout << "got here" << endl;
		while (true) {}
		
		return noteLocations;
	}

	void printSamples()
	{
		cout << "in rhythm accuracy's print samples" << endl;
		//cout << "samples[10000]: " << samples[10000] << endl;
		cout << "sampleCount:" << sampleCount << endl;


		double minSample = SHRT_MAX;
		double maxSample = SHRT_MIN;

		int minSampleCount;
		int maxSampleCount;

		int checkpointCount = 25000;

		bool lookingForNote = true;

		//process of looking for first note goes in a separate for loop here

		for (int count = leadingEmptySamples; count < (sampleCount - trailingEmptySamples); count += 1000)
		{
			double avg_abs_samplesAtCount = 0.0;
			for (int count2 = 0; count2 < 1000; count2++)
			{
				avg_abs_samplesAtCount += abs(samples[count + count2]);
			}

			

			avg_abs_samplesAtCount /= 1000.0;

			//cout << "avg_abs_samplesAtCount:" << avg_abs_samplesAtCount << endl;

			

			/*



			if (!lookingForNote && abs_samplesAtCount < 200)
			{
				//the note that was "found" ends when the sample is less than 200
				//now, the next note is being looked for
				//lookingForNote = true;
			}

			*/

			if (avg_abs_samplesAtCount < minSample)
			{
				minSample = avg_abs_samplesAtCount;
				minSampleCount = count;
			}
			if (avg_abs_samplesAtCount > maxSample)
			{
				maxSample = avg_abs_samplesAtCount;
				maxSampleCount = count;
			}


			//leadingEmptySamples is the value where count started in this for loop
			if ((count - leadingEmptySamples) % 25000 == 0 && count > leadingEmptySamples)
			{
				cout << "checkpoint " << checkpointCount << ": minSample is " << minSample
					<< " at " << minSampleCount << " and maxSample is " << maxSample << " at " << maxSampleCount << endl;

				minSample = SHRT_MAX;
				maxSample = SHRT_MIN;
				checkpointCount += 25000;
			}

		}

		//while(true){}
	}

private:

	short* samples;
	long long sampleCount;
	int leadingEmptySamples;
	int trailingEmptySamples;

	int sampleRate;
	int channelCount;

	int bpm;
	double shortestNote;


};