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
	

	// (1) �Է� ����ũ ������ �����ϰ� �����Ѵ�.
	//
	cvMorphologyEx(mask, mask, 0, 0, CV_MOP_OPEN, CVCLOSE_ITR);
	cvMorphologyEx(mask, mask, 0, 0, CV_MOP_CLOSE , CVCLOSE_ITR);
	cvErode(mask, mask, NULL , 1);
	//cvMorphologyEx(Depth, Depth, 0, 0, CV_MOP_GRADIENT , CVCLOSE_ITR);//�ܰ����� ����

	// (2) ū ������ �ѷ��� �ܰ����� ã�´�.
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

		// �ܰ��� ���� len �� �ּ� �Ӱ谪 ���
		//
		double q = (mask->height + mask->width)/perimScale;

		// �ܰ��� ���̰� �ʹ� ���� ������ �����Ѵ�.
		//
		if (len < q) {
			cvSubstituteContour( scanner, NULL);
		}else{
			// ������ ���̰� ����� ū ��� ������ �������Ѵ�.
			//
			CvSeq* c_new;

			if( poly1_hull0){
				//�ٰ��� �ٻ�ȭ 
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

				// ������ �� �ٻ�ȭ 
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
	
	
	// (3) �ٿ�� �ڽ� �� ���� 

	cvSetZero( mask );

	// ������ �߽ɰ� �ٿ�� �ڽ��� ���Ѵ�.
	//
	if ( num != NULL){

		// ����ڰ� ����� ��ġ�� �����ϱ⸦ ���Ѵ�.
		//
		int N = *num, numFilled=0, i=0;
		CvMoments moments;
		double M00, M01, M10;
		maskTemp = cvCloneImage(mask);

		for (i=0, c=contours; c != NULL; c=c->h_next, i++){
			if( i < N)
			{
				// *num ������ �۾� ����
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

				// �� �ܰ����� �߽��� ã�´�.
				//
				if(centers != NULL){
					cvMoments(maskTemp, &moments, 1);
					M00 = cvGetSpatialMoment(&moments, 0, 0);
					M01 = cvGetSpatialMoment(&moments, 0, 1);
					M10 = cvGetSpatialMoment(&moments, 1, 0);
					centers[i].x = (int)(M10/M00);
					centers[i].y = (int)(M01/M00);
					}
				// ������ ���δ� �ٿ�� �ڽ��� ���Ѵ�.
				//
				if(bbs != NULL){

					bbs[i] = cvBoundingRect(c,0);
				}
				cvSetZero(maskTemp);
				numFilled++;
			}
			//ä���� �ܰ����� mask ���� �׸���.
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
		
	}else{ // �ƴϸ� �ܼ��� ����ũ�� �׸��⸸ �Ѵ�.

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

void _find_connected_components(IplImage* mask){//������ ���� ����

	find_connected_components(mask, 1, 100,NULL,NULL,NULL);

}
