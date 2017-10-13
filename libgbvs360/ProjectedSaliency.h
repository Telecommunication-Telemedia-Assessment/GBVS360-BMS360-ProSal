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




#ifndef _ProjectedSaliency_
#define _ProjectedSaliency_

#include "Saliency.h"
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <vector>
#include <list>

#include "GBVS360.h" 



class Projection;



class ProjectedSaliency : public Saliency {


private:

	boost::shared_ptr<Projection> 						m_Projection;
	boost::shared_ptr<Saliency>							m_Saliency;

	std::list<ProjectedFrame> 							m_ProjectedFrames;
	std::vector< boost::shared_ptr<Saliency> > 			m_SaliencyWorkers;
	boost::mutex										m_mutex;
	cv::Mat												m_EquirectangularSaliency;




public:

	float 								blurfrac;
	int									maxDim;



public:
					 ProjectedSaliency 		();
	virtual 		~ProjectedSaliency		()													{};


	inline void 	setProjection			(boost::shared_ptr<Projection> &projection)			{ m_Projection = projection; };
	inline void		setSaliency				(boost::shared_ptr<Saliency>   &saliency)			{ m_Saliency = saliency; }


	virtual boost::shared_ptr<Saliency> newInstance();


private:



	virtual void 	process					(const cv::Mat &input, cv::Mat &output, bool normalize = true);



	void 			getRectilinearFrames	(const cv::Mat &inputImage);
	void			getRectilinearFramesJob	(const cv::Mat &inputImage);



	// baselines 1 & 2
	void 			getRectilinearSaliency	  ();
	void			getRectilinearSaliencyJob (int workerID);
	void 			getEquirectangularSaliency();
	void 			getActivation 			  (cv::Mat &output);


};

#endif

