/*               */
/* 한성대        */
/* 컴퓨터공학과  */
/* 0892013      */
/* 문대식       */
/*             */

#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>

#include <Windows.h>
#include <tchar.h>
#include <NuiApi.h>
#include <stdio.h>

#include "fcc.h"


#define DEPTH_WIDTH 640
#define DEPTH_HEIGHT 480

RGBQUAD rgb[640*480];
CvPoint pt;//(0)

void InitializeKinect()
{
	bool NotConnectKinect;

	do
	{
		HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX || NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE);
		
		if(FAILED(hr))
		{
			system("cls");
			printf("\nnFailed to Connect!\n");
			NotConnectKinect = true;
			system("PAUSE");
		}
		else
		{
			printf("Connect Established!\n");
			NotConnectKinect = false;
		}
	}
	while(NotConnectKinect);
}

/*Kinect 로부터 전해져오는 Depth값을 0~255사이의 값으로 변환*/
RGBQUAD Nui_ShortToQuad_Depth(SHORT s)
{
	USHORT realDepth = (s&0xfff8) >> 3;//Shift 연산을 통해 realDepth값으로 변환
	BYTE I= 255-(BYTE)(256*realDepth / (0x0fff));//뎁스정보를 255~0사이값으로 변환[역수임을 기억]

	if(I > 210 && I < 250)//215미만값이라면 즉 내가 얻고자하는 영상정보의 값보다 멀어진다면
	{
		I = 0;//그값은 필요없는 값이므로 제거한다.
	}
	else
		I = 255;

	RGBQUAD q;
	q.rgbRed = q.rgbBlue = q.rgbGreen = ~I;
	return q;
}


/* 전해진 Depth 정보를 기반으로 Image 만들어냄 */
int createDepthImage(HANDLE h, IplImage *Depth)
{
	int finnum =0;//(0)
	CvMemStorage* storage = cvCreateMemStorage(0);//(0)
	CvSeq* ptseq = cvCreateSeq(CV_SEQ_KIND_GENERIC | CV_32SC2, sizeof(CvContour), sizeof(CvPoint),storage);//(0)
	const NUI_IMAGE_FRAME *pImageFrame = NULL;
	//////////////////////////////////////////////////////////////
	IplImage* appImage = NULL;//근사화 이미지를 위해 기본 Depth값의 1channel만 따오기

	appImage = cvCreateImage(cvGetSize(Depth),IPL_DEPTH_8U, 1);

	cvSplit(Depth, appImage, NULL, NULL, NULL);
	//////////////////////////////////////////////////////////////

	HRESULT hr = NuiImageStreamGetNextFrame(h, 1000, &pImageFrame);

	if(FAILED(hr))
	{
		printf("Creating DepthImage is Failed\n");
		return -1;
	}
	
	INuiFrameTexture *pTexture = pImageFrame->pFrameTexture;
	NUI_LOCKED_RECT LockedRect;

	pTexture->LockRect(0, &LockedRect, NULL, 0);

	if(LockedRect.Pitch != 0)
	{
		BYTE *pBuffer = (BYTE*)LockedRect.pBits;
		RGBQUAD *rgbrun = rgb;
		USHORT *pBufferRun = (USHORT*)pBuffer;

		for(int y = 0; y <DEPTH_HEIGHT; y++)
		{
			for(int x = 0; x < DEPTH_WIDTH; x++)
			{
				RGBQUAD quad = Nui_ShortToQuad_Depth(*pBufferRun);
				pBufferRun++;
				*rgbrun = quad;
				rgbrun++;
				//////////////////////(0)
				if(quad.rgbRed != 0)
				{
					pt.x = x;
					pt.y = y;
					cvSeqPush(ptseq, &pt);
				}
				////////////////////(0)
			}
		}
		cvSetData(Depth,(BYTE*)rgb,Depth->widthStep);



		int det = 2;
		CvPoint cvp[2];


		find_connected_components(appImage, 1, 100, &det, NULL, cvp);//인자를 줘서 

		///////////////////////////////////////////////////////(0)
		
		pt.x = 0; pt.y = 0;
		CvSeq* hull;
		CvPoint end_pt;
		
	
		if(ptseq->total != 0)
		{
			hull = cvConvexHull2(ptseq, 0, CV_COUNTER_CLOCKWISE, 0 );

			for(int x = 0; x < hull->total; x++)
			{
				CvPoint hull_pt = **CV_GET_SEQ_ELEM(CvPoint*, hull, x);
	
				if(pt.x == 0 && pt.y ==0)
				{
					pt = hull_pt;
					end_pt = pt;
				}
				cvLine(appImage, pt, hull_pt, CV_RGB(255,255,255), 2 ,8);
				pt = hull_pt;
	
				if(x==hull->total-1)
					cvLine(appImage, hull_pt, end_pt, CV_RGB(255,255,255), 2, 8);
				finnum++;
			}
		}
		////////////////////////////////////////////////////////(0)
		//지금 남은 과제가 ptseq내부에 계속 들어가기만하고 해제가 안되기떄문에
		//문제가 생긴다 계속 엄청난양의 공간으로 그림을 그리니 느려질수 밖에 그러므로 잘 해제할것
		
		cvShowImage("DepthImage", appImage);
	}
	printf("%d\n", finnum);
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&appImage);
	NuiImageStreamReleaseFrame(h, pImageFrame);
	return 0;
}




int _tmain(int argc, _TCHAR argv[])
{
	HANDLE DepthStreamHandle;
	HANDLE NextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HRESULT hr;
	IplImage *Depth = cvCreateImage(cvSize(DEPTH_WIDTH, DEPTH_HEIGHT), IPL_DEPTH_8U, 4);

	
	
	cvNamedWindow("DepthImage", CV_WINDOW_AUTOSIZE);

	InitializeKinect();


	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_640x480, 0, 2,
		NextDepthFrameEvent, &DepthStreamHandle);

	NuiImageStreamSetImageFrameFlags(DepthStreamHandle, NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE);

	if(FAILED(hr))
	{
		printf("Could not open DepthStream\n");
		return hr;
	}
	
	while(1)
	{
		WaitForSingleObject(NextDepthFrameEvent, INFINITE);
		createDepthImage(DepthStreamHandle,Depth);


		if(cvWaitKey(10) == 0x001b)
		{
			break;
		}
	}

	
	cvReleaseImage(&Depth);
	cvDestroyWindow("DepthImage");
	NuiShutdown();
	return 0;
}
