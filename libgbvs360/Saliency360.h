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




#ifndef _Saliency360_
#define _Saliency360_

#include "Saliency.h"
#include <boost/shared_ptr.hpp>



class Projection;



class Saliency360 : public Saliency {


private:

	boost::shared_ptr<Projection> 						m_Projection;
	boost::shared_ptr<Saliency>							m_Saliency;


public:

	int 								model;
	float 								blurfrac;
	float								featureScaling;
	int 								salmapmaxsize;
	std::string 						channels;
	bool								equatorialPrior;
	bool								hmdMode;
	bool								precomputedSaliency;
	int									projMaxDim;
	bool								bms360;


public:
					 Saliency360 			();
	virtual 		~Saliency360()											{};




	inline void 	setProjection			(boost::shared_ptr<Projection>  projection)				{ m_Projection = projection; };
	inline void		setSaliency				(boost::shared_ptr<Saliency>    saliency)				{ m_Saliency = saliency; }
	



	inline boost::shared_ptr<Projection> &  getProjection		()									{ return m_Projection; };
	inline boost::shared_ptr<Saliency> & 	getSaliency			()									{ return m_Saliency; };




	virtual boost::shared_ptr<Saliency> newInstance();


private:

	virtual void 	process					(const cv::Mat &input, cv::Mat &output, bool normalize = true);

	void computeHS							(const cv::Mat &input, cv::Mat &output, bool normalize = true);
	void computeHES_GBVS360					(const cv::Mat &input, cv::Mat &output, bool normalize = true);
	void computeHES_ProjSal					(const cv::Mat &input, cv::Mat &output, bool normalize = true);
	void computeScanPath					(const cv::Mat &input, cv::Mat &output, bool normalize = true);

};

#endif

