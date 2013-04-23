// CkinectLearn.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CkinectLearn.h"
#include "afxdialogex.h"
#include <vector>

// CkinectLearn 대화 상자입니다.

IMPLEMENT_DYNAMIC(CkinectLearn, CDialogEx)

	CkinectLearn::CkinectLearn(CWnd* pParent /*=NULL*/)
	: CDialogEx(CkinectLearn::IDD, pParent)
{
	isLearnMode = false;
}

CkinectLearn::~CkinectLearn()
{
}

void CkinectLearn::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_COLOR, m_sColor);
	DDX_Control(pDX, IDC_STATIC_DEPTH, m_sDepth);
	DDX_Control(pDX, IDC_EDIT_SLNAME, m_eSLName);
}


BEGIN_MESSAGE_MAP(CkinectLearn, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CkinectLearn::OnBnClickedButtonRun)
	ON_BN_CLICKED(IDC_BUTTON_LEARN, &CkinectLearn::OnBnClickedButtonLearn)
END_MESSAGE_MAP()


// CkinectLearn 메시지 처리기입니다.


void CkinectLearn::OnTimer(UINT_PTR nIDEvent)
{
	if (!FAILED(kinectStream.CreateRGBImage(kinectStream.m_colorStreamHandle, kinectStream.m_colorIpl)))
	{
		if (!FAILED(kinectStream.CreateDepthImage(kinectStream.m_depthStreamHandle, kinectStream.m_depthIpl)))
		{
			kinectStream.ApplySkeleton(kinectStream.m_depthIpl);
			CDC *pDC;
			pDC = m_sColor.GetDC();

			CRect rect;
			rect.SetRect(0, 0, (int) (KINECT_WIDTH * VIEW_RATE / 100), (int) (KINECT_HEIGHT * VIEW_RATE / 100));

			cvvImage.CopyOf(kinectStream.m_colorIpl, 3);
			cvvImage.DrawToHDC(pDC->m_hDC, rect);

			pDC = m_sDepth.GetDC();
			cvvImage.CopyOf(kinectStream.m_depthIpl, 3);
			cvvImage.DrawToHDC(pDC->m_hDC, rect);
		}
	}
	/*
	{
	kinectStream.ApplySkeleton(kinectStream.m_depthIpl);
	CDC *pDC;
	pDC = m_sDepth.GetDC();

	CRect rect;
	rect.SetRect(0, 0, (int) (KINECT_WIDTH * VIEW_RATE / 100), (int) (KINECT_HEIGHT * VIEW_RATE / 100));

	cvvImage.CopyOf(kinectStream.m_depthIpl, 3);
	cvvImage.DrawToHDC(pDC->m_hDC, rect);
	}*/

	CDialogEx::OnTimer(nIDEvent);
}


void CkinectLearn::OnBnClickedButtonRun()
{
	kinectStream.InitializeKinect();
	kinectStream.OpenColorStream();
	kinectStream.OpenDepthStream();
	SetTimer(1, 1000 / FPS, NULL);
}


void CkinectLearn::OnBnClickedButtonLearn()
{
	if (!isLearnMode)
		isLearnMode = true;
	else
	{
		isLearnMode = false;
		CString path;
		path += "C:\\Users\\kwon\\Documents\\";
		CString slName;
		m_eSLName.GetWindowTextW(slName);
		slName += ".txt";
		path += slName;
		
		fout.open(path);
		if (fout.is_open())
		{
			for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
			{
				vector<Point2d>::const_iterator pos;
				vector<Point2d> &raw = kinectStream.rawData[i];
				if (raw.size() > 0)
				{
					for (pos = raw.begin(); pos != raw.end(); ++pos)
						fout << pos->x << ',';
					fout << endl;

					for (pos = raw.begin(); pos != raw.end(); ++pos)
						fout << pos->y << ',';
					fout << endl;
				}
			}
		}
		fout.close();
	}
}
