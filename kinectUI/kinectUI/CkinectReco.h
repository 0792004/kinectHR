#pragma once

#include "kinectStream.h"
#include "CvvImage.h"
#include "afxwin.h"
#include "resource.h"

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

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_sColor;
	CStatic m_sDepth;
	afx_msg void OnBnClickedButtonRun();
};
