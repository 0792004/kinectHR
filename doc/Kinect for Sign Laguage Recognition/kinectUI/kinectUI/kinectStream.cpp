#include "stdafx.h"
#include "kinectStream.h"

bool CKinectStream::InitializeKinect()
{
	HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR
		| NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX
		| NUI_INITIALIZE_FLAG_USES_SKELETON);

	if (FAILED(hr))
		return false;
	else
		return true;
}

void CKinectStream::CloseKinect()
{
	cvReleaseImageHeader(&m_colorIpl);
	cvReleaseImageHeader(&m_depthIpl);
	this->m_colorStreamHandle = NULL;
	this->m_depthStreamHandle = NULL;
	this->m_nextColorFrameEvent = NULL;
	this->m_nextDepthFrameEvent = NULL;
	NuiShutdown();
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
	USHORT playerIndex = NuiDepthPixelToPlayerIndex(s);
	BYTE b = 255 - (BYTE)(256 * realDepth / (0x0fff));

	if(b > 180 && b < 250)
		;//내가 필요로 하는 값 [원래 8bit 값으로 표현]
	else
		b = 255;//내가 원하는 값이 아닌경우 검은색으로 표현

	RGBQUAD q;
	q.rgbRed = q.rgbGreen = q.rgbBlue = ~b;
	/*switch (playerIndex)
	{
	case 0: case 7:
	q.rgbRed = q.rgbGreen = q.rgbBlue = 0;
	break;

	case 1: case 2: case 3:
	case 4: case 5: case 6:
	q.rgbRed = 0;
	q.rgbGreen = b;
	q.rgbBlue = 0;
	break;

	default:
	q.rgbRed = q.rgbGreen = q.rgbBlue = 0;
	}*/
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

void CKinectStream::DrawBone(NUI_SKELETON_POSITION_INDEX j1, NUI_SKELETON_POSITION_INDEX j2, IplImage *Skeleton)
{
	cvLine(Skeleton, m_skeletonPoints[j1], m_skeletonPoints[j2], CV_RGB(255, 0, 0), 3, 8, 0);
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

void CKinectStream::GetJointData(const NUI_SKELETON_DATA &position)
{
	for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
		m_skeletonPoints[i] = SkeletonToScreen(position.SkeletonPositions[i]);
}

void CKinectStream::GetSkeletonPoints(const NUI_SKELETON_DATA &position)
{
	// 가슴, 왼쪽 팔꿈치, 손, 오른쪽 팔꿈치, 손의 좌표값 저장
	m_jointData[NUI_SKELETON_POSITION_SPINE] = 
		Point2d(
		position.SkeletonPositions[NUI_SKELETON_POSITION_SPINE].x,
		position.SkeletonPositions[NUI_SKELETON_POSITION_SPINE].y
		);

	m_jointData[NUI_SKELETON_POSITION_ELBOW_LEFT] = 
		Point2d(
		position.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT].x,
		position.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT].y
		);

	m_jointData[NUI_SKELETON_POSITION_HAND_LEFT] = 
		Point2d(
		position.SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].x,
		position.SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].y
		);

	m_jointData[NUI_SKELETON_POSITION_ELBOW_RIGHT] = 
		Point2d(
		position.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT].x,
		position.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT].y
		);

	m_jointData[NUI_SKELETON_POSITION_HAND_RIGHT] =
		Point2d(
		position.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].x,
		position.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].y
		);
}

void CKinectStream::DrawSkeleton(IplImage *img)
{
	// skleton 뼈대 그리기

	// 몸통
	DrawBone(NUI_SKELETON_POSITION_HEAD, NUI_SKELETON_POSITION_SHOULDER_CENTER, img);
	DrawBone(NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_LEFT, img);
	DrawBone(NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_RIGHT, img);
	DrawBone(NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_SPINE, img);
	DrawBone(NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_SPINE, img);

	// 오른팔
	DrawBone(NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT, img);
	DrawBone(NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT, img);
	DrawBone(NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT, img);

	// 왼팔
	DrawBone(NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT, img);
	DrawBone(NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT, img);
	DrawBone(NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT, img);
}

void CKinectStream::ApplySkeleton(IplImage *img)
{
	NUI_SKELETON_FRAME skeletonFrame = {0};
	HRESULT hr = NuiSkeletonGetNextFrame(0,&skeletonFrame);

	// if(FAILED(hr)) return -1;

	NuiTransformSmooth(&skeletonFrame, NULL);

	for(int i=0; i<NUI_SKELETON_COUNT; i++)
	{
		NUI_SKELETON_TRACKING_STATE state = skeletonFrame.SkeletonData[i].eTrackingState;

		if(NUI_SKELETON_TRACKED == state)
		{
			GetSkeletonPoints(skeletonFrame.SkeletonData[i]);
			GetJointData(skeletonFrame.SkeletonData[i]);
		}
	}
}

bool FingerTipThreshold(CvPoint pt1, CvPoint pt2)
{
	if( ((pt1.x - pt2.x) * (pt1.x - pt2.x)) + ((pt1.y - pt2.y) * (pt1.y - pt2.y)) > FINGERTIPTHRESHOLD )
		return true;
	return false;
}

void CKinectStream::DrawFingerTip(IplImage *cloneImage, IplImage* handImage, int HAND)
{
	CvSeq* contours;
	CvSeq* hull;
	CvSeq* defects;

	CvPoint handPt;//왼손과 오른손 둘중하나로 보정하기위해 만듬
	CvPoint wristPt;

	CvPoint previousDepthPt;

	CvMemStorage* storage = cvCreateMemStorage(0);

	switch(HAND)
	{
	case RIGHT_HAND:
		handPt = m_skeletonPoints[NUI_SKELETON_POSITION_HAND_RIGHT];
		wristPt = m_skeletonPoints[NUI_SKELETON_POSITION_WRIST_RIGHT];
		break;

	case LEFT_HAND:
		handPt = m_skeletonPoints[NUI_SKELETON_POSITION_HAND_LEFT];
		handPt.x -= 10;
		wristPt = m_skeletonPoints[NUI_SKELETON_POSITION_WRIST_LEFT];
		break;
	}

	cvFindContours(handImage, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);//여기에서 외곽선을 따내고 contours에넣어준다.

	if(contours != NULL)
	{
		contours = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 3, 2);

		//cvDrawContours(cloneImage, contours, CV_RGB(255,255,255), CV_RGB(255,255,255), 100, 1);

		hull = cvConvexHull2(contours, 0, CV_CLOCKWISE, 0 );
		defects = cvConvexityDefects(contours, hull, NULL);

		for(;defects;defects = defects->h_next)
		{
			int total = defects->total;
			if(!total)
				continue;

			previousDepthPt = cvPoint(0,0);//만약 이전값을 가질 필요가 없을 경우 주석처리해도 무방

			CvConvexityDefect* defectArray = (CvConvexityDefect*)malloc(sizeof(CvConvexityDefect) * total);
			cvCvtSeqToArray(defects, defectArray,CV_WHOLE_SEQ);
			int finNum = 0;

			for(int i = 0; i < total; i++)
			{

				CvPoint fingerTipPt = *(defectArray[i].start);//시작점을 Defects의 start 부분부터 지정
				//CvPoint convexPt = *(defectArray[i].end//시작점을 Defects의 end 부분부터 지정
				CvPoint depthPt = *(defectArray[i].depth_point);

				if(HAND == LEFT_HAND)//왼손의 경우 좀 이상하게 오른쪽으로 치우쳐져있어 보정
					fingerTipPt.x +=10;

				fingerTipPt.x += handPt.x - HAND_RECT_WIDTH_CENTER;//ROI에서 뽑아낸 값을 실제 cloneImage로보정해주는 작업
				fingerTipPt.y += handPt.y - HAND_RECT_UP_TO_DOWN;

				depthPt.x += handPt.x - HAND_RECT_WIDTH_CENTER;
				depthPt.y += handPt.y - HAND_RECT_UP_TO_DOWN;

				if(FingerTipThreshold(fingerTipPt, handPt) && fingerTipPt.y < handPt.y + 20){
					//cvCircle(cloneImage, depthPt, 5, CV_RGB(255,255,255), -1, 8, 0);
					cvCircle(cloneImage, fingerTipPt, 5, CV_RGB(255,255,255), -1, 8, 0);
					cvLine(cloneImage, wristPt, fingerTipPt, CV_RGB(255,255,255));
					finNum++;
				}
			}
			free(defectArray);	
		}
	}

	cvReleaseMemStorage(&storage);
}


void CKinectStream::SetHandROI(IplImage *cloneImage)
{
	m_rightHandImage = cvCreateImage(cvSize(HAND_RECT_WIDTH, HAND_RECT_HEIGHT), IPL_DEPTH_8U, 1);
	m_leftHandImage = cvCreateImage(cvSize(HAND_RECT_WIDTH, HAND_RECT_HEIGHT), IPL_DEPTH_8U, 1);

	m_rightHandImage = (IplImage*)cvClone(cloneImage);
	m_leftHandImage = (IplImage*)cvClone(cloneImage);


	if( m_skeletonPoints[NUI_SKELETON_POSITION_HAND_RIGHT].x > 0 && m_skeletonPoints[NUI_SKELETON_POSITION_HAND_RIGHT].x < KINECT_WIDTH &&
		m_skeletonPoints[NUI_SKELETON_POSITION_HAND_RIGHT].y > 0 && m_skeletonPoints[NUI_SKELETON_POSITION_HAND_RIGHT].y < KINECT_HEIGHT)
	{
		cvSetImageROI(m_rightHandImage, cvRect(m_skeletonPoints[NUI_SKELETON_POSITION_HAND_RIGHT].x - HAND_RECT_WIDTH_CENTER,
			m_skeletonPoints[NUI_SKELETON_POSITION_HAND_RIGHT].y - HAND_RECT_UP_TO_DOWN,
			HAND_RECT_WIDTH,
			HAND_RECT_HEIGHT));
	}

	if( m_skeletonPoints[NUI_SKELETON_POSITION_HAND_LEFT].x > 0 && m_skeletonPoints[NUI_SKELETON_POSITION_HAND_LEFT].x < KINECT_WIDTH &&
		m_skeletonPoints[NUI_SKELETON_POSITION_HAND_LEFT].y > 0 && m_skeletonPoints[NUI_SKELETON_POSITION_HAND_LEFT].y < KINECT_HEIGHT )
	{

		cvSetImageROI(m_leftHandImage, cvRect(m_skeletonPoints[NUI_SKELETON_POSITION_HAND_LEFT].x - HAND_RECT_WIDTH_CENTER,
			m_skeletonPoints[NUI_SKELETON_POSITION_HAND_LEFT].y - HAND_RECT_UP_TO_DOWN,
			HAND_RECT_WIDTH,
			HAND_RECT_HEIGHT));

	}

	DrawFingerTip(cloneImage, m_rightHandImage, RIGHT_HAND);
	DrawFingerTip(cloneImage, m_leftHandImage, LEFT_HAND);

	cvReleaseImage(&m_rightHandImage);
	cvReleaseImage(&m_leftHandImage);
}

void CKinectStream::CreateCloneImage(IplImage *Depth)
{
	//OpenCV내의 함수를 사용하기위해 키넥트로 부터 얻어지는 4channel이미지중 1channel만 따오기위한 IplImage 변수
	m_cloneImage = cvCreateImage(cvGetSize(Depth), IPL_DEPTH_8U, 1);
	//처음에 분리해주는이유는 이후에 이 값을 통해 ROI를 설정후 ConvexityDefects를 찾아내기위해
	cvSplit(Depth, m_cloneImage, NULL, NULL, NULL);

	//잡음을 없애주기 위한 Mopology연산
	//안하면 잡음(조그만한 점)이 convexhull들어가게 됨
	cvMorphologyEx(m_cloneImage, m_cloneImage, 0, 0, CV_MOP_OPEN, 1);
	cvMorphologyEx(m_cloneImage, m_cloneImage, 0, 0, CV_MOP_CLOSE, 2);
}