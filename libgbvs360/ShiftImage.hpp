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

