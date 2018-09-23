#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "opencv\cv.h"
#include "opencv\cxcore.h"
#include "opencv\highgui.h"
#include "Poisson.h"

using namespace std;
using namespace cv;

IplImage *img0, *img1, *img2, *subimg, *subimg2, *result, *result2;
CvPoint point;
int drag = 0;
int destx, desty;

void drawImage(IplImage* target, IplImage* source, int x, int y)
{
	for (int ix = 0; ix<source->width; ix++)
	{
		for (int iy = 0; iy<source->height; iy++)
		{
			int r = cvGet2D(source, iy, ix).val[2];
			int g = cvGet2D(source, iy, ix).val[1];
			int b = cvGet2D(source, iy, ix).val[0];
			CvScalar bgr = cvScalar(b, g, r);
			cvSet2D(target, iy + y, ix + x, bgr);
		}
	}

}

void mouseHandler(int event, int x, int y, int flags, void* param)
{


	if (event == CV_EVENT_LBUTTONDOWN && !drag)
	{
		point = cvPoint(x, y);
		drag = 1;
	}

	if (event == CV_EVENT_MOUSEMOVE && drag)
	{
		img1 = cvCloneImage(img0);

		cvRectangle(img1, point, cvPoint(x, y), CV_RGB(255, 0, 0), 1, 8, 0);

		cvShowImage("Source", img1);
	}

	if (event == CV_EVENT_LBUTTONUP && drag)
	{
		img1 = cvCloneImage(img0);

		cvSetImageROI(img1, cvRect(point.x, point.y, x - point.x, y - point.y));

		subimg = cvCreateImage(cvGetSize(img1), img1->depth, img1->nChannels);

		cvCopy(img1, subimg, NULL);

		cvNamedWindow("ROI", 1);
		cvShowImage("ROI", subimg);
		cvSaveImage("MyROI.jpg", subimg);
		cvWaitKey(0);
		cvDestroyWindow("ROI");
		cvResetImageROI(img1);
		cvShowImage("Source", img1);
		drag = 0;
	}

	if (event == CV_EVENT_RBUTTONUP)
	{
		cvShowImage("Source", img0);
		drag = 0;
	}
}


void mouseHandler1(int event, int x, int y, int flags, void* param)
{
	IplImage *im, *im1;

	im1 = cvCloneImage(img2);

	if (event == CV_EVENT_LBUTTONDOWN)
	{
		point = cvPoint(x, y);

		cvRectangle(im1, cvPoint(x, y), cvPoint(x + subimg->width, y + subimg->height), CV_RGB(255, 0, 0), 1, 8, 0);

		destx = x;
		desty = y;

		cvShowImage("Destination", im1);
	}
	if (event == CV_EVENT_RBUTTONUP)
	{
		if (destx + subimg->width > img2->width || desty + subimg->height > img2->height)
		{
			//cout << "Index out of range" << endl;
			exit(0);
		}

		drawImage(im1, subimg, destx, desty);
		result = poisson_blend(img2, subimg, desty, destx);
		//result = poisson_blend(img2, subimg, 80, 80);
		//cout << " dest are "<< destx<<" and " << desty;
		////////// save blended result ////////////////////

		
		cvSaveImage("Output.jpg", result);
		cvSaveImage("cutpaste.jpg", im1);

		cvNamedWindow("Image cloned", 1);
		cvShowImage("Image cloned", result);
		//cvShowImage("Image cloned", subimg);
		cvWaitKey(0);
		cvDestroyWindow("Image cloned");
	}
}

int main(int argc, char** argv)
{
	if (argc == 8){
		//img0 = cvLoadImage(argv[0]);
		subimg = cvLoadImage(argv[1]);
		subimg2 = cvLoadImage(argv[2]);

		img2 = cvLoadImage(argv[3]);

		//int disX = atoi(argv[3]);
		//int disY = atoi(argv[4]);

		int disX = strtol(argv[4], NULL, 10);
		int disY = strtol(argv[5], NULL, 10);

		int disX2 = strtol(argv[6], NULL, 10);
		int disY2 = strtol(argv[7], NULL, 10);

		//cout << "my x " << disX << " my y " << disY<<endl;
		//img0 = cvLoadImage("bale.jpg");
		//img2 = cvLoadImage("bale.jpg");

		result = poisson_blend(img2, subimg, disX, disY);

		result2 = poisson_blend(result, subimg2, disX2, disY2);

		//cvNamedWindow("Image cloned", 1);
		//cvShowImage("Image cloned", result2);
		//cvWaitKey(0);
		//cvDestroyWindow("Image cloned");
		cvSaveImage("swapped.jpg", result2);
	}
	if (argc == 5){
		subimg = cvLoadImage(argv[1]);
		
		img2 = cvLoadImage(argv[2]);

		int disX = strtol(argv[3], NULL, 10);
		int disY = strtol(argv[4], NULL, 10);

		
		result = poisson_blend(img2, subimg, disX, disY);

		cvSaveImage("swapped.jpg", result);
	}
		//cvWaitKey(0);
	cvReleaseImage(&img0);
	cvReleaseImage(&img1);
	cvReleaseImage(&img2);

	return 0;
}