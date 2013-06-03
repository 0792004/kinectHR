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
	DDX_Control(pDX, IDC_BUTTON_RUN, m_btnRun);
	DDX_Control(pDX, IDC_BUTTON_LEARN, m_btnLearn);
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
			// 스켈레톤 그리기
			kinectStream.ApplySkeleton(kinectStream.m_depthIpl);
			// 손을 중심으로하는 사각형 그리기
			//DrawHandRect(position,NUI_SKELETON_POSITION_HAND_LEFT, NUI_SKELETON_POSITION_HAND_RIGHT, img);
			// ConvexHull 그리기
			kinectStream.DrawConvexHull(kinectStream.m_depthIpl);

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
	// settins.ini 파일을 읽어들임
	if (!settings.InitSettings())
	{
		MessageBox(_T("settings.ini 파일을 확인하고 프로그램을 다시 시작하십시오."));
		return;
	}

	// kinect 초기화
	if (kinectStream.InitializeKinect())
	{
		kinectStream.OpenColorStream();
		kinectStream.OpenDepthStream();
		InitFont();
		DisplayList();
		SetTimer(1, 1000 / settings.FPS, NULL);
		m_btnRun.EnableWindow(false);
	}
	else
		MessageBox(_T("Kinect 연결을 확인하고 프로그램을 다시 시작 하십시오."));
}


void CkinectLearn::OnBnClickedButtonLearn()
{
	if (!isLearnMode)
	{
		isLearnMode = true;
		UpdateData();
		m_btnLearn.SetWindowTextW(_T("학습 종료"));
	}
	else
	{
		isLearnMode = false;
		CString slPath;
		slPath += settings.PATH;
		slPath += m_strSLName;
		slPath += ".txt";

		if (rawData[NUI_SKELETON_POSITION_SPINE].size() > 0)
		{
			SaveRawData();
			DisplayList();
		}
		m_btnLearn.SetWindowTextW(_T("학습 시작"));
	}
}

void CkinectLearn::InitFont()
{
	m_fontInput.CreatePointFont(350, (LPCTSTR)"굴림");
	GetDlgItem(IDC_EDIT_SLNAME)->SetFont(&m_fontInput);

	m_fontList.CreatePointFont(150, (LPCTSTR)"굴림");
	GetDlgItem(IDC_LIST_SLNAME)->SetFont(&m_fontList);
}

char* ConvertWCtoC(wchar_t* str)
{
    //반환할 char* 변수 선언
    char* pStr ; 

    //입력받은 wchar_t 변수의 길이를 구함
    int strSize = WideCharToMultiByte(CP_ACP, 0,str,-1, NULL, 0,NULL, NULL);
    //char* 메모리 할당
    pStr = new char[strSize];

    //형 변환 
    WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0,0);
    return pStr;
}

void CkinectLearn::SaveRawData()
{
	// sl_list.txt에 수화 이름 저장
	CString listPath;
	listPath += settings.PATH;
	listPath += SL_LIST_TXT;

	ofstream fout;
	fout.open(listPath, ios::app);
	
	if (fout.is_open())
	{
		// 한글을 파일입출력 하기 위하여
		// wchar_t -> char형으로 변환
		wchar_t* wstr = T2W(m_strSLName.GetBuffer(0));
		fout << ConvertWCtoC(wstr) << endl;
		fout << rawData[NUI_SKELETON_POSITION_SPINE].size() << endl;
	}
	fout.close();

	CString slPath;
	slPath += settings.PATH;
	slPath += m_strSLName;
	slPath += ".txt";
	// name(수화명).txt에 각종 데이터 저장
	//ofstream fout;
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
	listPath += settings.PATH;
	listPath += SL_LIST_TXT;

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