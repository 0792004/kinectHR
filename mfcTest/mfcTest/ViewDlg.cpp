// ViewDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "mfcTest.h"
#include "ViewDlg.h"
#include "afxdialogex.h"


// CViewDlg 대화 상자입니다.

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


// CViewDlg 메시지 처리기입니다.


BOOL CViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.
	/*
	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
	*/
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CViewDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

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
		//MessageBoxA("해당하는 카메라 장치가 없습니다.","cvCreateFileCapture Error",MB_ICONERROR); 
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
