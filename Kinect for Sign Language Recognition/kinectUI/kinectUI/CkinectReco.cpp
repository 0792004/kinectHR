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
			// ����� �ν� �Ѵٸ� ���̷��� ������ ����
			kinectStream.ApplySkeleton(kinectStream.m_depthIpl);

			// ���̷��� �׸���
			kinectStream.DrawSkeleton(kinectStream.m_depthIpl);

			// �հ��� �׸���
			kinectStream.CreateCloneImage(kinectStream.m_depthIpl);
			kinectStream.SetHandROI(kinectStream.m_cloneImage);

			// picture control�� color, depth image�� �׸���
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

			// ����� �ν��� ���(SPINE���� �ִ� ���) RecogAction�Լ��� ����
			if (kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].x != 0.0f
				&& kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE].y != 0.0f)
				RecogAction();

			/*if (kinectStream.m_jointData[NUI_SKELETON_POSITION_HAND_RIGHT].x != 0.0f
			&& kinectStream.m_jointData[NUI_SKELETON_POSITION_HAND_RIGHT].y != 0.0f)
			TemplateMatching();*/

			// kinect�� jointData�� �����Ѵ�
			for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
				preJointData[i] = kinectStream.m_jointData[NUI_SKELETON_POSITION_SPINE];
		}
	}


	CDialogEx::OnTimer(nIDEvent);
}


void CkinectReco::OnBnClickedButtonRun()
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
		InitSignTable();
		//InitImgTable();
		RECOG_MODE = FIRST;
		SetTimer(1, 1000 / settings.FPS, NULL);
		m_btnRun.EnableWindow(false);
	}
	else
		MessageBox(_T("Kinect ������ Ȯ���ϰ� ���α׷��� �ٽ� ���� �Ͻʽÿ�."));
}

void CkinectReco::InitFont()
{
	m_fontStatic.CreatePointFont(350, (LPCTSTR)"����");
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

// RECOG_MODE�� ���� RecogAction�� ���Ѵ�
void CkinectReco::RecogAction()
{
	switch (RECOG_MODE)
	{
		// ���� AngleData�� �����Ѵ�
		// RECOG_MODE�� STOP���� SIGN���� �����ϴ� ������ �ȴ�
	case FIRST:
		{
			CString msg("�����");
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
			CString msg("��ȭ��");
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

						// MIN_STOPPED_FRAME��ŭ ���� ���� �������� �ʴ� �ٸ� RECOG_MODE = SIGN�̴�
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

		// signTable�߿��� frame���� +-0.5�� �ȿ� ������ DTW�� �̿��� ���Ѵ�
	case RECOG:
		{
			CString msg("�ν���");
			SetDlgItemText(IDC_STATIC_RECOG_MODE, msg);

			vector<int> fileData[ANGLE_DATA_COUNT];

			// SignTable�� frameCount�� rawAngleData�� RECOG_FRAME_RANGE���� �ִ� �͸� ���Ѵ�
			for (int i = 0; i < signTable.size(); i++)
			{
				SignTable &iSignTable = signTable.at(i);

				if (abs(iSignTable.frameCount - (int)(rawAngleData[0].size()))
					<= (int)(rawAngleData[0].size() * settings.RECOG_FRAME_RANGE))
				{
					iSignTable.isInArrange = true;

					// RECOG_FRAME_ARRANGE���� �ִٸ� �ش� ������ OPEN
					// fileData�� �����Ѵ�
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

			// Dialog�� ���
			CString strResult;
			CString strSimilarity;
			if (maxIdx != -1)
			{
				strResult += signTable.at(maxIdx).slName;
				strSimilarity.Format(_T("%d"), (int)(maxSimilarity * 100));
				strSimilarity += "%";
			}
			else
				strResult += "����";

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
			// MIN_STOPPED_FRAME��ŭ ���� ���� �������� �ʴ� �ٸ� Template Matching���� �ʴ´�
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

		// ���� �̹���
		IplImage *tempImg = cvLoadImage(iImgTable.fileName, -1);
		// ������ �̹���
		IplImage* coeffImg = cvCreateImage(
			cvSize(
			handRightImg->width - tempImg->width + 1,
			handRightImg->height - tempImg->height + 1),
			IPL_DEPTH_32F,
			1
			);

		// �������� �׸���
		cvMatchTemplate(handRightImg, tempImg, coeffImg, CV_TM_CCOEFF_NORMED);
		// �������� �ִ밪�� ���� ��ġ(leftTop) ã�´�
		cvMinMaxLoc(coeffImg, &min, &max, NULL, &leftTop);

		iImgTable.coeffVal = max;
		iImgTable.leftTop = leftTop;

		cvReleaseImage(&handRightImg);
		cvReleaseImage(&tempImg);
		cvReleaseImage(&coeffImg);
	}

	// ���� ��ȭ �߿� ���� ���� ������(CoeffVal)�� ���Ѵ�
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

	// Dialog�� ���
	CString strResult;
	CString strSimilarity;
	if (maxIdx != -1)
	{
		strResult += signTable.at(maxIdx).slName;
		strSimilarity.Format(_T("%d"), (int)(maxCoeffVal * 100));
		strSimilarity += "%";
	}
	else
		strResult += "����";

	SetDlgItemText(IDC_STATIC_SLNAME, strResult);
	SetDlgItemText(IDC_STATIC_RECOG_RATE, strSimilarity);
}
