#pragma once

#include "kinectStream.h"
#include "CvvImage.h"
#include "afxwin.h"
#include "resource.h"
#include <vector>
#include <fstream>

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

	// LEARN MODE
	bool isLearnMode;

	ofstream fout;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_sColor;
	CStatic m_sDepth;
	CEdit m_eSLName;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonRun();
	afx_msg void OnBnClickedButtonLearn();
};