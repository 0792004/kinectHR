
// kinectdialog.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CkinectdialogApp:
// �� Ŭ������ ������ ���ؼ��� kinectdialog.cpp�� �����Ͻʽÿ�.
//

class CkinectdialogApp : public CWinApp
{
public:
	CkinectdialogApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CkinectdialogApp theApp;