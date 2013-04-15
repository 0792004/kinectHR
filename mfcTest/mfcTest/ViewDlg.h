#pragma once
#include "afxwin.h"
#include "opencv.hpp"
#include "CvvImage.h"
// CViewDlg 대화 상자입니다.

class CViewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CViewDlg)

public:
	CViewDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CViewDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_KINECT_VIEW };

	// 추가
	//CvvImage m_CvvImage;
	IplImage *color;
	bool m_viewState;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	// 추가
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
