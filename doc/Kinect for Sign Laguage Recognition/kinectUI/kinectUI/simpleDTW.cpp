#include "stdafx.h"
#include "simpleDTW.h"

SimpleDTW::SimpleDTW(vector<int> &_a, vector<int> &_b, int _xLen, int _yLen)
{
	a = _a;
	b = _b;

	xLen = _xLen;
	yLen = _yLen;

	distance = new int* [xLen];
	for (int i = 0; i < xLen; i++)
		distance[i] = new int [yLen];

	f = new int* [xLen+1];
	for (int i = 0; i < xLen+1; i++)
		f[i] = new int [yLen+1];

	for (int i = 0; i < xLen; i++)
		for (int j = 0; j < yLen; j++)
			distance[i][j] = abs(a.at(i) - b.at(j));

	for (int i = 0; i <= xLen; i++)
		for (int j = 0; j <= yLen; j++)
			f[i][j] = -1;

	for (int i = 0; i <= xLen; i++)
		f[i][0] = INT_MAX;

	for (int i = 0; i <= yLen; i++)
		f[0][i] = INT_MAX;

	initGap = abs(xLen - yLen);
	widthGap = 0;
	heightGap = 0;
	equalSlope = 1; // 끝점도 equalSlope인데 함수에서 따로 세지 않으므로
	f[0][0] = 0;
	bSum = 0;
	fSum = 0;
}

SimpleDTW::SimpleDTW(int* _x,int* _y, int _xLen, int _yLen)
{
	x = _x;
	y = _y;

	xLen = _xLen;
	yLen = _yLen;

	distance = new int* [xLen];
	for (int i = 0; i < xLen; i++)
		distance[i] = new int [yLen];

	f = new int* [xLen+1];
	for (int i = 0; i < xLen+1; i++)
		f[i] = new int [yLen+1];

	for (int i = 0; i < xLen; i++)
		for (int j = 0; j < yLen; j++)
			distance[i][j] = abs(x[i] - y[j]);

	for (int i = 0; i <= xLen; i++)
		for (int j = 0; j <= yLen; j++)
			f[i][j] = -1;

	for (int i = 0; i <= xLen; i++)
		f[i][0] = INT_MAX;

	for (int i = 0; i <= yLen; i++)
		f[0][i] = INT_MAX;

	f[0][0] = 0;
	bSum = 0;
	fSum = 0;
}

SimpleDTW::~SimpleDTW()
{
	for (int i = 0; i < xLen; i++)
		delete [] distance[i];
	delete [] distance;

	for (int i = 0; i < xLen+1; i++)
		delete [] f[i];
	delete f;
}

int** SimpleDTW::getDistance()
{
	return distance;
}

int** SimpleDTW::getFMatrix()
{
	return f;
}

int SimpleDTW::getForwardSum()
{
	fSum = computeFForward();
	return fSum;
}

int SimpleDTW::getBackwardSum()
{
	bSum = computeFBackward(xLen, yLen);
	return bSum;
}

int SimpleDTW::getInitGap()
{
	return initGap;
}

int SimpleDTW::getWidthGap()
{
	return widthGap;
}

int SimpleDTW::getHeightGap()
{
	return heightGap;
}

int SimpleDTW::getEqualSlope()
{
	return equalSlope;
}

vector<CPoint> SimpleDTW::getPath()
{
	this->pathTracking(xLen, yLen);
	return path;
}

int SimpleDTW::computeFForward()
{
	for (int i = 1; i <= xLen; ++i)
	{
		for (int j = 1; j <= yLen; ++j)
		{
			if (f[i - 1][j] <= f[i - 1][j - 1] && f[i - 1][j] <= f[i][j - 1])
			{
				f[i][j] = distance[i - 1][j - 1] + f[i - 1][j];
			}
			else if (f[i][j - 1] <= f[i - 1][j - 1] && f[i][j - 1] <= f[i - 1][j])
			{
				f[i][j] = distance[i - 1][j - 1] + f[i][j - 1];
			}
			else if (f[i - 1][j - 1] <= f[i][j - 1] && f[i - 1][j - 1] <= f[i - 1][j])
			{
				f[i][j] = distance[i - 1][j - 1] + f[i - 1][j - 1];
			}
		}
	}
	return f[xLen][yLen];
}

int SimpleDTW::computeFBackward(int i, int j)
{
	if (!(f[i][j] < 0.0))
	{
		return f[i][j];
	}
	else
	{
		if (computeFBackward(i - 1, j) <= computeFBackward(i, j - 1) && computeFBackward(i - 1, j) <= computeFBackward(i - 1, j - 1)
			&& computeFBackward(i - 1, j) < INT_MAX)
		{
			f[i][j] = distance[i - 1][j - 1] + computeFBackward(i - 1, j);
		}
		else if (computeFBackward(i, j - 1) <= computeFBackward(i - 1, j) && computeFBackward(i, j - 1) <= computeFBackward(i - 1, j - 1)
			&& computeFBackward(i, j - 1) < INT_MAX)
		{
			f[i][j] = distance[i - 1][j - 1] + computeFBackward(i, j - 1);
		}
		else if (computeFBackward(i - 1, j - 1) <= computeFBackward(i - 1, j) && computeFBackward(i - 1, j - 1) <= computeFBackward(i, j - 1)
			&& computeFBackward(i - 1, j - 1) < INT_MAX)
		{
			f[i][j] = distance[i - 1][j - 1] + computeFBackward(i - 1, j - 1);
		}
	}
	return f[i][j];
}

void SimpleDTW::pathTracking(int i, int j)
{
	if ((i - 1 > 0) && (j - 1 > 0))
	{
		if ((f[i - 1][j - 1] <= f[i][j])
			&& (f[i - 1][j - 1] <= f[i - 1][j])
			&& (f[i - 1][j - 1] <= f[i][j - 1]))
		{
			pathTracking(i - 1, j - 1);
			equalSlope++;

		}
		else if ((f[i][j - 1] <= f[i][j])
			&& (f[i - 1][j - 1] <= f[i - 1][j])
			&& !(f[i - 1][j - 1] <= f[i][j - 1]))
		{
			pathTracking(i, j - 1);
			widthGap++;
		}
		else if ((f[i - 1][j] <= f[i][j])
			&& !(f[i - 1][j - 1] <= f[i - 1][j])
			&& (f[i - 1][j - 1] <= f[i][j - 1]))
		{
			pathTracking(i - 1, j);
			heightGap++;
		}
	}
	else if ((i - 1 > 0) && !(j - 1 > 0))
	{
		pathTracking(i - 1, j);
		heightGap++;
	}
	else if (!(i - 1 > 0) && (j - 1 > 0))
	{
		pathTracking(i, j - 1);
		widthGap++;
	}
	path.push_back(CPoint(i, j));
	return;
}