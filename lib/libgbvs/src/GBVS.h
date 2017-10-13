// **************************************************************************************************
//
// This program was implemented by Pierre Lebreton
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// **************************************************************************************************




#ifndef _GBVS_
#define _GBVS_

#include <vector>
#include <list>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>
#include <boost/thread/mutex.hpp>


//#define WITH_FFTW

#ifdef WITH_FFTW
	#include <fftw3.h>
	#include "fftw++.h"
#endif

struct FeatureMap {
	cv::Mat 	map;
	int 		type;
	int 		level;
	int 		channel;
	bool 		taskDone;
};

struct Feature {
	std::list< FeatureMap > maps;
	std::string description;
	float weight;
	int channel;
}; 


struct GaborFilter {
	cv::Mat g0;
	cv::Mat g90;
};

struct Frame {
	cv::Mat lx;
	cv::Mat d;
	std::vector< std::pair<int, int> > 	dims;
	std::vector<int> 					multilevels;

};


class GBVS {

public:

	// general
	int 	salmapmaxsize;		// size of output saliency maps (maximum dimension) don't set this too high (e.g., >60) 
	float	blurfrac;			// final blur to apply to master saliency map

	// feature channel parameters
	std::string channels;
	float	colorWeight;
	float	intensityWeight;
	float 	orientationWeight;
	float	contrastWeight;
	float 	flickerWeight;
	float 	motionWeight;
	float  	dklcolorWeight;
	float 	linperWeight;
	float 	faceFeatureWeight;
	float 	blurFeatureWeight;

	std::vector<float> 	gaborangles;
	std::vector<float> 	motionAngles;
	float	contrastwidth;
	float	flickerNewFrameWt;
	

	// GBVS parameters
	float	unCenterBias;
	std::vector<int> levels;
	std::vector<int> multilevels;
	int 			 maxcomputelevel;

	float	sigma_frac_act;
	float 	sigma_frac_norm;
	int		num_norm_iters;

	double  tol;
	int 	cyclic_type;
	int 	normalizationType;
	int 	normalizeTopChannelMaps;


	// CSF parameters
	bool  useCSF;
	float viewingDistance;
	float nbPixelPerDegree;

	// equatorial prior for 360 deg. images
	bool equatorialPrior;



	std::list<Feature> 			features;



	bool 						initDone;
	std::vector<int> 			salmapmaxsize_v;



	int 						nbThreads;



protected:

	std::list<FeatureMap> 		allmaps;
	cv::Mat 					master_map;
	std::list<FeatureMap>		channelMaps;


	// internal feature graph-activation
	Frame 						grframe;

private:

	// internal features
	std::vector<GaborFilter> 	gaborFilters;
	std::vector<float>          mapWeights;


	// internal csf variable
#ifdef WITH_FFTW
	static int 					 instanceCounter;
	static fftwpp::fft2d		*forward;
	static fftwpp::fft2d		*backward;
#endif


	boost::mutex 				 gbvs_mutex;

	


	bool initCascadeClassifier;
	bool faceCascadeEnabled;
	bool eyeCascadeEnabled;
	cv::CascadeClassifier face_cascade;
	cv::CascadeClassifier eye_cascade;



public:



	GBVS();
	virtual ~GBVS();



	// ------------------------------------------------------------------------------------------------
	// Run model 



	virtual void compute 	(const cv::Mat& imgBGR, cv::Mat &out, bool normalize = true);
	void computeActivation	(const cv::Mat& featureMap, cv::Mat &out, bool normalize = true);




	void computeFeatures	(const cv::Mat &imgBGR);
	void gbvsActivation		(const cv::Mat &imgBGR, cv::Mat &out, bool normalize = true);




	// ------------------------------------------------------------------------------------------------
	// Initialization 

protected:
	void		initGBVS			(const cv::Mat& img_u);
	void 		clear 				();


public: 
	void 		makeGaborFilters 	();



protected:
	void 		graphsalinit		(const std::vector<int> &dims);


	// ------------------------------------------------------------------------------------------------
	// Graph-related functions



	void 		getDims				(const std::vector<int> &dims, const std::vector<int> &multilevels, std::vector<cv::Mat> &o_apyr, std::vector< std::pair<int, int> > &o_dims) const ;
	void 		formMapPyramid		(const cv::Mat& a, const std::vector<int> &deltas, std::vector<cv::Mat> &apyr, std::vector< std::pair<int, int> > &o_dims) const ;
	void 		namenodes			(const std::vector< std::pair<int, int> > &dims, int &N, std::vector<cv::Mat> &nam) 		const ;
	cv::Mat  	makeLocationMap		(const std::vector< std::pair<int, int> > &dims, const std::vector<cv::Mat> &nam, int N) 	const;
	std::vector< std::vector<int> > partitionindex(int N, int M) 																const ;
	cv::Mat 	connectMatrix		(const std::vector< std::pair<int, int> >& dims , 
									 const cv::Mat&  lx , 
									 int inter_type , 
									 int intra_type , 
									 int cyclic_type ) 																const;
	cv::Mat 	distanceMatrix		(const std::vector< std::pair<int, int> >& dims, 
							 		 const cv::Mat&  lx, 
									 int cyclic_type) 																const;



	virtual cv::Mat simpledistance	(const std::pair<int, int>& dim, int cyclic_type) 								const;
	void 		arrangeLinear 		(const std::vector<cv::Mat> &apyr, const std::vector< std::pair<int, int> > &dims, std::vector<double> &o_datas) const;
	void 		assignWeights		(const std::vector<double>& AL, const cv::Mat& dw, cv::Mat &mm, int algtype) 	const;
	void 		columnNormalize		(cv::Mat &mm) 																	const;
	void 		principalEigenvectorRaw(const cv::Mat& markovA, float tol, std::vector<double>& AL, int &iteri) 	const ;
	float 		sparseness 			(const cv::Mat& markovA) 														const;
	void 		sumOverScales		(std::vector<double> &A, const cv::Mat &lx, int size, std::vector<double> &vo) 	const;
	




	cv::Mat 	graphsalapply 		(const cv::Mat &A, const Frame& frame, float sigma_frac, int num_iters, int algtype, float tol) const;






	// ------------------------------------------------------------------------------------------------
	// Features related functions



	cv::Mat 	makeGaborFilterGBVS (float orientation, float phase, float stddev = 2, float elongation = 2, float filterSize = -1, float filterPeriod = 3.14159265358979116f, bool makeDisk = false) const;
	void 		rgb2dkl 			(cv::Mat &imgR, cv::Mat &imgG, cv::Mat &imgB, cv::Mat &imgL, cv::Mat &imgC1, cv::Mat &imgC2) const;
	cv::Mat 	safeDivideGBVS 		(const cv::Mat &u, const cv::Mat &v) 		const;
	cv::Mat 	contrast 			(const cv::Mat &img, int size) 				const;
	cv::Mat 	subsample 			(const cv::Mat& img) 						const;

	cv::Mat 	defocusBlurMap 		(const cv::Mat& image)						const;
	cv::Mat 	faceFeaturesDectection(const cv::Mat& image);
	Feature 	segmentationFeature (const cv::Mat& image) 						const;
	
	

	cv::Mat		getFloatingImageC3 	(const cv::Mat &input) 						const;
	cv::Mat 	getUCharImageC1 	(const cv::Mat &input) 						const;
	void 		getFeatureMaps		(const cv::Mat& img);

	void 		getPerspectiveFeatureJob(const cv::Mat& img, cv::Mat &output, double *reliability) const;





	// ------------------------------------------------------------------------------------------------
	// Model related function




	void 		computeActivation	();
	void 		computeActivationJob();
	void		normalizeActivation	();
	void 		normalizeActivationJob();
	void 		averageByFeatureChannel();
	void		sumChannels			(bool normalize);
	void 		blurMasterMap		(bool normalize);



	cv::Mat 	maxNormalizeStdGBVS (const cv::Mat &map) 						const;


	// ------------------------------------------------------------------------------------------------
	// Equatorial prior





	float 		faceLine 					(const cv::Mat &image) 												const;
	float 		salientCenter 				(const cv::Mat& image, int step = 5) 								const;
	void		applyGaussianEquatorialPrior(cv::Mat& image, float gaussianM = 0.f, float gaussianSD = 700.f) 	const;
	void 		applyEquatorialPrior		(cv::Mat& image, const cv::Mat& colorImageInput) 					const ;







	// ------------------------------------------------------------------------------------------------
	// utility function

	cv::Mat 	transpose 			(const cv::Mat& input) 						const;
	virtual void attenuateBordersGBVS(cv::Mat &map, int borderSize) 			const;

	void 		lowPass6yDecY 		(double* sptr, double* rptr, int w, int hs) const;
	void 		lowPass6xDecX 		(double* sptr, double* rptr, int ws, int h) const;






	// ------------------------------------------------------------------------------------------------
	// Contrast sensitivity function. - Not part of original GBVS model -


	cv::Mat 	applyCSF			(const cv::Mat& img1);

#ifdef WITH_FFTW

	cv::Mat 	getCSF				(const cv::Mat& img1, fftwpp::fft2d &Forward, fftwpp::fft2d &Backward) 	const;
	void	 	applyAchromaticCSF	(Complex * fft, int height, int width) 				const;
	void 		applyChromaticCSF 	(Complex * fft, int height, int width, int color) 	const;
	void  		fftShift 			(Complex * fft, int height, int width) 				const;

#endif

};


#endif

