#pragma once

// 헤더파일 추가
#include <NuiApi.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>

// 해상도 정의
#define KINECT_WIDTH	640
#define KINECT_HEIGHT	480

// Finger
#define HAND_RECT_WIDTH 180 //HAND_JOINT를 기준으로 포함될 사각형의 WIDTH
#define HAND_RECT_HEIGHT 160 //HAND_JOINT를 기준으로 포함될 사각형의 HEIGHT
#define HAND_RECT_WIDTH_CENTER 90 //HAND_JOINT를 기준으로 사각형의 Horizontal 중점
#define HAND_RECT_UP_TO_DOWN 80 //HAND_JOINT를 기점으로 사각형의 위에서부터 아래로 거리

#define RIGHT_HAND 0 //손끝을 그릴때 왼손인지 오른손인지의 Flag
#define LEFT_HAND 1

#define FINGERTIPTHRESHOLD 2500 //손중심점에서 손끝까지의 거리 임계값  " (중심-손끝)^2 "

// cv 네임스페이스 사용
using namespace cv;

class CKinectStream
{
public:
	// 데이터
	// Color
	IplImage *m_colorIpl;
	HANDLE m_colorStreamHandle;
	HANDLE m_nextColorFrameEvent;
	HRESULT m_colorHr;

	// Depth
	RGBQUAD rgb[KINECT_WIDTH * KINECT_HEIGHT];
	IplImage *m_depthIpl;
	HANDLE m_depthStreamHandle;
	HANDLE m_nextDepthFrameEvent;
	HRESULT m_depthHr;

	// Skeleton
	CvPoint m_skeletonPoints[NUI_SKELETON_POSITION_COUNT];

	// Finger
	IplImage* m_rightHandImage;
	IplImage* m_leftHandImage;
	IplImage* m_cloneImage;

	// 관절의 x, y값을 Point2d형으로 저장(double-type)
	Point2d m_jointData[NUI_SKELETON_POSITION_COUNT];

	// 함수
	// 초기화
	bool InitializeKinect();
	// /
	void CloseKinect();

	// Color
	// Color 변수 초기화 및 stream open
	HRESULT OpenColorStream();
	// Color Image 생성
	HRESULT CreateRGBImage(HANDLE h, IplImage* Color);

	// Depth
	RGBQUAD Nui_ShortToQuad_Depth(USHORT s);
	// Depth 변수 초기화 및 stream open
	HRESULT OpenDepthStream();
	// Depth Image 생성
	HRESULT CreateDepthImage(HANDLE h, IplImage* Depth);

	// Skeleton
	CvPoint SkeletonToScreen(Vector4 skeletonPoint);
	void DrawBone(NUI_SKELETON_POSITION_INDEX j1, NUI_SKELETON_POSITION_INDEX j2, IplImage *img);
	void DrawSkeleton(IplImage *img);
	void ApplySkeleton(IplImage *img);
	void GetSkeletonPoints(const NUI_SKELETON_DATA &position);
	void GetJointData(const NUI_SKELETON_DATA &position);

	// Finger
	void DrawFingerTip(IplImage *cloneImage, IplImage* handImage, int HAND);
	void SetHandROI(IplImage *cloneImage);
	void CreateCloneImage(IplImage *Depth);

	// Get
	Point2d GetJointData(int idx);
};