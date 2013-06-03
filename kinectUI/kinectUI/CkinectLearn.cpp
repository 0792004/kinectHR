// CkinectLearn.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "CkinectLearn.h"
#include "afxdialogex.h"
#include <vector>

// CkinectLearn ��ȭ �����Դϴ�.

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


// CkinectLearn �޽��� ó�����Դϴ�.


void CkinectLearn::OnTimer(UINT_PTR nIDEvent)
{
	if (!FAILED(kinectStream.CreateRGBImage(kinectStream.m_colorStreamHandle, kinectStream.m_colorIpl)))
	{
		if (!FAILED(kinectStream.CreateDepthImage(kinectStream.m_depthStreamHandle, kinectStream.m_depthIpl)))
		{
			// ���̷��� �׸���
			kinectStream.ApplySkeleton(kinectStream.m_depthIpl);
			// ���� �߽������ϴ� �簢�� �׸���
			//DrawHandRect(position,NUI_SKELETON_POSITION_HAND_LEFT, NUI_SKELETON_POSITION_HAND_RIGHT, img);
			// ConvexHull �׸���
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
	// settins.ini ������ �о����
	if (!settings.InitSettings())
	{
		MessageBox(_T("settings.ini ������ Ȯ���ϰ� ���α׷��� �ٽ� �����Ͻʽÿ�."));
		return;
	}

	// kinect �ʱ�ȭ
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
		MessageBox(_T("Kinect ������ Ȯ���ϰ� ���α׷��� �ٽ� ���� �Ͻʽÿ�."));
}


void CkinectLearn::OnBnClickedButtonLearn()
{
	if (!isLearnMode)
	{
		isLearnMode = true;
		UpdateData();
		m_btnLearn.SetWindowTextW(_T("�н� ����"));
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
		m_btnLearn.SetWindowTextW(_T("�н� ����"));
	}
}

void CkinectLearn::InitFont()
{
	m_fontInput.CreatePointFont(350, (LPCTSTR)"����");
	GetDlgItem(IDC_EDIT_SLNAME)->SetFont(&m_fontInput);

	m_fontList.CreatePointFont(150, (LPCTSTR)"����");
	GetDlgItem(IDC_LIST_SLNAME)->SetFont(&m_fontList);
}

char* ConvertWCtoC(wchar_t* str)
{
    //��ȯ�� char* ���� ����
    char* pStr ; 

    //�Է¹��� wchar_t ������ ���̸� ����
    int strSize = WideCharToMultiByte(CP_ACP, 0,str,-1, NULL, 0,NULL, NULL);
    //char* �޸� �Ҵ�
    pStr = new char[strSize];

    //�� ��ȯ 
    WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0,0);
    return pStr;
}

void CkinectLearn::SaveRawData()
{
	// sl_list.txt�� ��ȭ �̸� ����
	CString listPath;
	listPath += settings.PATH;
	listPath += SL_LIST_TXT;

	ofstream fout;
	fout.open(listPath, ios::app);
	
	if (fout.is_open())
	{
		// �ѱ��� ��������� �ϱ� ���Ͽ�
		// wchar_t -> char������ ��ȯ
		wchar_t* wstr = T2W(m_strSLName.GetBuffer(0));
		fout << ConvertWCtoC(wstr) << endl;
		fout << rawData[NUI_SKELETON_POSITION_SPINE].size() << endl;
	}
	fout.close();

	CString slPath;
	slPath += settings.PATH;
	slPath += m_strSLName;
	slPath += ".txt";
	// name(��ȭ��).txt�� ���� ������ ����
	//ofstream fout;
	fout.open(slPath);
	if (fout.is_open())
	{
		// SPINE rawData�� iterator
		vector<Point2d>::const_iterator posSP;
		// SPINE �� �ٸ� rawData�� iterator
		vector<Point2d>::const_iterator pos;
		// SPINE�� rawDATA
		vector<Point2d> &rawSP = rawData[NUI_SKELETON_POSITION_SPINE];

		// SPINE(����)���� ELBOW_LEFT, HAND_LEFT, ELBOW_RIGHT, HAND_RIGHT���� ������ ���� �����Ѵ�
		for (int i = NUI_SKELETON_POSITION_SPINE + 1; i < NUI_SKELETON_POSITION_COUNT; i++)
		{
			// i��° rawDATA
			vector<Point2d> &raw = rawData[i];
			if (raw.size() > 0)
			{
				for (posSP = rawSP.begin(), pos = raw.begin(); posSP != rawSP.end(); posSP++, pos++)
					fout << (int)(GetAngle(posSP->x, pos->x, posSP->y, pos->y)) << endl;
				fout << endl;
			}
		}

		// �޼�
		// �����Ӱ��� ���� ���� ��ȭ�� ���Ͽ� �����Ѵ�
		// pos, pos+1���� ������ȭ�� ���ϹǷ� for���� ���� ������ end - 1�����̴�
		// ������ ������ ������ ���߱� ���� pos.end���� ������ȭ ���� �ִ´�
		pos = rawData[NUI_SKELETON_POSITION_HAND_LEFT].begin();
		for (posSP = rawSP.begin(); posSP != rawSP.end() - 1; posSP++, pos++)
			fout << (int)(GetAngle(pos->x, (pos + 1)->x, pos->y, (pos + 1)->y)) << endl;
		fout << (int)(GetAngle(pos->x, pos->x, pos->y, pos->y)) << endl;
		fout << endl;

		// ������
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