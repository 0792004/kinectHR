#pragma once

#include "afxwin.h"
#include "resource.h"
#include "kinectStream.h"
#include "CvvImage.h"
#include "simpleDTW.h"

#define PATH				"C:\\Users\\kwon\\Documents\\"
#define LIST_TXT			"sl_list.txt"

#define STRING_SIZE				1024
#define PI						3.1415926f
#define VIEW_RATE				70
#define FPS						30

#define ANGLE_DIFFERENCE		3
#define MIN_STOPPED_FRAME		3
#define RECOG_FRAME_ARRANGE		0.50f
#define RECOG_DTW_RATE			0.88f
#define MAX_RAW_DATA_FRAME		1000

// CkinectReco ��ȭ �����Դϴ�.

class CkinectReco : public CDialogEx
{
	DECLARE_DYNAMIC(CkinectReco)

public:
	CkinectReco(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CkinectReco();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_KINECT_RECO };

	// CKinectStream ����
	CKinectStream kinectStream;

	// CvvImage
	CvvImage cvvImage;

	// FONT
	CFont m_fontRecoMode;
	CFont m_fontResult;

	// ANGLE_DATA_INDEX
	enum _ANGLE_DATA_INDEX {
		LE_TO_SP	=	0,
		LH_TO_SP	=	(LE_TO_SP + 1),
		RE_TO_SP	=	(LH_TO_SP + 1),
		RH_TO_SP	=	(RE_TO_SP + 1),
		LE			=	(RH_TO_SP + 1),
		RE			=	(LE + 1),
		ANGLE_DATA_COUNT	=	(RE + 1)
	}ANGLE_DATA_INDEX;

	// RECOG MODE
	enum _RECOG_MODE { FIRST = 0, STOP, SIGN, RECOG };
	enum _RECOG_MODE RECOG_MODE;

	// rawData
	vector<int> rawAngleData[ANGLE_DATA_COUNT];

	// firstData
	int firstAngleData[ANGLE_DATA_COUNT];

	// preJointData
	Point2d preJointData[NUI_SKELETON_POSITION_COUNT];

	typedef struct RecogTable {
		char slName[STRING_SIZE];
		int frameCount;
		bool isInArrange;
		float silmilarity;
	}RecogTable;

	vector<RecogTable> recogTable;

	// SetRecogTable
	void InitRecogTable();

	void InitFont();

	// GetAngle
	double GetAngle(double x1, double x2, double y1, double y2);

	// SetAngle
	void SetAngleData(int *data);

	// 
	void RecogAction();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_sColor;
	CStatic m_sDepth;
	CStatic m_sRecoMode;
	CStatic m_sResult;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonRun();
};
