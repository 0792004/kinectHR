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
	DDX_Control(pDX, IDC_BUTTON_RUN, m_btnRun);
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
			// 사람을 인식 한다면 스켈레톤 데이터 생성
			kinectStream.ApplySkeleton(kinectStream.m_depthIpl);

			// 스켈레톤 그리기
			kinectStream.DrawSkeleton(kinectStream.m_depthIpl);

			// 손가락 그리기
			kinectStream.CreateCloneImage(kinectStream.m_depthIpl);
			kinectStream.SetHandROI(kinectStream.m_cloneImage);

			// picture control에 color, depth image를 그린다
			CDC *pDC;
			pDC = m_sColor.GetDC();

			CRect rect;
			rect.SetRect(0, 0, (int) (KINECT_WIDTH * VIEW_RATE / 100), (int) (KINECT_HEIGHT * VIEW_RATE / 100));

			cvvImage.CopyOf(kinectStream.m_colorIpl, 3);
			cvvImage.DrawToHDC(pDC->m_hDC, rect);

			pDC = m_sDepth.GetDC();
			cvvImage.CopyOf(kinectStream.m_depthIpl, 3);
			//cvvImage.CopyOf(kinectStream.m_cloneImage, 1);
			cvvImage.DrawToHDC(pDC->m_hDC, rect);

			// 사람을 인식한 경우(SPINE값이 있는 경우) RecogAction함수를 실행
			if (kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].x != 0.0f
				&& kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].y != 0.0f)
				RecogAction();

			/*if (kinectStream.m_jointData[NUI_SKELETON_POSITION_HAND_RIGHT].x != 0.0f
			&& kinectStream.m_jointData[NUI_SKELETON_POSITION_HAND_RIGHT].y != 0.0f)
			TemplateMatching();*/

			// kinect의 jointData를 저장한다
			for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
				preJointData[i] = kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE];
		}
	}


	CDialogEx::OnTimer(nIDEvent);
}


void CkinectReco::OnBnClickedButtonRun()
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
		InitSignTable();
		//InitImgTable();
		RECOG_MODE = FIRST;
		SetTimer(1, 1000 / settings.FPS, NULL);
		m_btnRun.EnableWindow(false);
	}
	else
		MessageBox(_T("Kinect 연결을 확인하고 프로그램을 다시 시작 하십시오."));
}

void CkinectReco::InitFont()
{
	m_fontStatic.CreatePointFont(350, (LPCTSTR)"굴림");
	GetDlgItem(IDC_STATIC_RECOG_MODE)->SetFont(&m_fontStatic);
	GetDlgItem(IDC_STATIC_SLNAME)->SetFont(&m_fontStatic);
	GetDlgItem(IDC_STATIC_RECOG_RATE)->SetFont(&m_fontStatic);
}

void CkinectReco::InitSignTable()
{
	CString listPath;
	listPath += settings.PATH;
	listPath += SL_LIST_TXT;

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

// RECOG_MODE에 따라 RecogAction을 취한다
void CkinectReco::RecogAction()
{
	switch (RECOG_MODE)
	{
		// 현재 AngleData를 저장한다
		// RECOG_MODE가 STOP인지 SIGN인지 구분하는 기준이 된다
	case FIRST:
		{
			CString msg("대기중");
			SetDlgItemText(IDC_STATIC_RECOG_MODE, msg);
			SetAngleData(firstAngleData);
			RECOG_MODE = STOP;
		}
		break;

		// FIRST에서 저장한 값과 현재의 데이터가 ANGLE_DIFFERENCE 이상 차이 난다면 RECOG_MODE를 SIGN으로 바꾼다
	case STOP:
		{
			int tempAngleData[ANGLE_DATA_COUNT];
			SetAngleData(tempAngleData);

			if ((abs(firstAngleData[LH_AND_SP] - tempAngleData[LH_AND_SP]) >= ANGLE_DIFFERENCE)
				|| (abs(firstAngleData[RH_AND_SP] - tempAngleData[RH_AND_SP]) >= ANGLE_DIFFERENCE))
				RECOG_MODE = SIGN;
		}
		break;

		// rawAngleData를 저장한다
	case SIGN:
		{
			CString msg("수화중");
			SetDlgItemText(IDC_STATIC_RECOG_MODE, msg);

			int tempAngleData[ANGLE_DATA_COUNT];
			SetAngleData(tempAngleData);

			for (int i = 0; i < ANGLE_DATA_COUNT; i++)
				rawAngleData[i].push_back(tempAngleData[i]);

			if (rawAngleData[0].size() <= settings.MAX_RAW_DATA_FRAME)
			{
				if (rawAngleData[0].size() >= settings.MIN_STOPPED_FRAME)
				{
					for (int i = 0; i < ANGLE_DATA_COUNT; i++)
					{
						int cnt = 0;
						for (int j = rawAngleData[i].size() - 2; cnt < settings.MIN_STOPPED_FRAME - 1; j--)
						{
							if (rawAngleData[i].at(rawAngleData[i].size() - 1) == rawAngleData[i].at(j))
								cnt++;
							else
								break;
						}

						// MIN_STOPPED_FRAME만큼 같은 값이 연속하지 않는 다면 RECOG_MODE = SIGN이다
						if (cnt < settings.MIN_STOPPED_FRAME - 1)
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

		// signTable중에서 frame수가 +-0.5배 안에 들어오면 DTW를 이용해 비교한다
	case RECOG:
		{
			CString msg("인식중");
			SetDlgItemText(IDC_STATIC_RECOG_MODE, msg);

			vector<int> fileData[ANGLE_DATA_COUNT];

			// SignTable의 frameCount가 rawAngleData의 RECOG_FRAME_RANGE내에 있는 것만 비교한다
			for (int i = 0; i < signTable.size(); i++)
			{
				SignTable &iSignTable = signTable.at(i);

				if (abs(iSignTable.frameCount - (int)(rawAngleData[0].size()))
					<= (int)(rawAngleData[0].size() * settings.RECOG_FRAME_RANGE))
				{
					iSignTable.isInArrange = true;

					// RECOG_FRAME_ARRANGE내에 있다면 해당 파일을 OPEN
					// fileData를 추출한다
					CString slPath;
					slPath += settings.PATH;
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

					// DTW를 이용하여 Similarity를 구한다
					float totSimilarity = 0;
					for (int i = 0; i < ANGLE_DATA_COUNT; i++)
					{
						SimpleDTW dtw(fileData[i], rawAngleData[i], fileData[i].size(), rawAngleData[i].size());
						int longSize = (fileData[i].size() > rawAngleData[i].size()) ? fileData[i].size() : rawAngleData[i].size();
						int max = (180 - (-180)) * longSize;
						float similarity = (max - dtw.getBackwardSum()) / (float) max;
						totSimilarity += similarity;
					}

					// AverageSimilarity를 구한다
					float avgSimilarity = totSimilarity / ANGLE_DATA_COUNT;
					iSignTable.silmilarity = avgSimilarity;

					// fileData벡터 clear
					for (int i = 0; i < ANGLE_DATA_COUNT; i++)
						fileData[i].clear();
				}
			}

			// 비교한 수화 중에 가장 높은 Similarity를 구한다
			float maxSimilarity = 0.0f;
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

			// Dialog에 출력
			CString strResult;
			CString strSimilarity;
			if (maxIdx != -1)
			{
				strResult += signTable.at(maxIdx).slName;
				strSimilarity.Format(_T("%d"), (int)(maxSimilarity * 100));
				strSimilarity += "%";
			}
			else
				strResult += "없음";

			SetDlgItemText(IDC_STATIC_SLNAME, strResult);
			SetDlgItemText(IDC_STATIC_RECOG_RATE, strSimilarity);

			for (int i = 0; i < ANGLE_DATA_COUNT; i++)
				rawAngleData[i].clear();

			RECOG_MODE = FIRST;
		}
	}
}

void CkinectReco::InitImgTable()
{
	CString listPath;
	listPath += settings.PATH;
	listPath += IMG_LIST_TXT;

	ifstream listfin;
	listfin.open(listPath);
	if (listfin.is_open())
	{
		char buf[STRING_SIZE];

		ImgTable tImgTable;

		while (listfin.getline(buf, STRING_SIZE))
		{
			strcpy(tImgTable.imgName, buf);
			listfin.getline(buf, STRING_SIZE);	
			strcpy(tImgTable.fileName, buf);
			tImgTable.coeffVal = 0.0f;
			tImgTable.leftTop.x = 0;
			tImgTable.leftTop.y = 0;
			imgTable.push_back(tImgTable);
		}
	}
	listfin.close();
}

IplImage* Crop( IplImage* src,  CvRect roi)
{

	// Must have dimensions of output image
	IplImage* cropped = cvCreateImage( cvSize(roi.width,roi.height), src->depth, src->nChannels );

	// Say what the source region is
	cvSetImageROI( src, roi );

	// Do the copy
	cvCopy( src, cropped );
	cvResetImageROI( src );

	return cropped;
}


void CkinectReco::TemplateMatching()
{
	double min, max;
	CvPoint leftTop;

	int tempAngleData[ANGLE_DATA_COUNT];
	SetAngleData(tempAngleData);

	for (int i = 0; i < ANGLE_DATA_COUNT; i++)
		rawAngleData[i].push_back(tempAngleData[i]);

	if (rawAngleData[0].size() > settings.MAX_RAW_DATA_FRAME)
	{
		for (int i = 0; i < ANGLE_DATA_COUNT; i++)
			rawAngleData[i].clear();
		return;
	}

	if (rawAngleData[0].size() >= settings.MIN_STOPPED_FRAME)
	{
		for (int i = 0; i < ANGLE_DATA_COUNT; i++)
		{
			int cnt = 0;
			for (int j = rawAngleData[i].size() - 2; cnt < settings.MIN_STOPPED_FRAME - 1; j--)
			{
				if (rawAngleData[i].at(rawAngleData[i].size() - 1) == rawAngleData[i].at(j))
					cnt++; 
				else
					break;
			}
			// MIN_STOPPED_FRAME만큼 같은 값이 연속하지 않는 다면 Template Matching하지 않는다
			if (cnt < settings.MIN_STOPPED_FRAME - 1)	
				return;
		}
	}
	else
		return;

	for (int i = 0; i < ANGLE_DATA_COUNT; i++)
		rawAngleData[i].clear();


	for (int i = 0; i < imgTable.size(); i++)
	{
		ImgTable &iImgTable = imgTable.at(i);

		int range = 80;
		CvRect handRect = cvRect(
			kinectStream.m_skeletonPoints[NUI_SKELETON_POSITION_HAND_RIGHT].x - range,
			kinectStream.m_skeletonPoints[NUI_SKELETON_POSITION_HAND_RIGHT].y - range,
			range * 2,
			range * 2);

		IplImage *handRightImg = Crop(kinectStream.m_depthIpl, handRect);

		// 비교할 이미지
		IplImage *tempImg = cvLoadImage(iImgTable.fileName, -1);
		// 상관계수 이미지
		IplImage* coeffImg = cvCreateImage(
			cvSize(
			handRightImg->width - tempImg->width + 1,
			handRightImg->height - tempImg->height + 1),
			IPL_DEPTH_32F,
			1
			);

		// 상관계수를 그린다
		cvMatchTemplate(handRightImg, tempImg, coeffImg, CV_TM_CCOEFF_NORMED);
		// 상관계수가 최대값을 값는 위치(leftTop) 찾는다
		cvMinMaxLoc(coeffImg, &min, &max, NULL, &leftTop);

		iImgTable.coeffVal = max;
		iImgTable.leftTop = leftTop;

		cvReleaseImage(&handRightImg);
		cvReleaseImage(&tempImg);
		cvReleaseImage(&coeffImg);
	}

	// 비교한 수화 중에 가장 높은 상관계수(CoeffVal)를 구한다
	int maxIdx = -1;
	double maxCoeffVal = 0.0f;
	for (int i = 0; i < imgTable.size(); i++)
	{
		ImgTable &iImgTable = imgTable.at(i);
		if (iImgTable.coeffVal > maxCoeffVal)
		{
			maxCoeffVal = iImgTable.coeffVal;
			maxIdx = i;
		}
	}

	// Dialog에 출력
	CString strResult;
	CString strSimilarity;
	if (maxIdx != -1)
	{
		strResult += signTable.at(maxIdx).slName;
		strSimilarity.Format(_T("%d"), (int)(maxCoeffVal * 100));
		strSimilarity += "%";
	}
	else
		strResult += "없음";

	SetDlgItemText(IDC_STATIC_SLNAME, strResult);
	SetDlgItemText(IDC_STATIC_RECOG_RATE, strSimilarity);
}
