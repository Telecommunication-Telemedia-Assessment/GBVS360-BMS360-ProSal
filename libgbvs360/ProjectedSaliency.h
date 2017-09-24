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

