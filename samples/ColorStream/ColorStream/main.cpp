#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>

#include <tchar.h>
#include <windows.h>
#include <NuiApi.h>

// stl
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

#define COLOR_WIDTH 640
#define COLOR_HEIGHT 480

void InitializeKinect()
{
	bool FailToConnect;

	do
	{
		HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR);
		
		if(FAILED(hr))
		{
			system("cls");
			cout << "\nFailed to Connect!\n\n";
			FailToConnect = true;
			system("PAUSE");
		}
		else
		{
			cout << "\nConnection Established!\n\n";
			FailToConnect = false;
		}
	}

	while(FailToConnect);
}

int createRGBImage(HANDLE h, IplImage* Color)
{
	const NUI_IMAGE_FRAME *pImageFrame = NULL;

	HRESULT hr = NuiImageStreamGetNextFrame(h, 1000, &pImageFrame);

	if(FAILED(hr))
	{
		cout << "Create RGB Image Failed\n";
		return -1;
	}

	INuiFrameTexture *pTexture = pImageFrame->pFrameTexture;
	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect(0, &LockedRect, NULL, 0);

	if(LockedRect.Pitch != 0)
	{
		BYTE * pBuffer = (BYTE*)LockedRect.pBits;
		cvSetData(Color, pBuffer, LockedRect.Pitch);
		cvShowImage("Color Image", Color);
	}

	NuiImageStreamReleaseFrame(h, pImageFrame);

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE colorStreamHandle;
	HANDLE nextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HRESULT hr;

	InitializeKinect();

	IplImage *Color = cvCreateImage(cvSize(COLOR_WIDTH, COLOR_HEIGHT), IPL_DEPTH_8U, 4);
	cvNamedWindow("Color Image", CV_WINDOW_AUTOSIZE);

	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480,
		0, 2, nextColorFrameEvent, &colorStreamHandle);

	if (FAILED(hr))
	{
		cout << "Could not open ImageStream" << endl;
		return hr;
	}

	while(1)
	{
		WaitForSingleObject(nextColorFrameEvent, 1000);
		createRGBImage(colorStreamHandle, Color);

		if(cvWaitKey(10) == 0x001b)
		{
			break;
		}
	}

	cvReleaseImageHeader(&Color);
	cvDestroyWindow("Color Image");
	NuiShutdown();
	return 0;
}