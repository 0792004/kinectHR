#pragma once

#include "afxwin.h"
#include "resource.h"
#include "kinectStream.h"
#include "CvvImage.h"
#include "simpleDTW.h"
#include "common.h"

// CkinectReco 대화 상자입니다.

class CkinectReco : public CDialogEx
{
	DECLARE_DYNAMIC(CkinectReco)

public:
	CkinectReco(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CkinectReco();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_KINECT_RECO };

	// Settings 변수
	Settings settings;

	// CKinectStream 변수
	CKinectStream kinectStream;

	// CvvImage
	CvvImage cvvImage;

	// FONT
	CFont m_fontStatic;

	// ANGLE_DATA_INDEX
	enum _ANGLE_DATA_INDEX {
		LE_AND_SP	=	0,
		LH_AND_SP	=	(LE_AND_SP + 1),
		RE_AND_SP	=	(LH_AND_SP + 1),
		RH_AND_SP	=	(RE_AND_SP + 1),
		LE			=	(RH_AND_SP + 1),
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

	// SignTable
	typedef struct SignTable {
		char slName[STRING_SIZE];
		int frameCount;
		bool isInArrange;
		float silmilarity;
	}SignTable;

	vector<SignTable> signTable;

	// ImgTable
	typedef struct ImgTable {
		char imgName[STRING_SIZE];
		char fileName[STRING_SIZE];
		double coeffVal;
		CvPoint leftTop;
	}ImgTable;
	
	vector<ImgTable> imgTable;

	void InitFont();
	void InitSignTable();
	void InitImgTable();
	void SetAngleData(int *data);
	void RecogAction();
	void TemplateMatching();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_sColor;
	CStatic m_sDepth;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonRun();
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CButton m_btnRun;
};
