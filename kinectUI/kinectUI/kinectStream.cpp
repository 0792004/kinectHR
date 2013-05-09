#include "stdafx.h"
#include "kinectStream.h"

void CKinectStream::InitializeKinect()
{
	bool FailToConnect;

	do
	{
		HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR
			| NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX
			| NUI_INITIALIZE_FLAG_USES_SKELETON);

		if(FAILED(hr))
		{
			FailToConnect = true;
		}
		else
		{
			FailToConnect = false;
		}
	}
	while(FailToConnect);
}

void CKinectStream::CloseKinect()
{
	cvReleaseImageHeader(&m_colorIpl);
	cvReleaseImageHeader(&m_depthIpl);
	this->m_colorStreamHandle = NULL;
	this->m_depthStreamHandle = NULL;
	this->m_nextColorFrameEvent = NULL;
	this->m_nextDepthFrameEvent = NULL;
	//NuiShutdown();
}

HRESULT CKinectStream::OpenColorStream()
{
	m_nextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_colorIpl = cvCreateImage(cvSize(KINECT_WIDTH, KINECT_HEIGHT), IPL_DEPTH_8U, 3);
	m_colorHr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480,
		0, 2, m_nextColorFrameEvent, &m_colorStreamHandle);

	if (FAILED(m_colorHr))
	{
		return -1;
	}
	else
	{
		return 0;
	}

}

HRESULT CKinectStream::CreateRGBImage(HANDLE h, IplImage* Color)
{
	const NUI_IMAGE_FRAME *pImageFrame = NULL;

	//HRESULT hr = NuiImageStreamGetNextFrame(h, 1000, &pImageFrame);
	HRESULT hr = NuiImageStreamGetNextFrame(h, 0, &pImageFrame);

	if(FAILED(hr))
	{
		return -1;
	}

	INuiFrameTexture *pTexture = pImageFrame->pFrameTexture;
	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect(0, &LockedRect, NULL, 0);

	if(LockedRect.Pitch != 0)
	{
		BYTE * pBuffer = (BYTE*)LockedRect.pBits;
		//cvSetData(Color, pBuffer, LockedRect.Pitch);
		BYTE *from, *to;
		int i;

		from = pBuffer;
		to = (BYTE*)Color->imageData;

		for (i = 0; i < (KINECT_WIDTH * KINECT_HEIGHT); i++)
		{
			memcpy(to, from, 3);
			from += 4;
			to += 3;
		}
	}

	NuiImageStreamReleaseFrame(h, pImageFrame);

	return 0;
}

RGBQUAD CKinectStream::Nui_ShortToQuad_Depth(USHORT s)
{
	USHORT realDepth = (s & 0xfff8) >> 3;
	BYTE i = 255 - (BYTE)(256 * realDepth / (0x0fff));

	RGBQUAD q;
	q.rgbRed = q.rgbGreen = q.rgbBlue = ~i;
	return q;
}

HRESULT CKinectStream::OpenDepthStream()
{
	m_nextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_depthIpl = cvCreateImage(cvSize(KINECT_WIDTH, KINECT_HEIGHT), IPL_DEPTH_8U, 3);
	m_depthHr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_640x480,
		0, 2, m_nextDepthFrameEvent, &m_depthStreamHandle);

	if (FAILED(m_depthHr))
	{
		return -1;
	}
	else
	{
		return 0;
	}

}

HRESULT CKinectStream::CreateDepthImage(HANDLE h, IplImage* Depth)
{
	const NUI_IMAGE_FRAME *pImageFrame = NULL;

	//HRESULT hr = NuiImageStreamGetNextFrame(h, 1000, &pImageFrame);
	HRESULT hr = NuiImageStreamGetNextFrame(h, 0, &pImageFrame);

	if(FAILED(hr))
	{
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

		for(int y = 0; y < KINECT_HEIGHT; y++)
		{
			for(int x = 0; x < KINECT_WIDTH; x++)
			{
				RGBQUAD quad = Nui_ShortToQuad_Depth(*pBufferRun);
				pBufferRun++;
				*rgbrun = quad;
				rgbrun++;
			}
		}

		//cvSetData(Depth, (BYTE*)rgb, Depth->widthStep);
		BYTE *from, *to;
		int i;

		from = (BYTE*)rgb;
		to = (BYTE*)Depth->imageData;

		for (i = 0; i < (KINECT_WIDTH * KINECT_HEIGHT); i++)
		{
			memcpy(to, from, 3);
			from += 4;
			to += 3;
		}
	}

	NuiImageStreamReleaseFrame(h, pImageFrame);

	return 0;
}

void CKinectStream::DrawBone(const NUI_SKELETON_DATA &position, NUI_SKELETON_POSITION_INDEX j1, NUI_SKELETON_POSITION_INDEX j2, IplImage *Skeleton)
{
	NUI_SKELETON_POSITION_TRACKING_STATE j1state = position.eSkeletonPositionTrackingState[j1];
	NUI_SKELETON_POSITION_TRACKING_STATE j2state = position.eSkeletonPositionTrackingState[j2];

	if(j1state == NUI_SKELETON_POSITION_TRACKED && j2state == NUI_SKELETON_POSITION_TRACKED)
	{
		cvLine(Skeleton, m_skeletonPoints[j1], m_skeletonPoints[j2], CV_RGB(0, 255, 0), 3, 8, 0);
	}
}

CvPoint CKinectStream::SkeletonToScreen(Vector4 skeletonPoint)
{
	LONG x, y;
	USHORT depth;

	NuiTransformSkeletonToDepthImage(skeletonPoint, &x, &y,
		&depth, NUI_IMAGE_RESOLUTION_640x480);

	float screenPointX = static_cast<float>(x);
	float screenPointY = static_cast<float>(y);

	return cvPoint(screenPointX, screenPointY);
}

void CKinectStream::CatchHand(const NUI_SKELETON_DATA &position, NUI_SKELETON_POSITION_INDEX lHand, NUI_SKELETON_POSITION_INDEX rHand, IplImage *img)
{
	NUI_SKELETON_POSITION_TRACKING_STATE lHandstate = position.eSkeletonPositionTrackingState[lHand];
	NUI_SKELETON_POSITION_TRACKING_STATE rHand2state = position.eSkeletonPositionTrackingState[rHand];

	CvPoint lLT = {m_skeletonPoints[lHand].x-50, m_skeletonPoints[lHand].y-50};
	CvPoint lRB = {m_skeletonPoints[lHand].x+50, m_skeletonPoints[lHand].y+50};
	CvPoint rLT = {m_skeletonPoints[rHand].x-50, m_skeletonPoints[rHand].y-50};
	CvPoint rRB = {m_skeletonPoints[rHand].x+50 ,m_skeletonPoints[rHand].y+50};

	cvRectangle(img, lLT, lRB, CV_RGB(255,0,0), 2, 8, 0);
	cvRectangle(img, rLT, rRB, CV_RGB(255,0,0), 2, 8, 0);
}

void CKinectStream::DrawSkeleton(const NUI_SKELETON_DATA &position, IplImage *img)
{
	for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
	{
		m_skeletonPoints[i] = SkeletonToScreen(position.SkeletonPositions[i]);
	}

	// 가슴, 왼쪽 팔꿈치, 손, 오른쪽 팔꿈치, 손의 좌표값 저장
	jointData[NUI_SKELETON_POSITION_SPINE] = 
		Point2d(
		position.SkeletonPositions[NUI_SKELETON_POSITION_SPINE].x,
		position.SkeletonPositions[NUI_SKELETON_POSITION_SPINE].y
		);

	jointData[NUI_SKELETON_POSITION_ELBOW_LEFT] = 
		Point2d(
		position.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT].x,
		position.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT].y
		);

	jointData[NUI_SKELETON_POSITION_HAND_LEFT] = 
		Point2d(
		position.SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].x,
		position.SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].y
		);

	jointData[NUI_SKELETON_POSITION_ELBOW_RIGHT] = 
		Point2d(
		position.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT].x,
		position.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT].y
		);

	jointData[NUI_SKELETON_POSITION_HAND_RIGHT] =
		Point2d(
		position.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].x,
		position.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].y
		);

	// skleton 그리기
	DrawBone(position, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT, img);
	DrawBone(position, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT, img);
	DrawBone(position, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT, img);

	DrawBone(position, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT, img);
	DrawBone(position, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT, img);
	DrawBone(position, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT, img);

	// 손을 중심으로하는 사각형 그리기
	CatchHand(position,NUI_SKELETON_POSITION_HAND_LEFT, NUI_SKELETON_POSITION_HAND_RIGHT, img);
}

void CKinectStream::ApplySkeleton(IplImage *img)
{
	NUI_SKELETON_FRAME skeletonFrame = {0};
	HRESULT hr = NuiSkeletonGetNextFrame(0,&skeletonFrame);
	/*
	if(FAILED(hr))
	{
	return -1;
	}
	*/
	NuiTransformSmooth(&skeletonFrame, NULL);

	for(int i=0; i<NUI_SKELETON_COUNT; i++)
	{
		NUI_SKELETON_TRACKING_STATE state = skeletonFrame.SkeletonData[i].eTrackingState;

		if(NUI_SKELETON_TRACKED == state)
		{
			DrawSkeleton(skeletonFrame.SkeletonData[i], img);
		}
	}
}

Point2d CKinectStream::GetSkeletonPositon(int idx)
{
	return jointData[idx];
}