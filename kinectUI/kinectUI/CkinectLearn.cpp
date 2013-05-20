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
			if ((kinectStream.m_jointData[i].x != 0.0f)
				&& (kinectStream.m_jointData[i].y != 0.0f))
				rawData[i].push_back(kinectStream.m_jointData[i]);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CkinectLearn::OnBnClickedButtonRun()
{
	InitFont();
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
		CString slPath;
		slPath += PATH;
		slPath += m_strSLName;
		slPath += ".txt";

		if (rawData[NUI_SKELETON_POSITION_SPINE].size() > 0)
		{
			SaveRawData();
			DisplayList();
		}
	}
}

void CkinectLearn::InitFont()
{
	//m_font.CreateFontW(20, 12, 0, 0, 1, 0, 0, 0, 0, OUT_DEFAULT_PRECIS, 0, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, (LPCTSTR)"굴림");
	m_font.CreatePointFont(120, (LPCTSTR)"굴림");
	GetDlgItem(IDC_EDIT_SLNAME)->SetFont(&m_font);
	GetDlgItem(IDC_LIST_SLNAME)->SetFont(&m_font);
}

// p1(x1, y1), p2(x2, y2)
// f(x) = y1인 수평선을 기준으로하여 p1과 p2사이의 각도를 구한다.
double CkinectLearn::GetAngle(double x1, double x2, double y1, double y2)
{
	double dx = x2 - x1;
	double dy = y2 - y1;

	double rad = atan2(dx, dy);
	double degree = (rad * 180) / PI ;

	return degree;
}

void CkinectLearn::SaveRawData()
{
	// sl_list.txt에 수화 이름 저장
	CString listPath;
	listPath += PATH;
	listPath += "sl_list.txt";

	ofstream fout;
	fout.open(listPath, ios::app);
	if (fout.is_open())
	{
		/*fout << name.GetString() << endl;
		fout << rawData[NUI_SKELETON_POSITION_SPINE].size() << endl;*/

		for (int i = 0; i < m_strSLName.GetLength(); i++)
			fout.put(m_strSLName.GetAt(i));
		fout.put('\n');
		fout << rawData[NUI_SKELETON_POSITION_SPINE].size() << endl;
	}
	fout.close();

	CString slPath;
	slPath += PATH;
	slPath += m_strSLName;
	slPath += ".txt";
	// name(수화명).txt에 각종 데이터 저장
	fout.open(slPath);
	if (fout.is_open())
	{
		// SPINE rawData의 iterator
		vector<Point2d>::const_iterator posSP;
		// SPINE 외 다른 rawData의 iterator
		vector<Point2d>::const_iterator pos;
		// SPINE의 rawDATA
		vector<Point2d> &rawSP = rawData[NUI_SKELETON_POSITION_SPINE];

		// SPINE(가슴)에서 ELBOW_LEFT, HAND_LEFT, ELBOW_RIGHT, HAND_RIGHT간의 각도를 구해 저장한다
		for (int i = NUI_SKELETON_POSITION_SPINE + 1; i < NUI_SKELETON_POSITION_COUNT; i++)
		{
			// i번째 rawDATA
			vector<Point2d> &raw = rawData[i];
			if (raw.size() > 0)
			{
				for (posSP = rawSP.begin(), pos = raw.begin(); posSP != rawSP.end(); posSP++, pos++)
					fout << (int)(GetAngle(posSP->x, pos->x, posSP->y, pos->y)) << endl;
				fout << endl;
			}
		}

		// 왼손
		// 프레임간의 손의 각도 변화를 구하여 저장한다
		// pos, pos+1간의 각도변화를 구하므로 for문의 종료 조건은 end - 1까지이다
		// 하지만 데이터 개수를 맞추기 위해 pos.end간의 각도변화 값을 넣는다
		pos = rawData[NUI_SKELETON_POSITION_HAND_LEFT].begin();
		for (posSP = rawSP.begin(); posSP != rawSP.end() - 1; posSP++, pos++)
			fout << (int)(GetAngle(pos->x, (pos + 1)->x, pos->y, (pos + 1)->y)) << endl;
		fout << (int)(GetAngle(pos->x, pos->x, pos->y, pos->y)) << endl;
		fout << endl;

		// 오른손
		pos = rawData[NUI_SKELETON_POSITION_HAND_RIGHT].begin();
		for (posSP = rawSP.begin(); posSP != rawSP.end() - 1; posSP++, pos++)
			fout << (int)(GetAngle(pos->x, (pos + 1)->x, pos->y, (pos + 1)->y)) << endl;
		fout << (int)(GetAngle(pos->x, pos->x, pos->y, pos->y)) << endl;
	}
	fout.close();

	for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
		rawData[i].clear();
}

void CkinectLearn::DisplayList()
{
	m_lSLName.ResetContent();

	CString listPath;
	listPath += PATH;
	listPath += LIST_TXT;

	ifstream fin;
	fin.open(listPath, ios::in);

	if (fin.is_open())
	{
		char slName[STRING_SIZE];
		int line = 0;
		int idx = 0;
		while (fin.getline(slName, STRING_SIZE))
		{
			line++;
			if (line % 2 == 1)
			{
				CString str(slName);
				m_lSLName.InsertString(idx, str);
				idx++;
			}
		}
	}

	fin.close();
}