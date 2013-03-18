#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>

#include <Windows.h>
#include <tchar.h>
#include <NuiApi.h>

#include <stdio.h>

#define DEPTH_WIDTH		640
#define DEPTH_HEIGHT	480

#define COLOR_WIDTH		640
#define COLOR_HEIGHT	480

void InitializeKinect();
void createIRImage(HANDLE h, IplImage *InfraRed);

RGBQUAD Nui_ShortToQuad_IR(USHORT s);
RGBQUAD m_irWk[COLOR_WIDTH * COLOR_HEIGHT];

int _tmain(int argc, _TCHAR argv[])
{
	HANDLE IRStreamHandle;
	HANDLE nextIRFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HRESULT hr;

	InitializeKinect();

	IplImage *InfraRed = cvCreateImage(cvSize(DEPTH_WIDTH, DEPTH_HEIGHT), IPL_DEPTH_8U, 4);

	cvNamedWindow("IR Image", CV_WINDOW_AUTOSIZE);

	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR_INFRARED, NUI_IMAGE_RESOLUTION_640x480, 0, 2,
		nextIRFrameEvent, &IRStreamHandle);

	if(FAILED(hr))
	{
		printf("Could not open IRStream\n");
		return hr;
	}

	while(1)
	{
		WaitForSingleObject(nextIRFrameEvent, 1000);
		createIRImage(IRStreamHandle, InfraRed);

		if(cvWaitKey(10) == 0x001b)
		{
			break;
		}
	}

	while(1)
	{
		WaitForSingleObject(nextIRFrameEvent, 1000);
		createIRImage(IRStreamHandle, InfraRed);

		if(cvWaitKey(10) == 0x001b)
		{
			break;
		}
	}

	NuiShutdown();

	cvReleaseImageHeader(&InfraRed);

	cvDestroyAllWindows();
	return 0;
}

void InitializeKinect()
{
	bool FailToConnect;

	do
	{
		HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH
			| NUI_INITIALIZE_FLAG_USES_SKELETON);

		if(FAILED(hr))
		{
			system("cls");
			printf("Failed to connect!\n\n");
			FailToConnect = true;
			system("PAUSE");
		}
		else
		{
			printf("Connection Established!\n\n");
			FailToConnect = false;
		}
	} while (FailToConnect);
}

void createIRImage(HANDLE h, IplImage *InfraRed)
{
	const NUI_IMAGE_FRAME *pImageFrame = NULL;
	HRESULT hr = NuiImageStreamGetNextFrame(h, 0, &pImageFrame);

	if(FAILED(hr))
	{
		return;
	}

	INuiFrameTexture *pTexture = pImageFrame->pFrameTexture;
	NUI_LOCKED_RECT LockedRect;

	pTexture->LockRect(0, &LockedRect, NULL, 0);

	if(LockedRect.Pitch != 0)
	{
		BYTE *pBuffer = (BYTE*)LockedRect.pBits;
		RGBQUAD *rgbrun = m_irWk;
		USHORT *pBufferRun = (USHORT*) pBuffer;

		for(int y = 0; y < COLOR_HEIGHT; y++)
		{
			for(int x = 0; x < COLOR_WIDTH; x++)
			{
				RGBQUAD quad = Nui_ShortToQuad_IR(*pBufferRun);
				pBufferRun++;
				*rgbrun = quad;
				rgbrun++;
			}
		}
		cvSetData(InfraRed, (BYTE*)m_irWk, InfraRed->widthStep);
		cvShowImage("IR Image", InfraRed);
	}

	NuiImageStreamReleaseFrame(h, pImageFrame);
}

RGBQUAD Nui_ShortToQuad_IR(USHORT s)
{
	USHORT pixel = s>>8;

	BYTE intensity = pixel;
	RGBQUAD q;

	q.rgbBlue = intensity;
	q.rgbGreen = intensity;
	q.rgbRed = intensity;

	return q;
}