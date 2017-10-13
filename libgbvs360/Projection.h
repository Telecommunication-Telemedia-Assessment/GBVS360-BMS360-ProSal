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




#ifndef _Projection_
#define _Projection_

#include <opencv2/core.hpp>

class Projection {

public:

	/* Image initialization variables */
	unsigned char nrRed;
	unsigned char nrGreen;
	unsigned char nrBlue;

	/* Projection float variables */
	double nrApper;
	double nrSightX;
	double nrSightY;
	double nrAzim;
	double nrHead;
	double nrElev;
	double nrRoll;
	double nrFocal;
	double nrPixel;

	/* Projection integer variables */
	int nrrWidth;
	int nrrHeight;
	int nrmWidth;
	int nrmHeight;
	int nreHeight;
	int nreWidth;
	int nrmCornerX;
	int nrmCornerY;

	/* Exportation options variables */
	int nrOption;

	/* Parallel processing variables */
	int nrThread;

	/* Interpolation tag variables */
	std::string nrMethod;

	/* projection method */
	int projMethod;


public:
	Projection();
	
	void equirectangularToRectilinear(const cv::Mat& input, cv::Mat& output);
	void equirectangularToRectilinear(const cv::Mat& input, cv::Mat& output, float azim, float elev, float roll = 0.f);

	void rectilinearToEquirectangular(const cv::Mat& input, cv::Mat& output);
	void rectilinearToEquirectangular(const cv::Mat& input, cv::Mat& output, float azim, float elev, float roll = 0.f);
	void rectilinearToEquirectangularFC3(const cv::Mat& input, cv::Mat& output); 
	void rectilinearToEquirectangularFC3(const cv::Mat& input, cv::Mat& output, float azim, float elev, float roll = 0.f);

}; 


#endif

