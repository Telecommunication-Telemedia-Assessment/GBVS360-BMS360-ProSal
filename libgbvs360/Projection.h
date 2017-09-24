#ifndef _Projection_
#define _Projection_

#include <opencv2/core.hpp>

class Projection {

public:

	/* Image initialization variables */
	unsigned char nrRed;
	unsigned char nrGreen;
	unsigned char nrBlue;

	/* Projection float variables */
	double nrApper;
	double nrSightX;
	double nrSightY;
	double nrAzim;
	double nrHead;
	double nrElev;
	double nrRoll;
	double nrFocal;
	double nrPixel;

	/* Projection integer variables */
	int nrrWidth;
	int nrrHeight;
	int nrmWidth;
	int nrmHeight;
	int nreHeight;
	int nreWidth;
	int nrmCornerX;
	int nrmCornerY;

	/* Exportation options variables */
	int nrOption;

	/* Parallel processing variables */
	int nrThread;

	/* Interpolation tag variables */
	std::string nrMethod;

	/* projection method */
	int projMethod;


public:
	Projection();
	
	void equirectangularToRectilinear(const cv::Mat& input, cv::Mat& output);
	void equirectangularToRectilinear(const cv::Mat& input, cv::Mat& output, float azim, float elev, float roll = 0.f);

	void rectilinearToEquirectangular(const cv::Mat& input, cv::Mat& output);
	void rectilinearToEquirectangular(const cv::Mat& input, cv::Mat& output, float azim, float elev, float roll = 0.f);
	void rectilinearToEquirectangularFC3(const cv::Mat& input, cv::Mat& output); 
	void rectilinearToEquirectangularFC3(const cv::Mat& input, cv::Mat& output, float azim, float elev, float roll = 0.f);

}; 


#endif

