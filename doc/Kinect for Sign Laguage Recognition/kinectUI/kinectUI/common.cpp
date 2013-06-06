#include "stdafx.h"
#include "common.h"
#include <fstream>

bool Settings::InitSettings()
{
	// settins.ini 파일을 읽어들임
	ifstream fin;
	fin.open("settings.ini");
	if (fin.is_open())
	{
		char name[STRING_SIZE] = {0};
		char val[STRING_SIZE] = {0};

		// PATH
		fin.getline(name, STRING_SIZE);
		if (strcmp(name, "PATH") != 0)
			return false;

		fin.getline(val, STRING_SIZE);
		strcpy(PATH, val);

		// FPS
		fin.getline(name, STRING_SIZE);
		if (strcmp(name, "FPS") != 0)
			return false;

		fin.getline(val, STRING_SIZE);
		int fps = atoi(val);
		if (fps >= 15)
			FPS = fps;
		else
			return false;

		// MIN_STOPPED_FRAME
		fin.getline(name, STRING_SIZE);
		if (strcmp(name, "MIN_STOPPED_FRAME") != 0)
			return false;

		fin.getline(val, STRING_SIZE);
		int minStoppedFrame = atoi(val);
		if (minStoppedFrame >= 3)
			MIN_STOPPED_FRAME = minStoppedFrame;
		else
			return false;

		// RECOG_FRAME_RANGE
		fin.getline(name, STRING_SIZE);
		if (strcmp(name, "RECOG_FRAME_RANGE") != 0)
			return false;

		fin.getline(val, STRING_SIZE);
		float range = atof(val);
		if (range >= 0.1)
			RECOG_FRAME_RANGE = range;
		else
			return false;

		// MAX_RAW_DATA_FRAME
		fin.getline(name, STRING_SIZE);
		if (strcmp(name, "MAX_RAW_DATA_FRAME") != 0)
			return false;

		fin.getline(val, STRING_SIZE);
		int maxRawDataFrame = atoi(val);
		if (maxRawDataFrame >= 300)
			MAX_RAW_DATA_FRAME = maxRawDataFrame;
		else
			return false;
	}
	fin.close();
	return true;
}