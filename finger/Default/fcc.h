#ifndef FCC_H_
#define FCC_H_

// �ٻ�ȭ �Ӱ谪 - �̰��� Ŭ���� �ܰ����� �ܼ�
#define CVCONTOUR_APPROX_LEVEL 2

// �������� ���� Ƚ�� 
#define CVCLOSE_ITR 1

/* params
 IplImage* mask,  : 8bit grayscale image
 int poly1_hull0,  : �ٰ��� �ٻ�ȭ or ������ �� �ٻ�ȭ
 float perimScale,  : LEN = image(width + height) / perimScale,
 					LEN ���� ���� ���� ��Ҵ� ������
 int* num,  :�簢�� �߽��� �ִ� ���� 
 CvRect* bbs,  : �ٿ�� �ڽ� �迭 
 CvPoint* centers : �ܰ��� �߽��� �迭  
 
 */
 
#include "cv.h"
#include "cxcore.h"
 
void find_connected_components(
 		IplImage* mask,
 		int poly1_hull0, // 1
 		float perimScale, // 4 
 		int* num, // NULL
 		CvRect* bbs, // NULL
 		CvPoint* centers // NULL
 		);
 
void _find_connected_components(IplImage* mask);
 
#endif /* end of include guard: FCC_H_ */
