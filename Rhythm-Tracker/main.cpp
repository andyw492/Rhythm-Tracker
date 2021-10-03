#include <iostream>
#include "RhythmTracker.cpp"

using namespace std;

int main()
{
	cout << "Please make sure the audio file is a .wav file" << endl;

	string fileName = "Recording (228).wav";
	int bpm = 120;
	string shortestNoteString = "eighth";

	RhythmTracker rhythmTracker;

	rhythmTracker.testing(false, false);

	rhythmTracker.loadSoundBuffer(false, true, fileName, bpm, shortestNoteString);
	rhythmTracker.loadBeatDifferences();
	rhythmTracker.displayWindow();

	// for some reason, a change needs to be made somewhere in main.cpp for each new compile
	string compileChange = "7777777777777777777777";

	return 0;
}