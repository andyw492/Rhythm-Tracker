#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <chrono>

#include "AudioInfo.h"

using namespace std;

// writes and reads user settings from Settings.txt
// stores and retrieves previous audio from files
class FileManager
{

public:

	FileManager(){}

	string getTimeString()
	{
		// 10/2/2021 7:30pm 32 seconds -> 2021-10-02 19 30 32

		auto now = chrono::system_clock::now();
		time_t now_time = chrono::system_clock::to_time_t(now);
		string dateTime = ctime(&now_time);

		// trim newline character
		dateTime = dateTime.substr(0, dateTime.length() - 1);

		string year = dateTime.substr(dateTime.length() - 4, dateTime.length());
		dateTime = dateTime.substr(0, dateTime.length() - 5);

		string seconds = dateTime.substr(dateTime.length() - 2, dateTime.length());
		dateTime = dateTime.substr(0, dateTime.length() - 3);

		string minutes = dateTime.substr(dateTime.length() - 2, dateTime.length());
		dateTime = dateTime.substr(0, dateTime.length() - 3);

		string hours = dateTime.substr(dateTime.length() - 2, dateTime.length());
		dateTime = dateTime.substr(0, dateTime.length() - 3);

		string day = dateTime.substr(dateTime.length() - 2, dateTime.length());
		int blankIndex = day.find(" ");
		if (blankIndex != string::npos)
		{
			day.replace(0, 1, "0");
		}
		dateTime = dateTime.substr(0, dateTime.length() - 3);

		string month = dateTime.substr(dateTime.length() - 3, dateTime.length());
		// cant use switch() with strings :/
		if (!month.compare("Jan")) { month = "01"; }
		if (!month.compare("Feb")) { month = "02"; }
		if (!month.compare("Mar")) { month = "03"; }
		if (!month.compare("Apr")) { month = "04"; }
		if (!month.compare("May")) { month = "05"; }
		if (!month.compare("Jun")) { month = "06"; }
		if (!month.compare("Jul")) { month = "07"; }
		if (!month.compare("Aug")) { month = "08"; }
		if (!month.compare("Sep")) { month = "09"; }
		if (!month.compare("Oct")) { month = "10"; }
		if (!month.compare("Nov")) { month = "11"; }
		if (!month.compare("Dec")) { month = "12"; }	

		string timeString = year + "-" + month + "-" + day + " " + hours + " " + minutes + " " + seconds;

		return timeString;
	}

	void saveAudioToFile(sf::SoundBuffer buffer, AudioInfo info, string fileName)
	{
		// set recording_index to highest recording_index + 1
		ifstream reader;
		int highestRecordingIndex = -1;
		string line;
		reader.open("Saved Recording Info.txt", ifstream::in);

		while (getline(reader, line))
		{
			if (line.find("recording_index=") != string::npos)
			{
				highestRecordingIndex = stoi(line.substr(line.find("=") + 1));
			}
		}

		reader.close();
		
		int recording_index = highestRecordingIndex + 1;

		// if no specified fileName (i.e. fileName == "")
		// set fileName to getTimeString()
		if (fileName.length() == 0) { fileName = getTimeString(); }

		// write information to Saved Recordings Info.txt
		ofstream writer;
		writer.open("Saved Recording Info.txt", ofstream::out | ofstream::app);
		
		writer << "recording_index=" << recording_index << endl;
		writer << "fileName=" << fileName << endl;
		writer << "sampleRate=" << info.getSampleRate() << endl;
		writer << "channelCount=" << info.getChannelCount() << endl;
		writer << "bpm=" << info.getBpm() << endl;
		writer << "shortestNote=" << info.getShortestNote() << endl;

		// write noteLocations as 1234,2345,3456,...
		vector<int> noteLocations = info.getNoteLocations();
		string noteLocationsString = "noteLocations=";
		for (int i = 0; i < noteLocations.size(); i++)
		{
			noteLocationsString = noteLocationsString + to_string(noteLocations[i]) + ",";
		}
		// remove last ','
		noteLocationsString = noteLocationsString.substr(0, noteLocationsString.length() - 1);
		writer << noteLocationsString << endl;

		writer << "samplesBeforeFirstNote=" << info.getSamplesBeforeFirstNote() << endl;
		writer << "---" << endl;
		
		writer.close();

		// save the audio as an .ogg file
		buffer.saveToFile("Saved Recordings/" + fileName + ".ogg");
	}
};