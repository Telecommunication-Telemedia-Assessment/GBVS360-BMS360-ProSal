#ifndef _BMSSaliency_
#define _BMSSaliency_

#include "Saliency.h"

struct Configuration {
	int sampleStep;
	int dilatationWidth1;
	int dilatationWidth2;
	float blurStd;
	int normalize;
	int handleBorder;
	int colorSpace;
	bool whitening;
	int maxDim;
	bool equatorialPrior;
}; 




class BMSSaliency : public Saliency {

public:
	int 				sampleStep;
	int 				dilatationWidth1;
	int 				dilatationWidth2;
	float				blurStd;
	bool				normalize;
	bool				handleBorder;
	int 				colorSpace;
	bool 				whitening;
	float				maxDim;
	bool				equatorialPrior;
	int 				nb_projections;
	bool 				bms360;


public:
	BMSSaliency 		(bool bms360);
	virtual ~BMSSaliency()											{};


	virtual boost::shared_ptr<Saliency> 	newInstance();


private:


	// ------------------------------------------------------------------------------------------------
	// Apply saliency model + Multiple map fusion



	virtual void process(const cv::Mat &input, cv::Mat &output, bool normalize = true);
	void processOneProjection(const cv::Mat &input, cv::Mat &output, int maxDim, int dilatationWidth1, int dilatationWidth2, int normalize, int handleBorder, int colorSpace, bool whitening, int sampleStep, float blurStd);
	void processJob(int workerID, int nb_shift, const cv::Mat &input, std::vector<cv::Mat> &outputs, Configuration &conf);





};



#endif

