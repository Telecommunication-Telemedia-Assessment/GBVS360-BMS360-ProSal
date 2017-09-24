#ifndef _EquatorialPrior_
#define _EquatorialPrior_

#include <opencv2/core.hpp>

float salientCenter 				(const cv::Mat& image, int step = 5);
void applyGaussianEquatorialPrior	(cv::Mat& image, float gaussianM = 0.f, float gaussianSD = 700.f);
void applyEquatorialPrior			(cv::Mat& image, const cv::Mat& colorImageInput);


#endif

