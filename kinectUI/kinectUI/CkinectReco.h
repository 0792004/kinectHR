#pragma once

#include "kinectStream.h"
#include "CvvImage.h"
#include "afxwin.h"
#include "resource.h"

// CkinectReco 대화 상자입니다.

class CkinectReco : public CDialogEx
{
	DECLARE_DYNAMIC(CkinectReco)

public:
	CkinectReco(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CkinectReco();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_KINECT_RECO };

	// CKinectStream 변수
	CKinectStream kinectStream;

	// CvvImage
	CvvImage cvvImage;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_sColor;
	CStatic m_sDepth;
	afx_msg void OnBnClickedButtonRun();
};
