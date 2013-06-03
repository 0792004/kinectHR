#pragma once

#include "afxwin.h"
#include "resource.h"
#include "kinectStream.h"
#include "CvvImage.h"
#include "common.h"
#include <vector>
#include <fstream>

using namespace std;

class CkinectLearn : public CDialogEx
{
	DECLARE_DYNAMIC(CkinectLearn)

public:
	CkinectLearn(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CkinectLearn();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_KINECT_LEARN };

	// Settings 변수
	Settings settings;

	// CKinectStream 변수
	CKinectStream kinectStream;

	// CvvImage
	CvvImage cvvImage;

	// rawData Vector
	vector<Point2d> rawData[NUI_SKELETON_POSITION_COUNT];

	// FONT
	CFont m_fontInput;
	CFont m_fontList;

	// LEARN MODE
	bool isLearnMode;

	void InitFont();
	void SaveRawData();
	void DisplayList();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_sColor;
	CStatic m_sDepth;
	CString m_strSLName;
	CListBox m_lSLName;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonRun();
	afx_msg void OnBnClickedButtonLearn();
	CButton m_btnRun;
	CButton m_btnLearn;
};