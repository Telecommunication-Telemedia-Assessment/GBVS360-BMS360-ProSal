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




#include "Saliency360.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <limits>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "Projection.h"
#include "Options.h"
#include "GBVS360.h"
#include "BMSSaliency.h"
#include "ProjectedSaliency.h"
#include "EquatorialPrior.h"

Saliency360::Saliency360() {

	blurfrac		= 0.02f;
	salmapmaxsize 	= 60;
	featureScaling	= 1.0f;
	model 			= 1;
	channels		= "DIO";
	equatorialPrior = true;
	hmdMode 		= false;
	precomputedSaliency = false;
	projMaxDim		= 2000;
	bms360			= true;

	m_Projection = boost::shared_ptr<Projection>(new Projection());
	m_Saliency   = boost::shared_ptr<Saliency>(new BMSSaliency(bms360));

	m_Projection->nrApper = 70;
	m_Projection->nrrWidth = 960;
	m_Projection->nrrHeight = 960;

}


boost::shared_ptr<Saliency> Saliency360::newInstance() {
	Saliency360 *saliency360 = new Saliency360();
	saliency360->m_Projection = m_Projection;
	saliency360->m_Projection->nrApper = m_Projection->nrApper;
	saliency360->m_Projection->nrrWidth = m_Projection->nrrWidth;
	saliency360->m_Projection->nrrHeight = m_Projection->nrrHeight;

	saliency360->blurfrac = blurfrac;
	saliency360->salmapmaxsize = salmapmaxsize;
	saliency360->featureScaling = featureScaling;
	saliency360->model = model;
	saliency360->channels = channels;
	saliency360->equatorialPrior = equatorialPrior;
	saliency360->hmdMode = hmdMode;
	saliency360->precomputedSaliency = precomputedSaliency;
	saliency360->projMaxDim = projMaxDim;


	if(m_Saliency)
		saliency360->m_Saliency   = m_Saliency->newInstance();

	return boost::shared_ptr<Saliency>(saliency360);
}






void Saliency360::process(const cv::Mat &input, cv::Mat &output, bool normalize) {
	// Apply the right model
	

	if(model == 1 && m_Saliency) {
		computeHS(input, output, normalize);
	}

	if(model == 2) {
		if(bms360) {
			computeHS(input, output, normalize);
		} else {
			computeHES_GBVS360(input, output, normalize);
		}	
	}

	if (model == 3) {
		computeScanPath(input, output, normalize);
	}

	if(model == 4) {
		computeHES_ProjSal(input, output, normalize);
	}


	if (model == 5) {

		cv::Mat salmap1;
		computeHES_GBVS360(input, salmap1, normalize);

		cv::Mat salmap2;
		computeHES_ProjSal(input, salmap2, normalize);
	
		output = 0.45 * salmap1 + 0.55 * salmap2;
	}

	if(model == 6) {
		bool savedBMS360 = bms360;
		bms360 = true;
		computeHS(input, output, normalize);
		bms360 = savedBMS360;
	}

}




void Saliency360::computeHS(const cv::Mat &input, cv::Mat &output, bool normalize) {
	boost::shared_ptr<BMSSaliency> bms = boost::dynamic_pointer_cast<BMSSaliency>(m_Saliency);
	if(bms && (bms360 != bms->bms360)) {
		bms->bms360 = bms360;
	}

	if (bms) {
		bms->equatorialPrior = equatorialPrior;
		bms->maxDim = 2000;
		bms->dilatationWidth1 = static_cast<int>(fmax(round(7 * bms->maxDim / 400.f), 1.f));
		bms->dilatationWidth2 = static_cast<int>(fmax(round(9 * bms->maxDim / 400.f), 1.f));
		bms->blurStd = round(9 * bms->maxDim / 400);
	}

	m_Saliency->estimate(input, output, normalize);
}





void Saliency360::computeHES_GBVS360(const cv::Mat &input, cv::Mat &output, bool normalize) {
	GBVS360 gbvs(m_Projection);
	gbvs.blurfrac = blurfrac;
	gbvs.featureScaling = featureScaling;
	gbvs.salmapmaxsize = salmapmaxsize;
	gbvs.channels = channels;
	gbvs.equatorialPrior = equatorialPrior;
	gbvs.hmdMode = hmdMode;

	gbvs.compute(input, output, normalize);
}




void Saliency360::computeHES_ProjSal(const cv::Mat &input, cv::Mat &output, bool normalize) {
	double nrApper = m_Projection->nrApper;
	//m_Projection->nrApper = 110;

	boost::shared_ptr<BMSSaliency> bms = boost::dynamic_pointer_cast<BMSSaliency>(m_Saliency);
	if (bms) {
		bms->nb_projections = 1;
	}

	ProjectedSaliency projectedSaliency;
	projectedSaliency.setProjection(m_Projection);
	projectedSaliency.setSaliency(m_Saliency);
	projectedSaliency.maxDim = projMaxDim;

	projectedSaliency.estimate(input, output, normalize);

	if (equatorialPrior)
		applyEquatorialPrior(output, input);

	// restore to previous state
	m_Projection->nrApper = nrApper;

}


void Saliency360::computeScanPath(const cv::Mat &input, cv::Mat &output, bool normalize) {
	GBVS360 gbvs(m_Projection);
	gbvs.blurfrac = blurfrac;
	gbvs.featureScaling = featureScaling;
	gbvs.salmapmaxsize = salmapmaxsize;
	gbvs.channels = channels;
	gbvs.equatorialPrior = equatorialPrior;
	gbvs.hmdMode = hmdMode;


	cv::Mat salMap;
	// check if a saliency map is provided as input
	if (precomputedSaliency) {
		salMap = input;

		// if it is a BGR image, convert it.
		if (salMap.channels() == 3 && salMap.type() == CV_8UC3) {
			cv::cvtColor(salMap, salMap, cv::COLOR_BGR2GRAY);
			salMap.convertTo(salMap, CV_32FC1);
			salMap /= 255;
		}
		else {
			// otherwise just scale it as float
			salMap.convertTo(salMap, CV_32FC1);
		}


	} else {
		// If not, we need to compute it. 
		computeHES_GBVS360(input, salMap, normalize);
	}


	if (equatorialPrior)
		applyEquatorialPrior(salMap, input);


	// run the scan path prediction using GBVS360 framework.
	gbvs.scanPath(salMap, output, true);
}



