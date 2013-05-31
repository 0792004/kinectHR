#pragma once
//#include "afxwin.h"
#include <cmath>

#define PI						3.14159265358979323846f
#define SL_LIST_TXT				"sl_list.txt"
#define IMG_LIST_TXT			"img_list.txt"
#define STRING_SIZE				1024
#define VIEW_RATE				70
#define ANGLE_DIFFERENCE		3

using namespace std;

class Settings {
public:
	int FPS;
	int MIN_STOPPED_FRAME;
	int MAX_RAW_DATA_FRAME;
	float RECOG_FRAME_RANGE;
	char PATH[STRING_SIZE];

	bool InitSettings();
};

inline double GetAngle(double x1, double x2, double y1, double y2)
{
	double dx = x2 - x1;
	double dy = y2 - y1;

	double rad = atan2(dx, dy);
	double degree = (rad * 180) / PI ;

	return degree;
}
