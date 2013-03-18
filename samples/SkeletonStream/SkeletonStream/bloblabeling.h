/// @file BlobLabeling.h
/// @brief Labeling ���� �Լ� ����
/// @author �����(http://martinblog.net) - �λ���б� ��ǻ�Ͱ��а� NR-Lab

#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>

#define HORIZONTAL	0
#define	VERTICAL	1

#define _DEF_MAX_BLOBS	10000   ///<    �ִ� BLOB ����

#define max(a, b)	(a > b ? a : b)
#define min(a, b)	(a < b ? a : b)

/// @struct Visited
/// @brief  Labeling �� �湮���θ� �����ϴ� ����ü
typedef struct
{
	bool	bVisitedFlag;
	CvPoint ptReturnPoint;
} Visited;

/// @class CBlobLabeling
/// @brief Labeling ���� �� �� Blob�� ���� �̿��� ���� Class
class  CBlobLabeling
{
public:
	CBlobLabeling(void);
	~CBlobLabeling(void);

public:
	IplImage*	m_Image;        ///< ���̺��� ���� �̹���(0 �Ǵ� 255 ���� ������ 1ch �̹���)
	int		m_nThreshold;   ///< ���̺� Threshold(�ȼ��� ��)
	Visited*	m_vPoint;       ///< ���̺��� �湮����
	int		m_nBlobs;	///< ���̺��� ����

	CvRect*		m_recBlobs;	///< �� ���̺� ��������
	int*		m_intBlobs;	///< �� ���̺� �ε���

public:
	// ���̺� �̹��� ����
	void	SetParam(IplImage* image, int nThreshold);

	// ���̺�(����)
	void	DoLabeling();

private:
	// ���̺�(����)
	int     Labeling(IplImage* image, int nThreshold);
	void    DetectLabelingRegion(int nLabelNumber, unsigned char *DataBuf, int nWidth, int nHeight);

	// ����Ʈ �ʱ�ȭ
	void	InitvPoint(int nWidth, int nHeight);
	void	DeletevPoint();

	// ���̺�(���� �˰���)
	int     _Labeling(unsigned char *DataBuf, int nWidth, int nHeight, int nThreshold);

	// _Labling ���� ��� �Լ�
	int     __NRFIndNeighbor(unsigned char *DataBuf, int nWidth, int nHeight, int nPosX, int nPosY, int *StartX, int *StartY, int *EndX, int *EndY );
	int     __Area(unsigned char *DataBuf, int StartX, int StartY, int EndX, int EndY, int nWidth, int nLevel);


	// ����-���κ� �񱳰� ���� ���� ���̺� ����
public:
	void	BlobWidthHeightSmallRatioConstraint(float fRatio);
private:
	int	_BlobWidthHeightSmallRatioConstraint(float fRatio, int nRecNumber, CvRect* rect, int* label);

	// ����, ���� ũ�Ⱑ �񱳰� ���� ���� ���̺� ����
public:
	void	BlobSmallSizeConstraint(int nWidth, int nHeight);
private:
	int	_BlobSmallSizeConstraint(int nWidth, int nHeight, int nRecNumber, CvRect* rect, int* label);

	// ����, ���� ũ�Ⱑ �񱳰� ���� ū ���̺� ����
public:
	void	BlobBigSizeConstraint(int nWidth, int nHeight);
private:
	int	_BlobBigSizeConstraint(int nWidth, int nHeight, int nRecNumber, CvRect* rect, int* label);

	// �߽����� �̹����� �𼭸��� �����ϴ� ���̺� ����
public:
	void    BlobEdgeConstraint(int marginX, int marginY);
private:
	int    _BlobEdgeConstraint(int marginX, int marginY, int nRecNumber, CvRect* rect, int* label);

	// ��ü�� ȭ�� ���� ũ�� ������ �񱳰� ���� ���� ���̺� ����
public:
	void    BlobIncludeRatioConstraint(float ratio);
private:
	int    _BlobIncludeRatioConstraint(float ratio, int nRecNumber, CvRect* rect, int* label);


	// ���ϴ� ����� �̹��� ������ �����´�.
public:
	void	GetBlobImage(IplImage* dest, int nLabel, int nX = 0, int nY = 0);
};
