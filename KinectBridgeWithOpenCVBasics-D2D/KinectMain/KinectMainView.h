
// KinectMainView.h : CKinectMainView Ŭ������ �������̽�
//

#pragma once


class CKinectMainView : public CView
{
protected: // serialization������ ��������ϴ�.
	CKinectMainView();
	DECLARE_DYNCREATE(CKinectMainView)

// Ư���Դϴ�.
public:
	CKinectMainDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CKinectMainView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // KinectMainView.cpp�� ����� ����
inline CKinectMainDoc* CKinectMainView::GetDocument() const
   { return reinterpret_cast<CKinectMainDoc*>(m_pDocument); }
#endif

