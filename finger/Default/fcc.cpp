#include "fcc.h"

 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void find_connected_components(
		IplImage* mask,
		int poly1_hull0, // 1
		float perimScale, // 4 
		int* num, // NULL
		CvRect* bbs, // NULL
		CvPoint* centers // NULL
		){


	//
	//
	//
	
	static CvMemStorage* mem_storage = NULL; // (1) 
	static CvSeq*			contours = NULL; // (1)

	
	CvContourScanner scanner; // (2)
	CvSeq* c; // (2) 
	int numCont=0; // (2)

	
	const CvScalar CVX_WHITE = CV_RGB(0xff, 0xff, 0xff); // (3)
	const CvScalar CVX_BLACK = CV_RGB(0x00, 0x00, 0x00); // (3)

	IplImage* maskTemp; //(3)
	//
	// ----------
	

	// (1) 입력 마스크 영상을 깨끗하게 정리한다.
	//
	cvMorphologyEx(mask, mask, 0, 0, CV_MOP_OPEN, CVCLOSE_ITR);
	cvMorphologyEx(mask, mask, 0, 0, CV_MOP_CLOSE , CVCLOSE_ITR);
	cvErode(mask, mask, NULL , 1);
	//cvMorphologyEx(Depth, Depth, 0, 0, CV_MOP_GRADIENT , CVCLOSE_ITR);//외곽선을 추출

	// (2) 큰 영역을 둘러싼 외곽선만 찾는다.
	// 
	if (mem_storage == NULL){
		mem_storage = cvCreateMemStorage(0);
	}else{
		cvClearMemStorage( mem_storage );
	}

	scanner = cvStartFindContours(
			mask,
			mem_storage,
			sizeof(CvContour),
			CV_RETR_EXTERNAL,
			CV_CHAIN_APPROX_SIMPLE,
			cvPoint(0,0)
			);

	while((c = cvFindNextContour( scanner )) != NULL){

		double len = cvContourPerimeter( c );

		// 외곽선 길이 len 의 최소 임계값 계산
		//
		double q = (mask->height + mask->width)/perimScale;

		// 외곽선 길이가 너무 작은 영역은 제거한다.
		//
		if (len < q) {
			cvSubstituteContour( scanner, NULL);
		}else{
			// 엣지의 길이가 충분히 큰 경우 엣지를 스무딩한다.
			//
			CvSeq* c_new;

			if( poly1_hull0){
				//다각형 근사화 
				//
				c_new = cvApproxPoly(
						c,
						sizeof(CvContour),
						mem_storage,
						CV_POLY_APPROX_DP,
						CVCONTOUR_APPROX_LEVEL,
						0
						);
			}else{

				// 컨벡스 헐 근사화 
				//
				c_new = cvConvexHull2(
						c,
						mem_storage,
						CV_CLOCKWISE,
						1
						);
			}
			cvSubstituteContour(scanner, c_new);
			numCont++;
		}
	}

	contours = cvEndFindContours( &scanner );
	
	
	// (3) 바운딩 박스 및 센터 

	cvSetZero( mask );

	// 영역의 중심과 바운딩 박스를 구한다.
	//
	if ( num != NULL){

		// 사용자가 통계적 수치를 수집하기를 원한다.
		//
		int N = *num, numFilled=0, i=0;
		CvMoments moments;
		double M00, M01, M10;
		maskTemp = cvCloneImage(mask);

		for (i=0, c=contours; c != NULL; c=c->h_next, i++){
			if( i < N)
			{
				// *num 까지만 작업 수행
				//
				cvDrawContours(
						maskTemp,
						c,
						CVX_WHITE,
						CVX_WHITE,
						-1,
						CV_FILLED,
						8,
						cvPoint(0,0)
						);

				// 각 외곽선의 중심을 찾는다.
				//
				if(centers != NULL){
					cvMoments(maskTemp, &moments, 1);
					M00 = cvGetSpatialMoment(&moments, 0, 0);
					M01 = cvGetSpatialMoment(&moments, 0, 1);
					M10 = cvGetSpatialMoment(&moments, 1, 0);
					centers[i].x = (int)(M10/M00);
					centers[i].y = (int)(M01/M00);
					}
				// 영역을 감싸는 바운딩 박스를 구한다.
				//
				if(bbs != NULL){

					bbs[i] = cvBoundingRect(c,0);
				}
				cvSetZero(maskTemp);
				numFilled++;
			}
			//채워진 외곽선을 mask 영상에 그린다.
			//
			cvDrawContours(
					mask,
					c,
					CVX_WHITE,
					CVX_WHITE,
					-1,
					CV_FILLED,
					8,
					cvPoint(0,0)
				);

		}
		*num = numFilled;
		cvReleaseImage(&maskTemp);
		
	}else{ // 아니면 단순히 마스크에 그리기만 한다.

		for ( c = contours; c != NULL; c=c->h_next)
		{
			cvDrawContours(
					mask,
					c,
					CVX_WHITE,
					CVX_WHITE,
					-1,
					CV_FILLED,
					8,
					cvPoint(0,0)
					);
			
		}
	}
	//add codd//add codd//add codd//add codd//add codd//add codd//add codd//add codd//add codd//add codd//add codd
	/*
	CvSeq* ptseq;
	CvPoint pt;
	CvSeq* hull;
	CvPoint end_pt;
	pt.x = 0; pt.y = 0;

	cvSeqPush(ptseq, &pt);

	if(ptseq->total != 0)
	{
		hull = cvConvexHull2(ptseq, 0, CV_COUNTER_CLOCKWISE, 0 );

		for(int x = 0; x < c->total; x++)
		{
			CvPoint hull_pt = **CV_GET_SEQ_ELEM(CvPoint*, hull, x);
	
			if(pt.x == 0 && pt.y ==0)
			{
				pt = hull_pt;
				end_pt = pt;
			}
			cvLine(mask, pt, hull_pt, CV_RGB(255,255,255), 2 ,8);
			pt = hull_pt;
	
			if(x==c->total-1)
				cvLine(mask, hull_pt, end_pt, CV_RGB(255,255,255), 2, 8);
		}
	}
	
	*/
	//add codd//add codd//add codd//add codd//add codd//add codd//add codd//add codd//add codd//add codd
}

void _find_connected_components(IplImage* mask){//생성자 쓰네 ㅎㅎ

	find_connected_components(mask, 1, 100,NULL,NULL,NULL);

}
