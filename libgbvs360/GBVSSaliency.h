#ifndef _GBVSSaliency_
#define _GBVSSaliency_


#include "Saliency.h"
#include <boost/shared_ptr.hpp>

#include <GBVS.h>


class GBVSSaliency : public Saliency {


private:
	
	boost::shared_ptr<GBVS>				m_GBVS;


public:
	GBVSSaliency 		();
	virtual ~GBVSSaliency()											{};



	virtual boost::shared_ptr<Saliency> 	newInstance();

	void setBlurFrac		(float blurFrac);
	void setSalmapmaxsize	(int salmapmaxsize);
	void setChanels			(std::string channels);


private:

	virtual void process(const cv::Mat &input, cv::Mat &output, bool normalize = true);
	
};



#endif


