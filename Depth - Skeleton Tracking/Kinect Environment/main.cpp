////////////////////////////////////////
//  Dynamic Sign Language Recognition //
////////////////////////////////////////
/*	Hansung University	              */
/*	Computer System Engineering	      */
/*	Daesik - Mun				          */
/*  2012.07.01 ~ 2013.06.15           */
////////////////////////////////////////

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>

#include <tchar.h>
#include <Windows.h>
#include <NuiApi.h>  // Microsoft Kinect SDK

#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

CvPoint points[NUI_SKELETON_POSITION_COUNT];//to use skeleton joint
RGBQUAD rgb[640*480];//depthstream 


CvPoint SkeletonToScreen(Vector4 skeletonPoint);
RGBQUAD Nui_ShortToQuad_Depth(SHORT s);

void InitializeKinect();
int createRGBImage(HANDLE h, IplImage* Color);//RAW Image

void drawBone(const NUI_SKELETON_DATA &position, NUI_SKELETON_POSITION_INDEX j1, NUI_SKELETON_POSITION_INDEX j2, IplImage *Skeleton);//draw bone use Skeleton joint
void drawSkeleton(const NUI_SKELETON_DATA &position, IplImage *Skeleton);//used skeleton image
int createDepthSkeletonImage(HANDLE h, IplImage *Depth, IplImage *Skeleton);



CvPoint SkeletonToScreen(Vector4 skeletonPoint)
{
	LONG x,y;
	USHORT depth;
	NuiTransformSkeletonToDepthImage(skeletonPoint, &x, &y, &depth,NUI_IMAGE_RESOLUTION_640x480);

	float screenPointX = static_cast<float>(x);
	float screenPointY = static_cast<float>(y);

	return cvPoint(screenPointX, screenPointY);
}

RGBQUAD Nui_ShortToQuad_Depth(SHORT s)
{
	USHORT realDepth = (s&0xfff8) >> 3;
	BYTE I = 255-(BYTE)(256*realDepth / (0x0fff));

	RGBQUAD q;
	q.rgbRed = q.rgbBlue = q.rgbGreen = ~I;
	return q;
}


void InitializeKinect()  // 키넥트 초기화 함수 
{
	bool FailToConnect;

	do
	{
		HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_SKELETON | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | 
			NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT);  // NuiApi.h에서 불러온 함수가 처음 등장함
		if(FAILED(hr))
		{
			system("cls");
			cout<<"\nFailed to Connect!\n\n";
			FailToConnect = true;
			system("PAUSE");
		}
		else
		{
			cout<<"\nConnection Established!\n\n";
			FailToConnect=false;
		}
	}
	while(FailToConnect);
}


int createRGBImage(HANDLE h, IplImage* Color)
{
	const NUI_IMAGE_FRAME *pImageFrame = NULL;
	HRESULT hr = NuiImageStreamGetNextFrame(h,1000,&pImageFrame);

	if(FAILED(hr))
	{
		cout<<"Create FGB Image Failed\n";
		return -1;
	}

	INuiFrameTexture *pTexture=pImageFrame->pFrameTexture;
	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect(0,&LockedRect,NULL,0);

	if(LockedRect.Pitch !=0)
	{
		BYTE* pBuffer = (BYTE*)LockedRect.pBits;
		cvSetData(Color, pBuffer,LockedRect.Pitch);
		cvShowImage("Color Image", Color);
	}

	NuiImageStreamReleaseFrame(h,pImageFrame);
	return 0;
}

void drawBone(const NUI_SKELETON_DATA &position, NUI_SKELETON_POSITION_INDEX j1, NUI_SKELETON_POSITION_INDEX j2, IplImage *Skeleton)
{
	NUI_SKELETON_POSITION_TRACKING_STATE j1state = position.eSkeletonPositionTrackingState[j1];
	NUI_SKELETON_POSITION_TRACKING_STATE j2state = position.eSkeletonPositionTrackingState[j2];

	if(j1state == NUI_SKELETON_POSITION_TRACKED && j2state == NUI_SKELETON_POSITION_TRACKED)
	{
		cvLine(Skeleton, points[j1], points[j2], CV_RGB(0,0,255),3,8,0);
		cvCircle(Skeleton, points[j2], 10,  CV_RGB(255,0,0), 1, 8, 0);
	}
}

void catchHand(const NUI_SKELETON_DATA &position, NUI_SKELETON_POSITION_INDEX lHand, NUI_SKELETON_POSITION_INDEX rHand, IplImage *Skeleton)
{
	NUI_SKELETON_POSITION_TRACKING_STATE lHandstate = position.eSkeletonPositionTrackingState[lHand];
	NUI_SKELETON_POSITION_TRACKING_STATE rHand2state = position.eSkeletonPositionTrackingState[rHand];

	CvPoint lLT = {points[lHand].x-50,points[lHand].y-50},lRB = {points[lHand].x+50,points[lHand].y+50};
	CvPoint rLT = {points[rHand].x-50,points[rHand].y-50},rRB = {points[rHand].x+50,points[rHand].y+50};

	//if(lHandstate == NUI_SKELETON_POSITION_TRACKED && rHand2state == NUI_SKELETON_POSITION_TRACKED)//매칭이 부정확하면 그리지 않음
	//{
		cvRectangle(Skeleton, lLT, lRB, CV_RGB(255,0,0),2,8,0);
		cvRectangle(Skeleton, rLT, rRB, CV_RGB(255,0,0),2,8,0);
	//}
}

void drawSkeleton(const NUI_SKELETON_DATA &position, IplImage *Skeleton)
{
	for(int i =0; i<NUI_SKELETON_POSITION_COUNT;++i)
	{
		points[i]=SkeletonToScreen(position.SkeletonPositions[i]);
	}

	drawBone(position,NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT,Skeleton);
	drawBone(position,NUI_SKELETON_POSITION_ELBOW_RIGHT,NUI_SKELETON_POSITION_WRIST_RIGHT,Skeleton);
	drawBone(position,NUI_SKELETON_POSITION_WRIST_RIGHT,NUI_SKELETON_POSITION_HAND_RIGHT,Skeleton);

	drawBone(position,NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT, Skeleton);
	drawBone(position,NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT, Skeleton);
	drawBone(position,NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT, Skeleton);

	catchHand(position,NUI_SKELETON_POSITION_HAND_LEFT, NUI_SKELETON_POSITION_HAND_RIGHT, Skeleton);
}

int createDepthSkeletonImage(HANDLE h, IplImage *Depth, IplImage *Skeleton)
{
	NUI_SKELETON_FRAME skeletonFrame ={0};
	const NUI_IMAGE_FRAME *pImageFrame = NULL;
	HRESULT hr;

	IplImage *Skeleton_clear = cvCreateImage(cvSize(WINDOW_WIDTH,WINDOW_HEIGHT), IPL_DEPTH_8U,4);
	cvCopy(Skeleton_clear, Skeleton);

	hr = NuiSkeletonGetNextFrame(0,&skeletonFrame);

	if(FAILED(hr))
	{
		return -1;
	}

	hr = NuiImageStreamGetNextFrame(h, 1000, &pImageFrame);

	if(FAILED(hr))
	{
		printf("Creating DepthImage is Failed\n");
		return -1;
	}
	
	NuiTransformSmooth(&skeletonFrame,NULL);


	INuiFrameTexture *pTexture = pImageFrame->pFrameTexture;
	NUI_LOCKED_RECT LockedRect;

	pTexture->LockRect(0, &LockedRect, NULL, 0);

	if(LockedRect.Pitch != 0)
	{
		BYTE *pBuffer = (BYTE*)LockedRect.pBits;
		RGBQUAD *rgbrun = rgb;
		USHORT *pBufferRun = (USHORT*)pBuffer;

		for(int y = 0; y <WINDOW_HEIGHT; y++)
		{
			for(int x = 0; x < WINDOW_WIDTH; x++)
			{
				RGBQUAD quad = Nui_ShortToQuad_Depth(*pBufferRun);
				pBufferRun++;
				*rgbrun = quad;
				rgbrun++;
			}
		}

		for(int i=0; i<NUI_SKELETON_COUNT;i++)//skeletondata used to draw skeleton
		{
			NUI_SKELETON_TRACKING_STATE state = skeletonFrame.SkeletonData[i].eTrackingState;

			if(NUI_SKELETON_TRACKED == state)
			{
				drawSkeleton(skeletonFrame.SkeletonData[i], Depth);
			}
		}

		cvSetData(Depth,(BYTE*)rgb,Depth->widthStep);
		cvShowImage("Depth & Skeleton Image", Depth);
	}
	
	cvReleaseImage(&Skeleton_clear);
	NuiImageStreamReleaseFrame(h, pImageFrame);
	return 0;
}







int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE colorStreamHandle;
	HANDLE depthStreamHandle;
	HANDLE nextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE NextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE nextSkeletonFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	HRESULT hr;

	InitializeKinect();

	IplImage *Color = cvCreateImage(cvSize(WINDOW_WIDTH,WINDOW_HEIGHT),IPL_DEPTH_8U,4);
	IplImage *Skeleton = cvCreateImage(cvSize(WINDOW_WIDTH,WINDOW_HEIGHT),IPL_DEPTH_8U,4);
	IplImage *Depth = cvCreateImage(cvSize(WINDOW_WIDTH, WINDOW_HEIGHT), IPL_DEPTH_8U, 4);

	cvNamedWindow("Color Image",CV_WINDOW_AUTOSIZE);

	hr=NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480,0,2,nextColorFrameEvent,&colorStreamHandle);//Color Image

	if(FAILED(hr))
	{
		cout<<"Could not open ImageStream"<<endl;
		return hr;
	}

	hr = NuiSkeletonTrackingEnable(nextSkeletonFrameEvent,0);//Skeleton Image

	if(FAILED(hr))
	{
		cout<<"Could not open SkeletonStream"<<endl;
		return hr;
	}

	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_640x480, 0, 2,//Depth Image
		NextDepthFrameEvent, &depthStreamHandle);


	if(FAILED(hr))
	{
		cout<<"Could not open DepthStream"<<endl;
		return hr;
	}

	

	while(1)
	{
		WaitForSingleObject(nextColorFrameEvent,0);
		createRGBImage(colorStreamHandle,Color);

		WaitForSingleObject(nextSkeletonFrameEvent,0);
		//createSkeleton(Skeleton);

		WaitForSingleObject(NextDepthFrameEvent, 0);//INFINITE
		createDepthSkeletonImage(depthStreamHandle,Depth, Skeleton);

		

		if(cvWaitKey(10)==0x001b)
		{
			break;
		}
	}
	NuiShutdown();   

	cvReleaseImageHeader(&Color);
	cvReleaseImageHeader(&Depth);
	cvDestroyAllWindows();
	return 0;
}
/*
현재 이 코드는 컬러이미지 & 그레이스케일이미지 & 스켈레톤이미지

여기서 그레이스케일 이미지 존재 가치? 쓸모없어 제거 ㄱㄱ


결과적으로 내가 사용하고자하는것은 먼저
1. 컬러스트림 <- 원본영상을 보여주는것이 좋을듯하다는 의견에 따라
2. 뎁스스트림 <- 결과적으로 난 이걸 사용해야한다 그러므로 사용해야됨 ㅎㅎ
3. 스켈레톤이미지 <- 이건 당연히 필요하고 결과적으로 나의 어깨관절 - 팔꿈치 - 손목 - 손 까지 이어지는 
                    관절정보를이용하여야 한다.

고찰
1. 뎁스이미지와 스켈레톤이미지가 생성된다 어떤놈을 인자로주어 하나의 영상으로 만들어낼것인가?

1-1. 뎁스가 모체가된다

1-2. 스켈레톤이 모체가된다.

*/