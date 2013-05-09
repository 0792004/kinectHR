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
	, m_strSLName(_T(""))
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
	DDX_Control(pDX, IDC_LIST_SLNAME, m_lSLName);
	DDX_Text(pDX, IDC_EDIT_SLNAME, m_strSLName);
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

	if (isLearnMode)
	{
		for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
		{
			if ((kinectStream.GetSkeletonPositon(i).x != 0.0f)
				&& (kinectStream.GetSkeletonPositon(i).y != 0.0f))
				rawData[i].push_back(kinectStream.GetSkeletonPositon(i));
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CkinectLearn::OnBnClickedButtonRun()
{
	kinectStream.InitializeKinect();
	kinectStream.OpenColorStream();
	kinectStream.OpenDepthStream();
	DisplayList();

	SetTimer(1, 1000 / FPS, NULL);
}


void CkinectLearn::OnBnClickedButtonLearn()
{
	if (!isLearnMode)
	{
		isLearnMode = true;
		UpdateData();
	}
	else
	{
		isLearnMode = false;
		CString path;
		path += "C:\\Users\\kwon\\Documents\\";
		path += m_strSLName;
		path += ".txt";

		SaveRawData(m_strSLName, path);
		//UpdateData();
		//m_lSLName.AddString(m_strSLName);
		DisplayList();
	}
}

// p1(x1, y1), p2(x2, y2)
// f(x) = y1인 수평선을 기준으로하여 p1과 p2사이의 각도를 구한다.
double CkinectLearn::GetDegree(double x1, double x2, double y1, double y2)
{
	double dx = x2 - x1;
	double dy = y2 - y1;

	double rad = atan2(dx, dy);
	double degree = (rad * 180) / PI ;

	return degree;
}

void CkinectLearn::SaveRawData(CString name, CString &path)
{
	ofstream fout;
	fout.open("C:\\Users\\kwon\\Documents\\sl_list.txt", ios::app);
	if (fout.is_open())
	{
		//fout << name.GetString() << endl;
		for (int i = 0; i < name.GetLength(); i++)
			fout.put(name.GetAt(i));
		fout.put('\n');
	}
	fout.close();
	
	fout.open(path);

	if (fout.is_open())
	{
		// SPINE rawData의 iterator
		vector<Point2d>::const_iterator posSP;
		// SPINE의 rawDATA
		vector<Point2d> &rawSP = rawData[NUI_SKELETON_POSITION_SPINE];
		// fStep프레임 당 한번 데이터 저장
		const int fStep = 1;

		for (int i = NUI_SKELETON_POSITION_SPINE + 1; i < NUI_SKELETON_POSITION_COUNT; i++)
		{
			// SPINE 외 다른 rawData의 iterator
			vector<Point2d>::const_iterator pos;
			// i번째 rawDATA
			vector<Point2d> &raw = rawData[i];
			
			if (raw.size() > 0)
			{
				int cnt = 0;
				for (posSP = rawSP.begin(), pos = raw.begin(); posSP != rawSP.end(); posSP++, pos++)
				{
					cnt++;
					if (cnt == fStep)
					{
						cnt = 0;
						//fout << (int)(GetDegree(posSP->x, pos->x, posSP->y, pos->y)) << ", " << endl;
						fout << (int)(GetDegree(posSP->x, pos->x, posSP->y, pos->y)) << endl;
					}
				}
				fout << endl;
			}
		}
	}
	fout.close();

	for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
		rawData[i].clear();
}

void CkinectLearn::DisplayList(void)
{
	m_lSLName.ResetContent();

	ifstream fin;
	CString path("C:\\Users\\kwon\\Documents\\sl_list.txt");
	fin.open(path, ios::in);
	
	if (fin.is_open())
	{
		char slName[1024];
		int idx = 0;
		while (fin.getline(slName, 1024))
		{
			CString str(slName);
			m_lSLName.InsertString(idx, str);
			idx++;
		}
	}

	fin.close();
}