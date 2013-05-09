#pragma once

#include "kinectStream.h"
#include "CvvImage.h"
#include "afxwin.h"
#include "resource.h"
#include <vector>
#include <fstream>

#define PI		3.1415926f

// CkinectLearn ��ȭ �����Դϴ�.
using namespace std;

class CkinectLearn : public CDialogEx
{
	DECLARE_DYNAMIC(CkinectLearn)

public:
	CkinectLearn(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CkinectLearn();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_KINECT_LEARN };

	// CKinectStream ����
	CKinectStream kinectStream;

	// CvvImage
	CvvImage cvvImage;

	// rawData Vector
	vector<Point2d> rawData[NUI_SKELETON_POSITION_COUNT];

	// LEARN MODE
	bool isLearnMode;

	// GetDegree
	double GetDegree(double x1, double x2, double y1, double y2);

	// SaveRawData
	void SaveRawData(CString name, CString &path);

	// DisplayList
	void DisplayList(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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