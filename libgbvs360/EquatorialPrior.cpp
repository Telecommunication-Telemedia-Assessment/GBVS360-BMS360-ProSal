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



#include "EquatorialPrior.h"

#include <opencv2/opencv.hpp>



// ==============================================================================================================================
// Equatorial prior





float faceLine(const cv::Mat &image) {
	cv::Mat gray;
	cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

	cv::CascadeClassifier face_cascade;
	cv::CascadeClassifier faceProfil_cascade;
	bool faceCascadeEnabled = false;
	bool faceProfilCascadeEnabled = false;

	// ------------------------------------------------------------------------------------------------
	// init haar cascades framework

	if (face_cascade.load("data/haarcascade_frontalface_alt.xml")) {
		faceCascadeEnabled = true;
	}
	else {
		std::cerr << "[I] cannot open: haarcascade_frontalface_alt.xml" << std::endl;
	}

	if (faceProfil_cascade.load("data/haarcascade_profileface.xml")) {
		faceProfilCascadeEnabled = true;
	}
	else {
		std::cerr << "[I] cannot open: haarcascade_profileface.xml" << std::endl;
	}


	// ------------------------------------------------------------------------------------------------
	// find features

	std::list<cv::Rect> allFeatures;
	std::vector<cv::Rect> faceFeatures;
	if (faceCascadeEnabled)
		face_cascade.detectMultiScale(gray, faceFeatures, 2, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(15, 15));

	for (size_t i = 0; i < faceFeatures.size(); ++i) {
		allFeatures.push_back(faceFeatures[i]);
	}

	faceFeatures.clear();
	if (faceProfilCascadeEnabled)
		faceProfil_cascade.detectMultiScale(gray, faceFeatures, 2, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(15, 15));

	for (size_t i = 0; i < faceFeatures.size(); ++i) {
		allFeatures.push_back(faceFeatures[i]);
	}

	if (allFeatures.size() < 2) return -1;


	float mnX = static_cast<float>(image.cols);
	float mxX = 0;
	for (std::list<cv::Rect>::iterator it = allFeatures.begin(); it != allFeatures.end(); ++it) {
		float x = static_cast<float>(it->x + it->width / 2);

		if (x > mxX) mxX = x;
		if (x < mnX) mnX = x;
	}

	if ((mxX - mnX) / image.cols < 0.2) return -1;	// if the detected faces are collocated in one area, then skip it.

													// ------------------------------------------------------------------------------------------------
													// find equatorial line


	float meanY = 0; int nbLines = 0;
	float meanY2 = 0;
	for (std::list<cv::Rect>::iterator it = allFeatures.begin(); it != allFeatures.end(); ++it) {
		meanY += it->y + it->height / 2;
		meanY2 += (it->y + it->height / 2)*(it->y + it->height / 2);
		++nbLines;
	}

	if (nbLines == 0) return -1;

	meanY /= nbLines;
	meanY2 /= nbLines;


	float stdev = std::sqrt(meanY2 - meanY*meanY);
	float meanValue = meanY;

	meanY = 0; nbLines = 0;

	for (std::list<cv::Rect>::iterator it = allFeatures.begin(); it != allFeatures.end(); ++it) {
		float y = static_cast<float>(it->y + it->height / 2);

		if (std::abs(y - meanValue) > 1.3 * stdev) continue;

		meanY += y;
		++nbLines;
	}

	if (nbLines == 0) return meanValue;


	meanY /= nbLines;

	if (meanY < (gray.rows * 1 / 3) && allFeatures.size() < 2) { // we need at least two faces to justify an equatorial line lower higher the first third of the image
		return -1;
	}


	return meanY;

}


float salientCenter(const cv::Mat& image, int step) {

	std::vector<float> histogram(180 / step);
	int offset = 0; //static_cast<int>(30.f*(step/180.f));

	float sumHist = 0;
	for (int i = offset; i < 180 - offset; i += step) {
		float top = image.rows*static_cast<float>(i) / 180;
		float bottom = image.rows*static_cast<float>(i + step) / 180;

		float sum = 0;
		for (int ii = static_cast<int>(top); ii < static_cast<int>(bottom); ++ii) {
			for (int jj = 0; jj < image.cols; ++jj) {
				sum += image.at<float>(ii, jj);
			}
		}

		sum /= (bottom - top) * image.cols;
		histogram[i / step] = sum;
		sumHist += sum;
	}

	float loc = 0;
	for (size_t i = offset; i < histogram.size() - offset; ++i) {
		loc += i*step*histogram[i] / sumHist;
	}

	return image.rows * static_cast<float>(loc) / 180.f;

}


void applyGaussianEquatorialPrior(cv::Mat& image, float gaussianM, float gaussianSD) {

	for (int i = 0; i < image.rows; ++i) {
		float lat = (.5f - static_cast<float>(i) / image.rows) * 180.f;
		float prior = (.01f + .40f * exp(-((lat - gaussianM)*(lat - gaussianM)) / (gaussianSD)) + .75f * exp(-((lat - 100)*(lat - 100)) / (200)) + .75f * exp(-((lat + 90)*(lat + 90)) / (200))) / .41f;
		prior = std::min(1.f, prior);

		for (int j = 0; j < image.cols; ++j) {
			image.at<float>(i, j) = prior * image.at<float>(i, j);
		}
	}

}


void applyEquatorialPrior(cv::Mat& image, const cv::Mat& colorImageInput) {
	float scaling_factor = static_cast<float>(image.cols) / 1400.f;	// normalize the size of the images
	cv::Mat colorImage = colorImageInput.clone();

	cv::resize(colorImage, colorImage, cv::Size(colorImage.size().width / scaling_factor, colorImage.size().height / scaling_factor));
	float fc = faceLine(colorImage);
	float slCenter = salientCenter(image);


	slCenter = std::max(std::min((slCenter - image.rows / 2) / image.rows, 0.1f), -0.1f);
	slCenter = slCenter * image.rows + image.rows / 2;
	slCenter = (slCenter / scaling_factor + colorImage.rows / 2) / 2;

	if (fc > 0) {
		if (fc > colorImage.rows / 3)	// prior: faces are not higher than the first third of the image
			slCenter = fc;
	}


	float equatorialLine = (.5f - slCenter / colorImage.rows) * 180.f;

	applyGaussianEquatorialPrior(image, equatorialLine);


	double mn, mx;
	cv::minMaxLoc(image, &mn, &mx);
	image = (image - mn) / (mx - mn);

}


