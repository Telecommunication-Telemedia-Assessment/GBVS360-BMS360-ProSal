#ifndef _VPD_
#define _VPD_

#include <opencv2/core.hpp>

std::vector< std::pair<float, float> >  extractVP 					(const cv::Mat &image);
cv::Mat 								vanishingLineFeatureMap 	(const cv::Mat &image, double *reliability = NULL);
cv::Mat 								vanishingLineFeatureMapF64	(const cv::Mat &image, double *reliability = NULL);
cv::Mat 								vanishingLineFeatureMapF64C3(const cv::Mat &image,  double *reliability = NULL);

float			 						horizonLine					(cv::Mat &image);

#endif


