#pragma once

#include "kinectStream.h"
#include "CvvImage.h"
#include "afxwin.h"
#include "resource.h"
#include <vector>
#include <fstream>

// CkinectLearn 대화 상자입니다.
using namespace std;

class CkinectLearn : public CDialogEx
{
	DECLARE_DYNAMIC(CkinectLearn)

public:
	CkinectLearn(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CkinectLearn();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_KINECT_LEARN };

	// CKinectStream 변수
	CKinectStream kinectStream;

	// CvvImage
	CvvImage cvvImage;

	// LEARN MODE
	bool isLearnMode;

	ofstream fout;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_sColor;
	CStatic m_sDepth;
	CEdit m_eSLName;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonRun();
	afx_msg void OnBnClickedButtonLearn();
};