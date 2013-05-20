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

			if (kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].x != 0.0f
				&& kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].y != 0.0f)
				RecogAction();

			for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
				preJointData[i] = kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE];
		}
	}


	CDialogEx::OnTimer(nIDEvent);
}


void CkinectReco::OnBnClickedButtonRun()
{
	InitFont();
	InitRecogTable();
	kinectStream.InitializeKinect();
	kinectStream.OpenColorStream();
	kinectStream.OpenDepthStream();
	RECOG_MODE = FIRST;
	SetTimer(1, 1000 / FPS, NULL);
}

void CkinectReco::InitRecogTable()
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
		RecogTable tRecogTable;

		while (listfin.getline(slName, STRING_SIZE))
		{
			listfin.getline(slLen, STRING_SIZE);	
			strcpy(tRecogTable.slName, slName);
			tRecogTable.frameCount = atoi(slLen);
			tRecogTable.isInArrange = false;
			tRecogTable.silmilarity = 0.0f;
			recogTable.push_back(tRecogTable);
		}
	}
	listfin.close();
}

void CkinectReco::InitFont()
{
	//m_font.CreateFontW(20, 12, 0, 0, 1, 0, 0, 0, 0, OUT_DEFAULT_PRECIS, 0, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, (LPCTSTR)"굴림");
	m_fontRecoMode.CreatePointFont(350, (LPCTSTR)"굴림");
	m_fontResult.CreatePointFont(250, (LPCTSTR)"굴림");
	GetDlgItem(IDC_STATIC_RECOG_MODE)->SetFont(&m_fontRecoMode);
	GetDlgItem(IDC_STATIC_RESULT)->SetFont(&m_fontResult);
}

// p1(x1, y1), p2(x2, y2)
// f(x) = y1인 수평선을 기준으로하여 p1과 p2사이의 각도를 구한다.
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
	data[LE_TO_SP] = GetAngle(
		kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].x,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_ELBOW_LEFT].x,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].y,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_ELBOW_LEFT].y);

	data[LH_TO_SP] = GetAngle(
		kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].x,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_HAND_LEFT].x,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].y,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_HAND_LEFT].y);

	data[RE_TO_SP] = GetAngle(
		kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].x,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_ELBOW_RIGHT].x,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].y,
		kinectStream.m_jointData[NUI_SKELETON_POSITION_ELBOW_RIGHT].y);

	data[RH_TO_SP] = GetAngle(
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

void CkinectReco::RecogAction()
{
	switch (RECOG_MODE)
	{
	case FIRST:
		{
			CString msg("FIRST");
			SetDlgItemText(IDC_STATIC_RECOG_MODE, msg);
			SetAngleData(firstAngleData);
			RECOG_MODE = STOP;
		}
		break;

	case STOP:
		{
			int tempAngleData[ANGLE_DATA_COUNT];
			SetAngleData(tempAngleData);

			if ((abs(firstAngleData[LH_TO_SP] - tempAngleData[LH_TO_SP]) > ANGLE_DIFFERENCE)
				|| (abs(firstAngleData[RH_TO_SP] - tempAngleData[RH_TO_SP]) > ANGLE_DIFFERENCE))
				RECOG_MODE = SIGN;
		}
		break;

	case SIGN:
		{
			CString msg("SIGN");
			SetDlgItemText(IDC_STATIC_RECOG_MODE, msg);

			int tempAngleData[ANGLE_DATA_COUNT];
			SetAngleData(tempAngleData);

			for (int i = 0; i < ANGLE_DATA_COUNT; i++)
				rawAngleData[i].push_back(tempAngleData[i]);

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

					// MIN_STOPPED_FRAME만큼 같은 값이 연속하지 않는 다면 RECOG_MODE = SIGN이다
					if (cnt < MIN_STOPPED_FRAME - 1)
					{
						RECOG_MODE = SIGN;
						break;
					}
					RECOG_MODE = RECOG;
				}
			}
		}
		break;

	case RECOG:
		{
			CString msg("RECOG");
			SetDlgItemText(IDC_STATIC_RECOG_MODE, msg);

			vector<int> fileData[ANGLE_DATA_COUNT];

			CString path;
			path = "C:\\Users\\kwon\\Documents\\";

			// recogTable의 frameCount가 rawAngleData의 RECOG_FRAME_ARRANGE내에 있는 것만 비교한다
			for (int i = 0; i < recogTable.size(); i++)
			{
				RecogTable &iRecogTable = recogTable.at(i);

				if (abs(iRecogTable.frameCount - (int)(rawAngleData[0].size()))
					<= (rawAngleData[0].size() * RECOG_FRAME_ARRANGE))
				{
					iRecogTable.isInArrange = true;

					// RECOG_FRAME_ARRANGE내에 있다면 해당 파일을 OPEN
					// fileData를 추출한다
					CString slPath = path + iRecogTable.slName + ".txt";
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

					// DTW를 이용하여 Similarity를 구한다
					float totSimilarity = 0;
					for (int i = 0; i < ANGLE_DATA_COUNT; i++)
					{
						SimpleDTW dtw(fileData[i], rawAngleData[i], fileData[i].size(), rawAngleData[i].size());
						int longSize = (fileData[i].size() > rawAngleData[i].size()) ? fileData[i].size() : rawAngleData[i].size();
						int max = (180 - (-180)) * longSize;
						float similarity = (max - dtw.getBackwardSum()) / (float) max;
						totSimilarity += similarity;

						/*int **f = dtw.getFMatrix();
						fstream fout;
						CString outPath = path + "output.txt";
						fout.open(outPath, ios::app);
						if (fout.is_open())
						{
							fout << i << endl;		
							for (int j = 1; j <= fileData[i].size(); j++)
							{
								for (int k = 1; k <= rawAngleData[i].size(); k++)
									fout << f[j][k] << "\t";
								fout << endl;
							}
						}
						fout << "Similarity = " << similarity << endl;
						fout.close();*/
					}

					// AverageSimilarity를 구한다
					float avgSimilarity = totSimilarity / ANGLE_DATA_COUNT;
					iRecogTable.silmilarity = avgSimilarity;

					// fileData벡터 clear
					for (int i = 0; i < ANGLE_DATA_COUNT; i++)
						fileData[i].clear();
				}
			}

			// 비교한 데이터 중에 가장 높은 Similarity를 구한다
			float maxSimilarity = 0;
			int maxIdx = - 1;
			for (int i = 0; i < recogTable.size(); i++)
			{
				if (recogTable.at(i).isInArrange)
				{
					recogTable.at(i).isInArrange = false;
					if (recogTable.at(i).silmilarity > maxSimilarity)
					{
						maxSimilarity = recogTable.at(i).silmilarity;
						maxIdx = i;
					}
				}
			}

			// 출력
			CString strSim;
			CString strResult;
			strSim.Format(_T("%1.2f"), maxSimilarity);
			strResult += strSim;
			strResult += " ";

			if (maxSimilarity >= RECOG_DTW_RATE)
			{	
				strResult += recogTable.at(maxIdx).slName;
				strResult += " OK";
			}

			SetDlgItemText(IDC_STATIC_RESULT, strResult);

			for (int i = 0; i < ANGLE_DATA_COUNT; i++)
				rawAngleData[i].clear();

			RECOG_MODE = FIRST;
		}
	}
}