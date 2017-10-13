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


