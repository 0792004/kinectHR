
// kinectUIDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "kinectStream.h"

// CkinectUIDlg ��ȭ ����
class CkinectUIDlg : public CDialogEx
{
public:
	CImage m_bgImage;

// �����Դϴ�.
public:
	CkinectUIDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_KINECTUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonSsettings();
	afx_msg void OnBnClickedMfcbuttonLearn();
	afx_msg void OnBnClickedMfcbuttonRecog();
	afx_msg void OnBnClickedMfcbuttonExit();
};
