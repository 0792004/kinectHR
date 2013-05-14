
// KinectMainView.cpp : CKinectMainView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "KinectMain.h"
#endif

#include "KinectMainDoc.h"
#include "KinectMainView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKinectMainView

IMPLEMENT_DYNCREATE(CKinectMainView, CView)

BEGIN_MESSAGE_MAP(CKinectMainView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CKinectMainView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CKinectMainView ����/�Ҹ�

CKinectMainView::CKinectMainView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CKinectMainView::~CKinectMainView()
{
}

BOOL CKinectMainView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CKinectMainView �׸���

void CKinectMainView::OnDraw(CDC* /*pDC*/)
{
	CKinectMainDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CKinectMainView �μ�


void CKinectMainView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CKinectMainView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CKinectMainView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CKinectMainView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}

void CKinectMainView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CKinectMainView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CKinectMainView ����

#ifdef _DEBUG
void CKinectMainView::AssertValid() const
{
	CView::AssertValid();
}

void CKinectMainView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKinectMainDoc* CKinectMainView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKinectMainDoc)));
	return (CKinectMainDoc*)m_pDocument;
}
#endif //_DEBUG


// CKinectMainView �޽��� ó����
