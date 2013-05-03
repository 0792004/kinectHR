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
			for (int i = NUI_SKELETON_POSITION_SPINE + 1; i < NUI_SKELETON_POSITION_COUNT; i++)
			{
				vector<Point2d>::const_iterator pos;			// SPINE 외 다른 rawData의 iterator
				vector<Point2d>::const_iterator posSP;			// SPINE rawData의 iterator

				vector<Point2d> &raw = kinectStream.rawData[i];								// i번째 rawDATA
				vector<Point2d> &rawSP = kinectStream.rawData[NUI_SKELETON_POSITION_SPINE]; // SPINE의 rawDATA

				if (raw.size() > 0)
				{
					const int K = 2;		// K프레임에 한번 데이터 저장
					int cnt = 0;

					for (posSP = rawSP.begin(), pos = raw.begin(); posSP != rawSP.end(); posSP++, pos++)
					{
						cnt++;
						if (cnt == K)
						{
							cnt = 0;
							fout << (int)(GetDegree(pos->x, posSP->x, pos->y, posSP->y)) << ", " << endl;
						}
					}
					fout << endl;
					kinectStream.rawData[i].clear();

					/*for (posSP = rawSP.begin(), pos = raw.begin(); posSP != rawSP.end(); posSP++, pos++)
					{
					cnt++;
					if (cnt == K)
					{
					cnt = 0;
					fout << (int) ((pos->x) * 1000) << ", " << endl;
					}
					}
					fout << endl;

					cnt = 0;
					for (posSP = rawSP.begin(), pos = raw.begin(); posSP != rawSP.end(); posSP++, pos++)
					{
					cnt++;
					if (cnt == K)
					{
					cnt = 0;
					fout << (int) ((pos->y) * 1000) << ", " << endl;
					}
					}
					fout << endl;*/
				}
			}
			kinectStream.rawData[NUI_SKELETON_POSITION_SPINE].clear();
		}
		fout.close();
	}
}

double CkinectLearn::GetDegree(double x1, double x2, double y1, double y2)
{
	double dx = x2 - x1;
	double dy = y2 - y1;

	double rad = atan2(dx, dy);
	double degree = (rad * 180) / PI ;

	return degree;
}