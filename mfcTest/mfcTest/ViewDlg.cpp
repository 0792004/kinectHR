// ViewDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "mfcTest.h"
#include "ViewDlg.h"
#include "afxdialogex.h"


// CViewDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CViewDlg, CDialogEx)

CViewDlg::CViewDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CViewDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	image=0;
	capture=0;
	start = false;
}

CViewDlg::~CViewDlg()
{
}

void CViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIEW, m_sView);
	DDX_Control(pDX, IDRUN, m_run);
	DDX_Control(pDX, IDSTOP, m_stop);
}


BEGIN_MESSAGE_MAP(CViewDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDRUN, &CViewDlg::OnBnClickedRun)
	ON_BN_CLICKED(IDSTOP, &CViewDlg::OnBnClickedStop)
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CViewDlg �޽��� ó�����Դϴ�.


BOOL CViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.
	/*
	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDD_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.
	*/
	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CViewDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		//Picture Control size
		CStatic *staticSize = (CStatic *)GetDlgItem(IDC_STATIC_VIEW);
		CRect p_rect;
		staticSize->GetClientRect(p_rect);
		int picWidth=p_rect.Width();
		int picHeight=p_rect.Height();

		if(image){
			CDC *pDC;
			CRect rect;

			pDC = m_sView.GetDC();
			rect.top=0;
			rect.left=0;
			rect.right=picWidth;
			rect.bottom=picHeight;
			cvvImage.CopyOf(image);
			cvvImage.DrawToHDC(pDC->m_hDC,rect);
			ReleaseDC(pDC);
		}

	}
}


void CViewDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CDialogEx::OnSysCommand(nID, lParam);
}


HCURSOR CViewDlg::OnQueryDragIcon()
{
	//return CDialogEx::OnQueryDragIcon();
	return static_cast<HCURSOR>(m_hIcon);
}


void CViewDlg::OnBnClickedRun()
{
	capture=cvCreateCameraCapture(0);
	//if(!capture)
		//MessageBoxA("�ش��ϴ� ī�޶� ��ġ�� �����ϴ�.","cvCreateFileCapture Error",MB_ICONERROR); 
	SetTimer(1,30,NULL);
}


void CViewDlg::OnBnClickedStop()
{
	capture=0;
	if(capture){
		cvReleaseCapture(&capture);
		CViewDlg::OnDestroy();
	}
	start = false;
}


void CViewDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	KillTimer(1);
}


void CViewDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(capture){
		image = cvQueryFrame(capture);
		if(start){
			cvWriteFrame( writer, img );
		}
		Invalidate(FALSE);
	}

	CDialogEx::OnTimer(nIDEvent);
}
