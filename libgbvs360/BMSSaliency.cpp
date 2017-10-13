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




#include "BMSSaliency.h"

#include <BMS.h>
#include <BMS360.h>
#include <opencv2/opencv.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "ShiftImage.hpp"
#include "EquatorialPrior.h"



BMSSaliency::BMSSaliency(bool loc_bms360) {
	sampleStep			= 8;
	dilatationWidth1	= 3;
	dilatationWidth2	= 11;
	blurStd				= 20;
	normalize			= 1;
	handleBorder		= 0;
	colorSpace 			= CL_Lab;
	whitening			= true;
	maxDim				= 400.f;
	equatorialPrior 	= false;
	nb_projections 		= 4;
	bms360				= loc_bms360;
}


boost::shared_ptr<Saliency> BMSSaliency::newInstance() {
	BMSSaliency *saliency = new BMSSaliency(bms360);
	saliency->sampleStep = sampleStep;
	saliency->dilatationWidth1 = dilatationWidth1;
	saliency->dilatationWidth2 = dilatationWidth2;
	saliency->blurStd = blurStd;
	saliency->handleBorder = handleBorder;
	saliency->colorSpace = colorSpace;
	saliency->whitening = whitening;
	saliency->maxDim = maxDim;
	saliency->nb_projections = nb_projections;

	return boost::shared_ptr<Saliency>(saliency);

}


void BMSSaliency::process(const cv::Mat &inputImage, cv::Mat &sMap, bool normalize) {
	Configuration conf;

    conf.sampleStep = sampleStep;
	conf.dilatationWidth1 = dilatationWidth1;
	conf.dilatationWidth2 = dilatationWidth2;
	conf.blurStd = blurStd;
	conf.normalize = normalize;
	conf.handleBorder = handleBorder;
	conf.colorSpace = colorSpace;
	conf.whitening = whitening;
	conf.maxDim = static_cast<int>(maxDim);
	conf.equatorialPrior = equatorialPrior;


    std::vector<cv::Mat> outputs(nb_projections);
    boost::thread_group g;
    for(int i = 0 ; i < nb_projections ; ++i) {
    	g.create_thread(boost::bind(&BMSSaliency::processJob, this, i, nb_projections, boost::ref(inputImage), boost::ref(outputs), boost::ref(conf)));

    }
    g.join_all();



    // for(int i = 0 ; i < nb_projections ; ++i) {
    // 	processJob(i, nb_projections, inputImage, outputs, conf);
    // }

    sMap = outputs[0];
    for(int i = 1 ; i < nb_projections ; ++i) {
    	sMap = sMap + shiftImage<float>(outputs[i], -i * outputs[i].cols / nb_projections, 0);
    }

	if(normalize) {
		if (blurStd > 0) {
			int blur_width = (int)MIN(floor(blurStd) * 4 + 1, 51);
			cv::GaussianBlur(sMap, sMap, cv::Size(blur_width, blur_width), blurStd, blurStd);
		}

		if(nb_projections > 1) {
			double mn, mx;
			cv::minMaxLoc(sMap, &mn, &mx);
			sMap = ((sMap-mn) / (mx - mn)) ;
		}
	}

	cv::resize(sMap, sMap, inputImage.size());



	if(equatorialPrior)
		applyEquatorialPrior(sMap, inputImage);

}



void BMSSaliency::processJob(int workerID, int nb_shift, const cv::Mat &input, std::vector<cv::Mat> &outputs, Configuration &conf) {
	cv::Mat inputImage = shiftImage<unsigned char>(input, workerID * input.cols / nb_shift, 0);

	processOneProjection(inputImage, outputs[workerID], conf.maxDim, conf.dilatationWidth1, conf.dilatationWidth2, conf.normalize, conf.handleBorder, conf.colorSpace, conf.whitening, conf.sampleStep, conf.blurStd);
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// apply BMS on one frame

void BMSSaliency::processOneProjection(const cv::Mat &input, cv::Mat &output, int maxDim, int dilatationWidth1, int dilatationWidth2, int normalize, int handleBorder, int colorSpace, bool whitening, int sampleStep, float ) {

	cv::Mat src_small;
	float w = (float)input.cols, h = (float)input.rows;
	float maxD = fmax(w, h);
	cv::resize(input, src_small, cv::Size((int)(maxDim*w / maxD), (int)(maxDim*h / maxD)), 0.0, 0.0, cv::INTER_AREA);

	boost::shared_ptr<BMS> bms;
	if(bms360) {
		bms = boost::shared_ptr<BMS>(new BMS360(src_small, dilatationWidth1, normalize == 1, handleBorder == 1, colorSpace, whitening));
	} else {
		bms = boost::shared_ptr<BMS>(new BMS(src_small, dilatationWidth1, normalize == 1, handleBorder == 1, colorSpace, whitening));
	}

	bms->computeSaliency((double)sampleStep);

	cv::Mat result = bms->getSaliencyMap(false);

	if (dilatationWidth2 > 0)
		dilate(result, output, cv::Mat(), cv::Point(-1, -1), dilatationWidth2);

}




