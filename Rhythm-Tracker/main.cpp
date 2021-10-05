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

	if (rhythmTracker.testing())
	{
		string s;
		cin >> s;
		return 0;
	}

	rhythmTracker.displayWindow();

	// for some reason, a change needs to be made somewhere in main.cpp for each new compile
	string compileChange = "77777777777777777777777";

	return 0;
}