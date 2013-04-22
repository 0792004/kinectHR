// CkinectReco.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CkinectReco.h"
#include "afxdialogex.h"

// CkinectReco 대화 상자입니다.

IMPLEMENT_DYNAMIC(CkinectReco, CDialogEx)

	CkinectReco::CkinectReco(CWnd* pParent /*=NULL*/)
	: CDialogEx(CkinectReco::IDD, pParent)
{
}

CkinectReco::~CkinectReco()
{
}

void CkinectReco::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_COLOR, m_sColor);
	DDX_Control(pDX, IDC_STATIC_DEPTH, m_sDepth);
}


BEGIN_MESSAGE_MAP(CkinectReco, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CkinectReco::OnBnClickedButtonRun)
END_MESSAGE_MAP()


// CkinectReco 메시지 처리기입니다.


void CkinectReco::OnTimer(UINT_PTR nIDEvent)
{
	if (!FAILED(kinectStream.createRGBImage(kinectStream.m_colorStreamHandle, kinectStream.m_colorIpl)))
	{
		CDC *pDC;
		pDC = m_sColor.GetDC();

		CRect rect;
		rect.SetRect(0, 0, (int) (KINECT_WIDTH * VIEW_RATE / 100), (int) (KINECT_HEIGHT * VIEW_RATE / 100));

		cvvImage.CopyOf(kinectStream.m_colorIpl, 3);
		cvvImage.DrawToHDC(pDC->m_hDC, rect);
	}

	if (!FAILED(kinectStream.createDepthImage(kinectStream.m_depthStreamHandle, kinectStream.m_depthIpl)))
	{
		kinectStream.ApplySkeleton(kinectStream.m_depthIpl);
		CDC *pDC;
		pDC = m_sDepth.GetDC();

		CRect rect;
		rect.SetRect(0, 0, (int) (KINECT_WIDTH * VIEW_RATE / 100), (int) (KINECT_HEIGHT * VIEW_RATE / 100));

		cvvImage.CopyOf(kinectStream.m_depthIpl, 3);
		cvvImage.DrawToHDC(pDC->m_hDC, rect);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CkinectReco::OnBnClickedButtonRun()
{
	kinectStream.openColorStream();
	kinectStream.openDepthStream();
	kinectStream.InitializeKinect();
	SetTimer(1, 1000 / FPS, NULL);
}
