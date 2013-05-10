#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <atlstr.h>

#define NEG_INF -999
#define POS_INF 999

using namespace std;

class SimpleDTW
{
private:
	int *x;
	int *y;
	int **distance;
	int **f;
	vector<int> a;
	vector<int> b;

	int sum;

	int xLen;
	int yLen;

public:
	SimpleDTW(vector<int> &_a, vector<int> &_b, int _xLen, int _yLen)
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
			f[i][0] = POS_INF;

		for (int i = 0; i <= yLen; i++)
			f[0][i] = POS_INF;

		f[0][0] = 0;
		sum = 0;
	}

	SimpleDTW(int* _x,int* _y, int _xLen, int _yLen)
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
			f[i][0] = POS_INF;

		for (int i = 0; i <= yLen; i++)
			f[0][i] = POS_INF;

		f[0][0] = 0;
		sum = 0;
	}

	~SimpleDTW()
	{
		for (int i = 0; i < xLen; i++)
			delete [] distance[i];
		delete [] distance;

		for (int i = 0; i < xLen+1; i++)
			delete [] f[i];
		delete f;
	}

	int getSum()
	{
		return sum;
	}
	
	int** getDistance()
	{
		return distance;
	}

	int** getFMatrix()
	{
		return f;
	}
	
	void computeDTW()
	{
		//sum = computeFBackward(xLen, yLen);
		sum = computeFForward();
	}

	int computeFForward()
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

	int computeFBackward(int i, int j)
	{
		if (!(f[i][j] < 0.0))
		{
			return f[i][j];
		}
		else
		{
			if (computeFBackward(i - 1, j) <= computeFBackward(i, j - 1) && computeFBackward(i - 1, j) <= computeFBackward(i - 1, j - 1)
				&& computeFBackward(i - 1, j) < POS_INF)
			{
				f[i][j] = distance[i - 1][j - 1] + computeFBackward(i - 1, j);
			}
			else if (computeFBackward(i, j - 1) <= computeFBackward(i - 1, j) && computeFBackward(i, j - 1) <= computeFBackward(i - 1, j - 1)
				&& computeFBackward(i, j - 1) < POS_INF)
			{
				f[i][j] = distance[i - 1][j - 1] + computeFBackward(i, j - 1);
			}
			else if (computeFBackward(i - 1, j - 1) <= computeFBackward(i - 1, j) && computeFBackward(i - 1, j - 1) <= computeFBackward(i, j - 1)
				&& computeFBackward(i - 1, j - 1) < POS_INF)
			{
				f[i][j] = distance[i - 1][j - 1] + computeFBackward(i - 1, j - 1);
			}
		}
		return f[i][j];
	}
};

void main()
{
	int x[] = { 9, 3, 1, 5, 1, 2, 0, 1, 0, 2, 2, 8, 1, 7, 0, 6, 4, 4, 5 };
    int y[] = { 1, 0, 5, 5, 0, 1, 0, 1, 0, 3, 3, 2, 8, 1, 0, 6, 4, 4, 5 };
	int xLen = sizeof(x) / sizeof(int);
	int yLen = sizeof(y) / sizeof(int);

	vector<int> a;
	vector<int> b;
	char line[1024];


	CString path;
	path = "C:\\Users\\kwon\\Documents\\";

	ifstream fin;
	CString aPath = path + "go.txt";
	fin.open(aPath);
	
	if (fin.is_open())
	{
		int i = 0;
		while(fin.getline(line, 1024))
		{
			if (strcmp(line, "\n") != 0) {
				a.push_back(atoi(line));
				//printf("%d\n", a.at(i));
				i++;
			}
		}
	}
	fin.close();

	CString bPath = path + "go2.txt";
	fin.open(bPath);
	
	if (fin.is_open())
	{
		int i = 0;
		while(fin.getline(line, 1024))
		{
			if (strcmp(line, "\n") != 0) {
				b.push_back(atoi(line));
				//printf("%d\n", b.at(i));
				i++;
			}
		}
	}
	fin.close();
	//b.push_back(100);


	/*for (int i = 0; i < xLen; i++)
		a.push_back(x[i]);

	for (int i = 0; i < yLen; i++)
		b.push_back(y[i]);*/

	//int x[] = { -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10 };
	//int x[] = { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 };
	//int y[] = { 1, 1 };
	// sum의 최소값 = 0
	// sum의 최대값 = (최대값 - 최소값) * 긴 배열의 길이
	
	SimpleDTW dtw(a, b, a.size(), b.size());
	//SimpleDTW dtw(x, y, xLen, yLen);
	dtw.computeDTW();
	cout << dtw.getSum() << endl;
	
	printf("%d\n", a.size());
	printf("%d\n", b.size());
	int longSize = (a.size() > b.size()) ? a.size() : b.size();
	int max = (180 - (-180)) * longSize;
	printf("max %d\n", max);
	float similarity = (max - dtw.getSum()) / (float) max;
	printf("similarity %1.2f\n", similarity);

	/*int **f = dtw.getFMatrix();
	for (int i = 0; i <= xLen; i++)
	{
		for (int j = 0; j <= yLen; j++)
			cout << f[i][j] << "\t";
		cout << endl;
	}

	cout << endl << endl;

	int **d = dtw.getDistance();
	for (int i = 0; i < xLen; i++)
	{
		for (int j = 0; j < yLen; j++)
			cout << f[i][j] << "\t";
		cout << endl;
	}*/
	
}