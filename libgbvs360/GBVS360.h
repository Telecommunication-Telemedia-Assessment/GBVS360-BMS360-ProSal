#ifndef _GBVS360_
#define _GBVS360_

#include <GBVS.h>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

class Projection;



struct ProjectedFrame {
	cv::Mat 					rectilinearFrame;
	cv::Mat 					saliency;
	std::list<Feature> 			features;
	int 						nrElev;
	int 						nrAzim;
	bool						taskDone;
} ;


struct FixationOption {
	int x;
	int y;
	float probability;
	float value;
};


class GBVS360 : public GBVS {

private:

	cv::Mat												m_InputImage;
	boost::shared_ptr<Projection> 						m_Projection;
	std::vector< boost::shared_ptr<GBVS> > 				m_GBVSWorkers;
	boost::mutex										m_mutex;
	std::list<ProjectedFrame> 							m_ProjectedFrames;
	std::vector< std::vector< std::vector< bool > > > 	featureDone; 
	cv::Size 											m_EquirectangularFrameSize;


public:

// ----------------------------------------------------------------------------------------------------------------------
// model parameters

	float 								blurfrac;
	float								featureScaling;
	bool								hmdMode;






public:
					 GBVS360 						(boost::shared_ptr<Projection> projection);
	virtual 		~GBVS360 						()						{};


	virtual void 	compute 				(const cv::Mat& imgBGR, cv::Mat &out, bool normalize = true);
	virtual void 	scanPath				(const cv::Mat& input, cv::Mat &out, bool inputSaliencyMap = false);





private:

	void 			getRectilinearFrames	(const cv::Mat &inputImage);
	void			getRectilinearFramesJob	(const cv::Mat &inputImage);


	void 			getRectilinearFeatures	     ();
	void			getRectilinearFeaturesJob 	 (int workerID);
	void 			getEquirectangularFeatures   ();
	void			getEquirectangularFeaturesJob();


	cv::Mat 		runScanPath 				(const cv::Mat& saliency, const cv::Mat& imgBGR, const std::vector<FixationOption>& groundTruthFixations, const cv::Mat &lx, const cv::Mat &mm, int initPosition);
	void	 		getTransitionMatrix 		(const cv::Mat &lx, const cv::Mat &mm, cv::Mat &distanceMatrix, int position, float scaling) const;
protected:

// ----------------------------------------------------------------------------------------------------------------------
// compute features from GBVS360

	void 			getFaceDetectionFeature		 (int height, int width);
	void 			getSegmentationFeature		 (const cv::Mat &inputImage, Feature &segFeature);
	void 			getLinPerFeature 			 (const cv::Mat &inputImage, Feature &linPerFeature);

// ----------------------------------------------------------------------------------------------------------------------
// redefine GBVS functions 


	virtual void attenuateBordersGBVS	 	(cv::Mat &map, int borderSize) 										const;
	virtual cv::Mat simpledistance				(const std::pair<int, int>& dim, int cyclic_type) 				const;

	const cv::Mat *	findMap						 (char channel, int level, int type) 							const ;
} ;


#endif


