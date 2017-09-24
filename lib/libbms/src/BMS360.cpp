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