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


