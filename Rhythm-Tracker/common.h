#pragma once

#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int findIntFromSettings(string searchSetting);

double findDoubleFromSettings(string searchSetting);

double getNoteSearchRange();

double getMaxRectangleHeight();

double getMaxBeatDifference();

double getGreenThreshold();

double getYellowThreshold();

int getMaxFileNameLength();

int findIntFromSettings(string searchSetting);


double findDoubleFromSettings(string searchSetting);

#endif