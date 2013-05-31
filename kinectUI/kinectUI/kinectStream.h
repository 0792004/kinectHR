#pragma once

// 헤더파일 추가
#include <NuiApi.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>

// 해상도 정의
#define KINECT_WIDTH	640
#define KINECT_HEIGHT	480

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
	CvMemStorage* m_storageRight;
	CvMemStorage* m_storageLeft;
	CvSeq* m_rightPtSeq;
	CvSeq* m_leftPtSeq;

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
	void DrawBone(const NUI_SKELETON_DATA &position, NUI_SKELETON_POSITION_INDEX j1, NUI_SKELETON_POSITION_INDEX j2, IplImage *img);
	void DrawHandRect(const NUI_SKELETON_DATA &position, NUI_SKELETON_POSITION_INDEX lHand, NUI_SKELETON_POSITION_INDEX rHand, IplImage *img);
	void DrawSkeleton(const NUI_SKELETON_DATA &position, IplImage *img);
	void ApplySkeleton(IplImage *img);

	// Finger
	bool CheckLine(CvPoint pt1,CvPoint pt2);
	void DrawConvexHull(IplImage *img);

	// Get
	Point2d GetJointData(int idx);
};