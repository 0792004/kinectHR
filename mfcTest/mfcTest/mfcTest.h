
// mfcTest.h : mfcTest ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CmfcTestApp:
// �� Ŭ������ ������ ���ؼ��� mfcTest.cpp�� �����Ͻʽÿ�.
//

class CmfcTestApp : public CWinApp
{
public:
	CmfcTestApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CmfcTestApp theApp;
