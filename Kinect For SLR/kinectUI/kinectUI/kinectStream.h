#pragma once

// ������� �߰�
#include <NuiApi.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>

// �ػ� ����
#define KINECT_WIDTH	640
#define KINECT_HEIGHT	480

// Finger
#define HAND_RECT_WIDTH 180 //HAND_JOINT�� �������� ���Ե� �簢���� WIDTH
#define HAND_RECT_HEIGHT 160 //HAND_JOINT�� �������� ���Ե� �簢���� HEIGHT
#define HAND_RECT_WIDTH_CENTER 90 //HAND_JOINT�� �������� �簢���� Horizontal ����
#define HAND_RECT_UP_TO_DOWN 80 //HAND_JOINT�� �������� �簢���� ���������� �Ʒ��� �Ÿ�

#define RIGHT_HAND 0 //�ճ��� �׸��� �޼����� ������������ Flag
#define LEFT_HAND 1

#define FINGERTIPTHRESHOLD 2500 //���߽������� �ճ������� �Ÿ� �Ӱ谪  " (�߽�-�ճ�)^2 "

// cv ���ӽ����̽� ���
using namespace cv;

class CKinectStream
{
public:
	// ������
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

	// ������ x, y���� Point2d������ ����(double-type)
	Point2d m_jointData[NUI_SKELETON_POSITION_COUNT];

	// �Լ�
	// �ʱ�ȭ
	bool InitializeKinect();
	// /
	void CloseKinect();

	// Color
	// Color ���� �ʱ�ȭ �� stream open
	HRESULT OpenColorStream();
	// Color Image ����
	HRESULT CreateRGBImage(HANDLE h, IplImage* Color);

	// Depth
	RGBQUAD Nui_ShortToQuad_Depth(USHORT s);
	// Depth ���� �ʱ�ȭ �� stream open
	HRESULT OpenDepthStream();
	// Depth Image ����
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