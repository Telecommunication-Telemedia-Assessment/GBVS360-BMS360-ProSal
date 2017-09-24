#include "Saliency.h"

#include <iostream>

void Saliency::estimate (const cv::Mat &input, cv::Mat &output, bool normalize) {
	process(input, output, normalize);

}




