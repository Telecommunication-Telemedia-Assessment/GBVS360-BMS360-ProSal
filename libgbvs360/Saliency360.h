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

