#include "Segm.h"


#include <math.h>

/* general include files */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* edison include files */
#include "segm/tdef.h"
#include "segm/msImageProcessor.h"
#include "edge/BgImage.h"
#include "edge/BgDefaults.h"
#include "edge/BgEdge.h"
#include "edge/BgEdgeList.h"
#include "edge/BgEdgeDetect.h"

#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

const kernelType DefualtKernelType = Uniform;
const unsigned int DefualtSpatialDimensionality = 2;

#define max(a, b) ((a)>(b))?(a):(b)
#define min(a, b) ((a)<(b))?(a):(b)

int msseg(cv::Mat &I, float spatialBW, float rangeBW, unsigned int minArea, cv::Mat &seg) {
	cv::Mat LUV, BGR;
	BGR = cv::Mat(cv::Size(I.cols, I.rows), CV_32FC3);
	for(int i = 0 ; i < I.rows ; ++i) {
		for(int j = 0 ; j < I.cols ; ++j) {
			cv::Point3_<unsigned char> &bgrU = I.at< cv::Point3_<unsigned char> >(i,j);
			cv::Point3_<float> &bgrF		 = BGR.at< cv::Point3_<float> >(i,j);

			bgrF.x = static_cast<float>(bgrU.x) / 255.f;
			bgrF.y = static_cast<float>(bgrU.y) / 255.f;
			bgrF.z = static_cast<float>(bgrU.z) / 255.f;

		}
	}


	cv::cvtColor(BGR, LUV, CV_BGR2Luv);

	int nbClass = segm(LUV, I, seg, 2, true, spatialBW, rangeBW, minArea, HIGH_SPEEDUP, 2, .3f, .3f);

	cv::cvtColor(LUV, BGR, CV_Luv2BGR);


	for(int i = 0 ; i < I.rows ; ++i) {
		for(int j = 0 ; j < I.cols ; ++j) {
			cv::Point3_<unsigned char> &bgrU = I.at< cv::Point3_<unsigned char> >(i,j);
			cv::Point3_<float> &bgrF		 = BGR.at< cv::Point3_<float> >(i,j);

			bgrU.x = static_cast<unsigned char>(bgrF.x*255.f);
			bgrU.y = static_cast<unsigned char>(bgrF.y*255.f);
			bgrU.z = static_cast<unsigned char>(bgrF.z*255.f);

		}
	}


	return nbClass;
}



int msseg_f(cv::Mat &I, float spatialBW, float rangeBW, unsigned int minArea, cv::Mat &seg) {
	cv::Mat LUV;

	cv::cvtColor(I, LUV, CV_BGR2Luv);

	int nbClass = segm(LUV, I, seg, 2, true, spatialBW, rangeBW, minArea, HIGH_SPEEDUP, 2, .3f, .3f);

	cv::cvtColor(LUV, I, CV_Luv2BGR);

	return nbClass;
}



int segm(cv::Mat &featureIm,
		  cv::Mat &rgbImg,
		  cv::Mat &labelImage,
		  int steps,
		  bool syn,
		  float spBW,
		  float fsBW,
		  unsigned int minArea,
		  int sul,
		  unsigned int grWin,
		  float aij,
		  float edgeThr) {
    int ii;
    
	float * fimage = reinterpret_cast<float*>(featureIm.data);
	int w=rgbImg.cols; 
	int h=rgbImg.rows;
	int N=3;
	unsigned char * rgbim = rgbImg.data;

    
    msImageProcessor ms;
    ms.DefineLInput(fimage, h, w, N); // image array should be after permute
    if (ms.ErrorStatus) {
		std::cerr << "Mean shift input error " << ms.ErrorMessage << std::endl;
		return false;
	}
    
    kernelType k[2] = {DefualtKernelType, DefualtKernelType};
    int P[2] = {DefualtSpatialDimensionality, N};
    float tempH[2] = {1.0, 1.0};
    ms.DefineKernel(k, tempH, P, 2); 
	if (ms.ErrorStatus) {
		std::cerr << "edison_wraper:edison " << ms.ErrorMessage << std::endl;
		return false;
	}

    float * conf = NULL;
    float * grad = NULL;
    float * wght = NULL;
    
    if (syn) {
        /* perform synergistic segmentation */
        conf = new float[w*h];
        grad = new float[w*h];
          
        BgImage rgbIm;
        rgbIm.SetImage(rgbim, w, h, true);
        BgEdgeDetect edgeDetector(grWin);
        edgeDetector.ComputeEdgeInfo(&rgbIm, conf, grad);
        
        wght = new float[w*h];
        
        for ( ii = 0 ; ii < w*h; ii++ ) {
            wght[ii] = (grad[ii] > .002) ? aij*grad[ii]+(1-aij)*conf[ii] : 0;
        }
        ms.SetWeightMap(wght, edgeThr);
		if (ms.ErrorStatus) {
			std::cerr << "Mean shift set weights " << ms.ErrorMessage << std::endl;
			delete[] conf;
			delete[] wght;
			delete[] grad;
			return false;
		}

    }
    ms.Filter(static_cast<int>(spBW), fsBW, static_cast<SpeedUpLevel>(sul));

	if (ms.ErrorStatus) {
		std::cerr << "Mean shift filter: " << ms.ErrorMessage << std::endl;
		delete[] conf;
		delete[] wght;
		delete[] grad;
		return false;
	}

    if (steps == 2) {
        ms.FuseRegions(fsBW, minArea);
		if (ms.ErrorStatus) {
			std::cerr << "Mean shift fuse: " << ms.ErrorMessage << std::endl;
			delete[] conf;
			delete[] wght;
			delete[] grad;
			return false;
		}
    }
    
    ms.GetRawData(fimage);
    
    int* labels;
    float* modes;
    int* count;
    int RegionCount = ms.GetRegions(&labels, &modes, &count);

	labelImage = cv::Mat(cv::Size(rgbImg.cols, rgbImg.rows), CV_32FC1);
	for(int i = 0 ; i < rgbImg.rows ; ++i) {
		for(int j = 0 ; j < rgbImg.cols ; ++j) {
			labelImage.at<float>(i,j) = static_cast<float>(labels[i*rgbImg.cols+j]);
		}
	}


    return RegionCount;    
}




