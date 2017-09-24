#ifndef _Saliency_
#define _Saliency_

#include <opencv2/core.hpp>
#include <boost/shared_ptr.hpp> 

class Saliency {

public: 
	void estimate		(const cv::Mat &input, cv::Mat &output, bool normalize = true);
	virtual ~Saliency 	() 														{}



	virtual boost::shared_ptr<Saliency> 	newInstance() = 0;


private:

	virtual void process 	(const cv::Mat &input, cv::Mat &output, bool normalize) = 0;


};



#endif

