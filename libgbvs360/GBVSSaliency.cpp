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




#include "GBVSSaliency.h"


GBVSSaliency::GBVSSaliency() {
	m_GBVS = boost::shared_ptr<GBVS>(new GBVS());
	m_GBVS->useCSF = false;
	m_GBVS->nbThreads = 1;
}

void GBVSSaliency::setBlurFrac(float blurfrac) {
	m_GBVS->blurfrac = blurfrac;
}

void GBVSSaliency::setSalmapmaxsize(int salmapmaxsize) {
	m_GBVS->salmapmaxsize = salmapmaxsize;
}

void GBVSSaliency::setChanels(std::string channels) {
	m_GBVS->channels = channels;
}

void GBVSSaliency::process(const cv::Mat &input, cv::Mat &output, bool normalize) {

	m_GBVS->compute(input, output, normalize);
	
}


boost::shared_ptr<Saliency> GBVSSaliency::newInstance() {
	GBVSSaliency *newInst = new GBVSSaliency();

	newInst->setBlurFrac(m_GBVS->blurfrac);
	newInst->setSalmapmaxsize(m_GBVS->salmapmaxsize);
	newInst->setChanels(m_GBVS->channels);

	return boost::shared_ptr<Saliency>(newInst);

}



