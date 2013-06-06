#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <limits>
#include <atlstr.h>
#include <atltypes.h>

using namespace std;

class SimpleDTW
{
private:
	int *x;
	int *y;
	
	vector<int> a;
	vector<int> b;
	vector<CPoint> path;
	
	int **distance;
	int **f;

	int initGap;
	int widthGap;
	int heightGap;
	int equalSlope;

	int fSum;
	int bSum;

	int xLen;
	int yLen;

public:
	SimpleDTW(vector<int> &_a, vector<int> &_b, int _xLen, int _yLen);
	SimpleDTW(int* _x,int* _y, int _xLen, int _yLen);
	~SimpleDTW();
	int** getDistance();
	int** getFMatrix();
	int getForwardSum();
	int getBackwardSum();
	int getInitGap();
	int getWidthGap();
	int getHeightGap();
	int getEqualSlope();
	vector<CPoint> getPath();
	int computeFForward();
	int computeFBackward(int i, int j);
	void pathTracking(int i, int j);
};