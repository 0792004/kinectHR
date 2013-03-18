#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>

#include <tchar.h>
#include <Windows.h>
#include <NuiApi.h>

// stl
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

#define DEPTH_WIDTH	640
#define DEPTH_HEIGHT 480

RGBQUAD rgb[640*480];

RGBQUAD Nui_ShortToQuad_Depth(USHORT s)
{
	USHORT realDepth = (s & 0xfff8) >> 3;
	BYTE i = 255 - (BYTE)(256 * realDepth / (0x0fff));
	
	RGBQUAD q;
	q.rgbRed = q.rgbGreen = q.rgbBlue = ~i;
	return q;
}

int createDepthImage(HANDLE h, IplImage* Depth)
{
	const NUI_IMAGE_FRAME *pImageFrame = NULL;

	HRESULT hr = NuiImageStreamGetNextFrame(h, 1000, &pImageFrame);

	if(FAILED(hr))
	{
		cout << "Create Depth Image Failed\n";
		return -1;
	}

	INuiFrameTexture *pTexture = pImageFrame->pFrameTexture;
	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect(0, &LockedRect, NULL, 0);

	if(LockedRect.Pitch != 0)
	{
		BYTE * pBuffer = (BYTE*)LockedRect.pBits;
		RGBQUAD *rgbrun = rgb;
		USHORT *pBufferRun = (USHORT*) pBuffer;

		for(int y = 0; y < DEPTH_HEIGHT; y++)
		{
			for(int x = 0; x < DEPTH_WIDTH; x++)
			{
				RGBQUAD quad = Nui_ShortToQuad_Depth(*pBufferRun);
				pBufferRun++;
				*rgbrun = quad;
				rgbrun++;
			}
		}

		cvSetData(Depth, (BYTE*)rgb, Depth->widthStep);
		cvShowImage("Depth Image", Depth);
	}

	NuiImageStreamReleaseFrame(h, pImageFrame);

	return 0;
}

void InitializeKinect()
{
	bool FailToConnect;

	do
	{
		HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);
		
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
	}while(FailToConnect);
}

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE depthStreamHandle;
	HANDLE nextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HRESULT hr;

	IplImage *Depth = cvCreateImage(cvSize(DEPTH_WIDTH, DEPTH_HEIGHT), IPL_DEPTH_8U, 4);
	cvNamedWindow("Depth Image", CV_WINDOW_AUTOSIZE);

	InitializeKinect();

	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_640x480,
		0, 2, nextDepthFrameEvent, &depthStreamHandle);

	if (FAILED(hr))
	{
		cout << "Could not open ImageStream" << endl;
		return hr;
	}

	while(1)
	{
		WaitForSingleObject(nextDepthFrameEvent, INFINITE);
		createDepthImage(depthStreamHandle, Depth);

		if(cvWaitKey(10) == 0x001b)
		{
			break;
		}
	}

	cvReleaseImageHeader(&Depth);
	cvDestroyWindow("Depth Image");
	NuiShutdown();
	
	return 0;
}