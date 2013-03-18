#ifndef FCC_H_
#define FCC_H_

// 근사화 임계값 - 이값이 클수록 외곽선이 단순
#define CVCONTOUR_APPROX_LEVEL 2

// 모폴로지 연산 횟수 
#define CVCLOSE_ITR 1

/* params
 IplImage* mask,  : 8bit grayscale image
 int poly1_hull0,  : 다각형 근사화 or 컨벡스 헐 근사화
 float perimScale,  : LEN = image(width + height) / perimScale,
 					LEN 보다 작은 구성 요소는 무시함
 int* num,  :사각형 중심점 최대 개수 
 CvRect* bbs,  : 바운딩 박스 배열 
 CvPoint* centers : 외곽선 중심점 배열  
 
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
