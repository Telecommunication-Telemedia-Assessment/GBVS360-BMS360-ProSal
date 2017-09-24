#ifndef _Segm_
#define _Segm_

#include <opencv2/core.hpp>

int segm(cv::Mat &fimage, 
		  cv::Mat &rgbim,
		  cv::Mat &labelImage,
		  int steps=3,
		  bool syn=true,
		  float spBW=7,
		  float fsBW=6.5,
		  unsigned int minArea=20,
		  int speedup=2,
		  unsigned int grWin=2,
		  float aij=0.3f,
		  float edgeThr=0.3f);



int msseg(cv::Mat &I, float spatialBW, float rangeBW, unsigned int minArea, cv::Mat &seg);
int msseg_f(cv::Mat &I, float spatialBW, float rangeBW, unsigned int minArea, cv::Mat &seg);



#endif

