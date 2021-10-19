#pragma once

#ifndef COMMON_CPP
#define COMMON_CPP

#include <iostream>
#include <fstream>
#include <string>

#include "common.h"

using namespace std;

int findIntFromSettings(string searchSetting);
double findDoubleFromSettings(string searchSetting);

int getMaxRecordingSeconds()
{
	return 60;
}

double getNoteSearchRange()
{
	return 1;
}

double getMaxRectangleHeight()
{

	return 125.0;
	//printf("in getmaxrectangleheight\n");
	//return findIntFromSettings("Max Rectangle Height");
}

double getMaxBeatDifference()
{
	return 1;
}

double getGreenThreshold()
{
	return 1;
}

double getYellowThreshold()
{
	return 1;
}

int getMaxFileNameLength()
{
	return 32;
}

int findIntFromSettings(string searchSetting)
{
	printf("in findintfromsettings\n");

	fstream settingsFile("Rhythm-Tracker/Settings.txt");

	cout << "open? " << settingsFile.is_open() << endl;

	bool foundString = false;
	int value = -1;

	while (!foundString && !settingsFile.eof())
	{
		string fileLine;
		getline(settingsFile, fileLine);
		//printf("file line: %s\n", fileLine);

		value++;
		if (value == 10)
		{
			while(true){}
		}

		if (fileLine.find(searchSetting) != string::npos)
		{
			printf("found the search setting\n");
			value = stoi(fileLine.substr(fileLine.find(":") + 1, fileLine.find("\n")));
			foundString = true;
		}
	}

	if (!foundString)
	{
		printf("ERROR: setting not found");
	}

	settingsFile.close();

	return value;
}

double findDoubleFromSettings(string searchSetting)
{
	return 1;
}

#endif