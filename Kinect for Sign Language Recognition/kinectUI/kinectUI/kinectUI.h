
// kinectUI.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.



// CkinectUIApp:
// �� Ŭ������ ������ ���ؼ��� kinectUI.cpp�� �����Ͻʽÿ�.
//

class CkinectUIApp : public CWinApp
{
public:
	CkinectUIApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CkinectUIApp theApp;