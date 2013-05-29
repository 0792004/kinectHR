// CkinectReco.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "CkinectReco.h"
#include "afxdialogex.h"

// CkinectReco ��ȭ �����Դϴ�.

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
	DDX_Control(pDX, IDC_STATIC_RECOG_MODE, m_sRecoMode);
	DDX_Control(pDX, IDC_STATIC_RESULT, m_sResult);
}


BEGIN_MESSAGE_MAP(CkinectReco, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CkinectReco::OnBnClickedButtonRun)
END_MESSAGE_MAP()


void CkinectReco::OnTimer(UINT_PTR nIDEvent)
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

			// picture control�� color, depth image�� �׸���
			CDC *pDC;
			pDC = m_sColor.GetDC();

			CRect rect;
			rect.SetRect(0, 0, (int) (KINECT_WIDTH * VIEW_RATE / 100), (int) (KINECT_HEIGHT * VIEW_RATE / 100));

			cvvImage.CopyOf(kinectStream.m_colorIpl, 3);
			cvvImage.DrawToHDC(pDC->m_hDC, rect);

			pDC = m_sDepth.GetDC();
			cvvImage.CopyOf(kinectStream.m_depthIpl, 3);
			cvvImage.DrawToHDC(pDC->m_hDC, rect);

			// ����� �ν��� ���(SPINE���� �ִ� ���) RecogAction�Լ��� ����
			if (kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].x != 0.0f
				&& kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].y != 0.0f)
				RecogAction();

			// kinect�� jointData�� �����Ѵ�
			for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
				preJointData[i] = kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE];
		}
	}


	CDialogEx::OnTimer(nIDEvent);
}


void CkinectReco::OnBnClickedButtonRun()
{
	if (kinectStream.InitializeKinect())
	{
		kinectStream.OpenColorStream();
		kinectStream.OpenDepthStream();
		InitFont();
		InitSignTable();
		RECOG_MODE = FIRST;
		SetTimer(1, 1000 / FPS, NULL);
	}
	else
		MessageBox(_T("Kinect ������ Ȯ���ϰ� ���α׷��� �ٽ� ���� �Ͻʽÿ�."));
}

void CkinectReco::InitSignTable()
{
	CString listPath;
	listPath += PATH;
	listPath += LIST_TXT;

	ifstream listfin;
	listfin.open(listPath);
	if (listfin.is_open())
	{
		char slName[STRING_SIZE];
		char slLen[STRING_SIZE];
		SignTable tSignTable;

		while (listfin.getline(slName, STRING_SIZE))
		{
			listfin.getline(slLen, STRING_SIZE);	
			strcpy(tSignTable.slName, slName);
			tSignTable.frameCount = atoi(slLen);
			tSignTable.isInArrange = false;
			tSignTable.silmilarity = 0.0f;
			signTable.push_back(tSignTable);
		}
	}
	listfin.close();
}

void CkinectReco::InitFont()
{
	//m_font.CreateFontW(20, 12, 0, 0, 1, 0, 0, 0, 0, OUT_DEFAULT_PRECIS, 0, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, (LPCTSTR)"����");
	m_fontRecoMode.CreatePointFont(350, (LPCTSTR)"����");
	m_fontResult.CreatePointFont(250, (LPCTSTR)"����");
	GetDlgItem(IDC_STATIC_RECOG_MODE)->SetFont(&m_fontRecoMode);
	GetDlgItem(IDC_STATIC_RESULT)->SetFont(&m_fontResult);
}

// p1(x1, y1), p2(x2, y2)
// f(x) = y1�� ������ ���������Ͽ� p1�� p2������ ������ ���Ѵ�.
double CkinectReco::GetAngle(double x1, double x2, double y1, double y2)
{
	double dx = x2 - x1;
	double dy = y2 - y1;

	double rad = atan2(dx, dy);
	double degree = (rad * 180) / PI ;

	return degree;
}

void CkinectReco::SetAngleData(int *data)
{
	data[LE_AND_SP] = GetAngle(
		kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].x,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_ELBOW_LEFT].x,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].y,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_ELBOW_LEFT].y);

	data[LH_AND_SP] = GetAngle(
		kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].x,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_HAND_LEFT].x,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].y,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_HAND_LEFT].y);

	data[RE_AND_SP] = GetAngle(
		kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].x,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_ELBOW_RIGHT].x,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].y,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_ELBOW_RIGHT].y);

	data[RH_AND_SP] = GetAngle(
		kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].x,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_HAND_RIGHT].x,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].y,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_HAND_RIGHT].y);

	data[LE] = GetAngle(
		preJointData[NUI_SKELETON_POSITION_HAND_LEFT].x,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_HAND_LEFT].x,
		preJointData[NUI_SKELETON_POSITION_HAND_LEFT].y,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_HAND_LEFT].y);

	data[RE] = GetAngle(
		preJointData[NUI_SKELETON_POSITION_HAND_RIGHT].x,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_HAND_RIGHT].x,
		preJointData[NUI_SKELETON_POSITION_HAND_RIGHT].y,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_HAND_RIGHT].y);
}

// RECOG_MODE�� ���� RecogAction�� ���Ѵ�
void CkinectReco::RecogAction()
{
	switch (RECOG_MODE)
	{
	// ����� AngleData�� �����Ѵ�
	case FIRST:
		{
			CString msg("FIRST");
			SetDlgItemText(IDC_STATIC_RECOG_MODE, msg);
			SetAngleData(firstAngleData);
			RECOG_MODE = STOP;
		}
		break;

	// FIRST���� ������ ���� ������ �����Ͱ� ANGLE_DIFFERENCE �̻� ���� ���ٸ� RECOG_MODE�� SIGN���� �ٲ۴�
	case STOP:
		{
			int tempAngleData[ANGLE_DATA_COUNT];
			SetAngleData(tempAngleData);

			if ((abs(firstAngleData[LH_AND_SP] - tempAngleData[LH_AND_SP]) >= ANGLE_DIFFERENCE)
				|| (abs(firstAngleData[RH_AND_SP] - tempAngleData[RH_AND_SP]) >= ANGLE_DIFFERENCE))
				RECOG_MODE = SIGN;
		}
		break;

	// rawAngleData�� �����Ѵ�
	case SIGN:
		{
			CString msg("SIGN");
			SetDlgItemText(IDC_STATIC_RECOG_MODE, msg);

			int tempAngleData[ANGLE_DATA_COUNT];
			SetAngleData(tempAngleData);

			for (int i = 0; i < ANGLE_DATA_COUNT; i++)
				rawAngleData[i].push_back(tempAngleData[i]);

			if (rawAngleData[0].size() <= MAX_RAW_DATA_FRAME)
			{
				if (rawAngleData[0].size() >= MIN_STOPPED_FRAME)
				{
					for (int i = 0; i < ANGLE_DATA_COUNT; i++)
					{
						int cnt = 0;
						for (int j = rawAngleData[i].size() - 2; cnt < MIN_STOPPED_FRAME - 1; j--)
						{
							if (rawAngleData[i].at(rawAngleData[i].size() - 1) == rawAngleData[i].at(j))
								cnt++;
							else
								break;
						}

						// MIN_STOPPED_FRAME��ŭ ���� ���� �������� �ʴ� �ٸ� RECOG_MODE = SIGN�̴�
						if (cnt < MIN_STOPPED_FRAME - 1)
						{
							RECOG_MODE = SIGN;
							break;
						}
						RECOG_MODE = RECOG;
					}
				}
			}
			else
				RECOG_MODE = RECOG;

		}
		break;

	// signTable�߿��� frame���� +-0.5�� �ȿ� ������ DTW�� �̿��� ���Ѵ�
	case RECOG:
		{
			CString msg("RECOG");
			SetDlgItemText(IDC_STATIC_RECOG_MODE, msg);

			vector<int> fileData[ANGLE_DATA_COUNT];

			// SignTable�� frameCount�� rawAngleData�� RECOG_FRAME_ARRANGE���� �ִ� �͸� ���Ѵ�
			for (int i = 0; i < signTable.size(); i++)
			{
				SignTable &iSignTable = signTable.at(i);

				if (abs(iSignTable.frameCount - (int)(rawAngleData[0].size()))
					<= (int)(rawAngleData[0].size() * RECOG_FRAME_ARRANGE))
				{
					iSignTable.isInArrange = true;

					// RECOG_FRAME_ARRANGE���� �ִٸ� �ش� ������ OPEN
					// fileData�� �����Ѵ�
					CString slPath;
					slPath += PATH;
					slPath += iSignTable.slName;
					slPath += ".txt";

					ifstream slfin;
					slfin.open(slPath);
					if (slfin.is_open())
					{
						int idx_vec = 0;
						char line[STRING_SIZE];
						while(slfin.getline(line, STRING_SIZE))
						{
							if (line[0] != NULL)
								fileData[idx_vec].push_back(atoi(line));
							else
								idx_vec++;
						}
					}
					slfin.close();

					// DTW�� �̿��Ͽ� Similarity�� ���Ѵ�
					float totSimilarity = 0;
					for (int i = 0; i < ANGLE_DATA_COUNT; i++)
					{
						SimpleDTW dtw(fileData[i], rawAngleData[i], fileData[i].size(), rawAngleData[i].size());
						int longSize = (fileData[i].size() > rawAngleData[i].size()) ? fileData[i].size() : rawAngleData[i].size();
						int max = (180 - (-180)) * longSize;
						float similarity = (max - dtw.getBackwardSum()) / (float) max;
						totSimilarity += similarity;
					}

					// AverageSimilarity�� ���Ѵ�
					float avgSimilarity = totSimilarity / ANGLE_DATA_COUNT;
					iSignTable.silmilarity = avgSimilarity;

					// fileData���� clear
					for (int i = 0; i < ANGLE_DATA_COUNT; i++)
						fileData[i].clear();
				}
			}

			// ���� ��ȭ �߿� ���� ���� Similarity�� ���Ѵ�
			float maxSimilarity = 0;
			int maxIdx = - 1;
			for (int i = 0; i < signTable.size(); i++)
			{
				SignTable &iSignTable = signTable.at(i);
				if (iSignTable.isInArrange)
				{
					signTable.at(i).isInArrange = false;
					if (iSignTable.silmilarity > maxSimilarity)
					{
						maxSimilarity =iSignTable.silmilarity;
						maxIdx = i;
					}
				}
			}

			// ���
			if (maxIdx != -1)
			{
				CString strSimilarity;
				CString strResult;
				strSimilarity.Format(_T("%1.2f"), maxSimilarity);
				strResult += strSimilarity;
				strResult += " ";
				strResult += signTable.at(maxIdx).slName;
				SetDlgItemText(IDC_STATIC_RESULT, strResult);
			}

			for (int i = 0; i < ANGLE_DATA_COUNT; i++)
				rawAngleData[i].clear();

			RECOG_MODE = FIRST;
		}
	}
}