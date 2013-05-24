#pragma once

#include "afxwin.h"
#include "resource.h"
#include "kinectStream.h"
#include "CvvImage.h"
#include <vector>
#include <fstream>

#define PATH			"C:\\Users\\kwon\\Documents\\"
#define LIST_TXT		"sl_list.txt"
#define STRING_SIZE		1024
#define PI				3.1415926f
#define VIEW_RATE		70
#define FPS				30

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

	// rawData Vector
	vector<Point2d> rawData[NUI_SKELETON_POSITION_COUNT];

	// FONT
	CFont m_fontInput;
	CFont m_fontList;

	// LEARN MODE
	bool isLearnMode;

	void InitFont();

	// GetAngle
	double GetAngle(double x1, double x2, double y1, double y2);

	// SaveRawData
	void SaveRawData();

	// DisplayList
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
};