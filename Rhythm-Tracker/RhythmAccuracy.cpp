#include <iostream>
#include <cmath>
#include <vector>
#include <string>

#include "AudioInfo.h"

using namespace std;

class RhythmAccuracy 
{

public:

	RhythmAccuracy(AudioInfo info)
	{
		audioInfo = AudioInfo(info);
	}

	// function called in main.cpp
	// returns a vector of the differences between found note location and expected note location for each beat
	// if there is no note in a beat, then INT_MIN is stored for that beat
	vector<double> findBeatDifferences()
	{
		
		vector<double> beatDifferences;
		
		vector<int> expectedNoteLocations(findExpectedNoteLocations());
		
		
		double samplesPerBeat = audioInfo.getSampleRate() * audioInfo.getChannelCount() *(60.0 / audioInfo.getBpm());
		
		// create a copy of AudioInfo::noteLocations because it needs to be modified to find the beat differences
		vector<int> tempNoteLocations = audioInfo.getNoteLocations();

		for (int i = 0; i < tempNoteLocations.size(); i++)
		{

			// if there was no note at the expected note location
			if (expectedNoteLocations[i] == INT_MAX)
			{
				// a value of INT_MAX in beatDifferences shows that were was no note
				beatDifferences.push_back(INT_MAX);

				// noteLocations needs to insert INT_MAX at i if there is no note to keep it synced with expectedNoteLocations
				tempNoteLocations.insert(tempNoteLocations.begin() + i, INT_MAX);
			}
			else
			{
				// fill beatDifferences with expectedNoteLocations[i] - noteLocations[i] converted from samples to beats
				beatDifferences.push_back((expectedNoteLocations[i] / samplesPerBeat) - (tempNoteLocations[i] / samplesPerBeat));
			}

		}

		return beatDifferences;
	}

	// returns a vector of the expected (i.e. rhythmically correct) locations for each note in noteLocations
	vector<int> findExpectedNoteLocations()
	{
		vector<int> noteLocations = audioInfo.getNoteLocations();
		vector<int> expectedNoteLocations;

		if (noteLocations.size() == 0)
		{
			return expectedNoteLocations;
		}

		int sampleIndex = noteLocations[0];
		expectedNoteLocations.push_back(sampleIndex);

		// sampleIndexIncrementCount is the number of samples for each beat
		double notesPerSecond = 1 / (audioInfo.getBpm() / 60.0);
		double sampleIndexIncrementCount = notesPerSecond * audioInfo.getShortestNote() * audioInfo.getSampleRate() * audioInfo.getChannelCount();

		// starting at the sample index of the first note, increment sampleIndex by the number of samples per beat
		// until there is a note that corresponds to the current beat
		// when a corresponding note is found (i.e. the next note in noteLocations), 
		// push it into expectedNoteLocations and end the for loop iteration to begin looking for the next note
		for (int i = 1; i < noteLocations.size(); i++)
		{
			bool noteFound = false;

			while (!noteFound)
			{
				if (sampleIndex + sampleIndexIncrementCount >= audioInfo.getSampleCount())
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

	AudioInfo audioInfo;

};