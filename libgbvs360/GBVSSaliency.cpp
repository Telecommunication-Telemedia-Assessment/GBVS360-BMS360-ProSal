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



