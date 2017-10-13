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




#ifndef _ShiftImage_
#define _ShiftImage_

#include <opencv2/core.hpp>

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// perform a shift the image


template<typename T>
struct ColorTrait{
	static int Value; 
	typedef cv::Point3_<T> TPoint;
};

template<typename T>
int ColorTrait<T>::Value = CV_8UC3;

template<>
struct ColorTrait<unsigned char>{
	static int Value; 
	typedef cv::Point3_<unsigned char> TPoint;
};

int ColorTrait<unsigned char>::Value = CV_8UC3;

template<>
struct ColorTrait<float>{
	static int Value; 
	typedef float TPoint;
};

int ColorTrait<float>::Value = CV_32FC1;



template<typename T>
cv::Mat shiftImage(const cv::Mat &input, int x, int y) {
	if(x == 0 && y == 0) return input;


	cv::Mat shiftedImage(input.size(), ColorTrait<T>::Value);

	for(int i = 0 ; i < input.rows ; ++i) {
		for(int j = 0 ; j < input.cols ; ++j) {
			int xoff = j+x;
			if(xoff >= 0) xoff = xoff % input.cols;
			else xoff = input.cols + xoff;

			int yoff = i+y;
			if(yoff >= 0) yoff = yoff % input.rows;
			else yoff = input.rows + yoff;

			shiftedImage.at< typename ColorTrait<T>::TPoint >(i,j) = input.at< typename ColorTrait<T>::TPoint >(yoff, xoff);
		}
	}

	return shiftedImage;
}







#endif

