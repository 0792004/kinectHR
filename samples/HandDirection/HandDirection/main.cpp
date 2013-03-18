#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>

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

CvPoint points[NUI_SKELETON_POSITION_COUNT];

void InitializeKinect()
{
	bool FailToConnect;

	do
	{
		HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_SKELETON);
		//HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);
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

void drawBone(const NUI_SKELETON_DATA &position, NUI_SKELETON_POSITION_INDEX j1, NUI_SKELETON_POSITION_INDEX j2, IplImage *Skeleton)
{
	NUI_SKELETON_POSITION_TRACKING_STATE j1state = position.eSkeletonPositionTrackingState[j1];
	NUI_SKELETON_POSITION_TRACKING_STATE j2state = position.eSkeletonPositionTrackingState[j2];

	if(j1state == NUI_SKELETON_POSITION_TRACKED && j2state == NUI_SKELETON_POSITION_TRACKED)
	{
		cvLine(Skeleton, points[j1], points[j2], CV_RGB(0, 255, 0), 3, 8, 0);
	}
}

CvPoint SkeletonToScreen(Vector4 skeletonPoint)
{
	LONG x, y;
	USHORT depth;

	NuiTransformSkeletonToDepthImage(skeletonPoint, &x, &y,
		&depth, NUI_IMAGE_RESOLUTION_640x480);

	float screenPointX = static_cast<float>(x);
	float screenPointY = static_cast<float>(y);

	return cvPoint(screenPointX, screenPointY);
}


void drawSkeleton(const NUI_SKELETON_DATA &position, IplImage *Skeleton)
{
	for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
	{
		points[i] = SkeletonToScreen(position.SkeletonPositions[i]);
	}

	drawBone(position, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT, Skeleton);
	drawBone(position, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT, Skeleton);
	drawBone(position, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT, Skeleton);
	
	drawBone(position, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT, Skeleton);
	drawBone(position, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT, Skeleton);
	drawBone(position, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT, Skeleton);

}

void createSkeleton(HANDLE h, IplImage * Skeleton, IplImage *Color)
{
	NUI_SKELETON_FRAME skeletonFrame = {0};
	//IplImage *Skeleton_clear = cvCreateImage(cvSize(COLOR_WIDTH, COLOR_HEIGHT), IPL_DEPTH_8U, 4);
	//cvCopy(Skeleton_clear, Skeleton);

	CvFont font;
	cvInitFont(&font, CV_FONT_VECTOR0, 0.45, 0.45, 0, 1);
	char str[100];

	HRESULT hr = NuiSkeletonGetNextFrame(0, &skeletonFrame);
	if(FAILED(hr))
	{
		return;
	}

	NuiTransformSmooth(&skeletonFrame, NULL);

	for (int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		NUI_SKELETON_TRACKING_STATE state = skeletonFrame.SkeletonData[i].eTrackingState;

		if(NUI_SKELETON_TRACKED == state)
		{
			drawSkeleton(skeletonFrame.SkeletonData[i], Color);
			/*
			sprintf(str, "(%1.3f, %1.3f) / %d",
				skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].x,
				skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].y,
				skeletonFrame.dwFrameNumber);
			cvPutText(Color, str, cvPoint(20, 20),  &font, CV_RGB(128, 128, 0));
			*/

			printf("x: %f %d\n",
				skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].x,
				(int)(skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].x
				* 1000.0));

			printf("y: %f %d\n",
				skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].y,
				(int)(skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].y
				* 1000.0));

			printf("NUI_SKELETON_TRACKED\n");
		}
		//cvShowImage("Skeleton Image", Color);
	}

	//cvReleaseImage(&Skeleton_clear);
}

int _tmain(int argc, _TCHAR* argv[])
{

	HANDLE colorStreamHandle;
	HANDLE nextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE skeletonStreamHandle;
	HANDLE nextSkeletonFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HRESULT hr;

	InitializeKinect();

	IplImage *Color = cvCreateImage(cvSize(COLOR_WIDTH, COLOR_HEIGHT), IPL_DEPTH_8U, 4);
	IplImage *Skeleton = cvCreateImage(cvSize(COLOR_WIDTH, COLOR_HEIGHT), IPL_DEPTH_8U, 4);

	cvNamedWindow("Color Image", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Skeleton Image", CV_WINDOW_AUTOSIZE);

	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480, 0, 2, nextColorFrameEvent, &colorStreamHandle);

	if (FAILED(hr))
	{
		cout << "Could not open ImageStream" << endl;
		return hr;
	}

	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480, 0, 2, nextSkeletonFrameEvent, &skeletonStreamHandle);
	hr = NuiSkeletonTrackingEnable(nextSkeletonFrameEvent, 0);
	//hr = NuiSkeletonTrackingEnable(nextSkeletonFrameEvent, NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT);

	if (FAILED(hr))
	{
		cout << "Could not open SkeletonStream" << endl;
		return hr;
	}
	
	while(1)
	{
		WaitForSingleObject(nextColorFrameEvent, 0);
		createRGBImage(colorStreamHandle, Color);
		WaitForSingleObject(nextSkeletonFrameEvent, INFINITE);
		createSkeleton(skeletonStreamHandle, Skeleton, Color);
		//createImage(colorStreamHandle, Color, skeletonStreamHandle, Skeleton);

		if(cvWaitKey(10) == 0x001b)
		{
			break;
		}
	
	}

	NuiShutdown();

	cvReleaseImageHeader(&Color);
	cvReleaseImageHeader(&Skeleton);

	cvDestroyAllWindows();
	return 0;
}