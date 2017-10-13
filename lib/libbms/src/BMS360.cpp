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




#include "BMS360.h"
#include <opencv2/highgui.hpp>

void BMS360::bmsNormalize(cv::Mat& map1, cv::Mat& map2) {
    
    scaleBooleanMap(map1);
    scaleBooleanMap(map2);

    cv::normalize(map1, map1, 1.0, 0.0, cv::NORM_L2);
	cv::normalize(map2, map2, 1.0, 0.0, cv::NORM_L2);

}


void BMS360::scaleBooleanMap(cv::Mat& map) {

    for(int i = 0 ; i < map.rows ; ++i) {
        float c = std::cos(3.1415926535898f * static_cast<float>(map.rows / 2 - i) / map.rows );
        for(int j = 0 ; j < map.cols ; ++j) {
            map.at<float>(i,j) *= c;
        }
    }
    
}