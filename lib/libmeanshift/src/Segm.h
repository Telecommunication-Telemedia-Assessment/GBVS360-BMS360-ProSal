// **************************************************************************************************
//
// The MIT License (MIT)
// 
// Copyright (c) 2017 Pierre Lebreton
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
// associated documentation files (the "Software"), to deal in the Software without restriction, including 
// without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the 
// following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or substantial 
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// **************************************************************************************************



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

