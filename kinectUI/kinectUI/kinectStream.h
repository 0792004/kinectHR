#pragma once

// 헤더파일 추가
#include <NuiApi.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>

// 해상도 정의
#define KINECT_WIDTH	640
#define KINECT_HEIGHT	480
#define VIEW_RATE		70
#define FPS				30

// cv 네임스페이스 사용
using namespace cv;

class CKinectStream
{
public:

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
	NUI_SKELETON_DATA m_skeletonData;
	CvPoint m_skeletonPoints[NUI_SKELETON_POSITION_COUNT];

	// Vetor
	vector<Point2d> rawData[NUI_SKELETON_POSITION_COUNT];

	// 함수
	// 초기화
	void InitializeKinect();

	//
	void CloseKinect();

	// Color
	HRESULT OpenColorStream();
	HRESULT CreateRGBImage(HANDLE h, IplImage* Color);

	// Depth
	RGBQUAD Nui_ShortToQuad_Depth(USHORT s);
	HRESULT OpenDepthStream();
	HRESULT CreateDepthImage(HANDLE h, IplImage* Depth);

	// Skeleton
	CvPoint SkeletonToScreen(Vector4 skeletonPoint);
	void DrawBone(const NUI_SKELETON_DATA &position, NUI_SKELETON_POSITION_INDEX j1, NUI_SKELETON_POSITION_INDEX j2, IplImage *img);
	void CatchHand(const NUI_SKELETON_DATA &position, NUI_SKELETON_POSITION_INDEX lHand, NUI_SKELETON_POSITION_INDEX rHand, IplImage *img);
	void DrawSkeleton(const NUI_SKELETON_DATA &position, IplImage *img);
	void ApplySkeleton(IplImage *img);

	// get
	vector<Point2d> GetSkeletonPositon(NUI_SKELETON_POSITION_INDEX idx);
};