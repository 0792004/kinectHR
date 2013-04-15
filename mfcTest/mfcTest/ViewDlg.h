#pragma once
#include "afxwin.h"
#include "opencv.hpp"
#include "CvvImage.h"
// CViewDlg ��ȭ �����Դϴ�.

class CViewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CViewDlg)

public:
	CViewDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CViewDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_KINECT_VIEW };

	// �߰�
	//CvvImage m_CvvImage;
	IplImage *color;
	bool m_viewState;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	// �߰�
protected:
	HICON m_hIcon;

public:
	CStatic m_sView;
	CvvImage  cvvImage;
	IplImage* image;
	CvCapture* capture;

	CString time_string; 
	CTime t;
	IplImage *img;	
	CvVideoWriter *writer;	
	bool start;

	CButton m_run;
	CButton m_stop;
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedRun();
	afx_msg void OnBnClickedStop();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
