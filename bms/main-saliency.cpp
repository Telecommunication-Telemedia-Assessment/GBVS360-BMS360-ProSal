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



#include <iostream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <BMS.h>
#include <BMS360.h>
#include "../libgbvs360/ShiftImage.hpp"


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// equatorial prior 

void applyEquatorialPrior(cv::Mat& image, bool adaptive = false) {
	// find the latitude where most of the saliency is located
	
	float equatorialLine = 0;
	if(adaptive) {
		float sum = 0;
		for(int i = 0 ; i < image.rows ; ++i) {
			float lat = (.5f - static_cast<float>(i)/image.rows) * 180.f;
			float weight = 0;
			for(int j = 0 ; j < image.cols ; ++j) {
				weight += image.at<float>(i,j);
				sum += image.at<float>(i,j);;
			}
			equatorialLine += lat * weight;
		}
		equatorialLine /= sum;	

		equatorialLine = std::max(std::min(equatorialLine, 8.f), -8.f);
	}

	// apply the equatorial prior 
	for(int i = 0 ; i < image.rows ; ++i) {
		float lat = (.5f - static_cast<float>(i)/image.rows) * 180.f - equatorialLine;
		float prior =  (.01 + .40 * exp(-(lat*lat)/(700)) + .75 * exp(-((lat-100)*(lat-100))/(200)) + .75 * exp(-((lat+90)*(lat+90))/(200))) / .41f;
		prior = std::min(1.f, prior);

		for(int j = 0 ; j < image.cols ; ++j) {
			image.at<float>(i,j) = prior * image.at<float>(i,j);
		}
	}
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// apply BMS on one frame

void process(const cv::Mat &input, cv::Mat &output, int maxDim, int dilatationWidth1, int dilatationWidth2, int normalize, int handleBorder, int colorSpace, bool whitening, int sampleStep, float blurStd, bool equirectangular) {

	cv::Mat src_small;
	float w = (float)input.cols, h = (float)input.rows;
	float maxD = fmax(w, h);
	cv::resize(input, src_small, cv::Size((int)(maxDim*w / maxD), (int)(maxDim*h / maxD)), 0.0, 0.0, cv::INTER_AREA);

	boost::shared_ptr<BMS> bms;
	if(equirectangular)
		bms = boost::shared_ptr<BMS>(new BMS360(src_small, dilatationWidth1, normalize == 1, handleBorder == 1, colorSpace, whitening));
	else
		bms = boost::shared_ptr<BMS>(new BMS(src_small, dilatationWidth1, normalize == 1, handleBorder == 1, colorSpace, whitening));

	bms->computeSaliency((double)sampleStep);

	cv::Mat result = bms->getSaliencyMap(false);

	if (dilatationWidth2 > 0)
		dilate(result, result, cv::Mat(), cv::Point(-1, -1), dilatationWidth2);

	// if (blurStd > 0) {
	// 	int blur_width = (int)MIN(floor(blurStd) * 4 + 1, 51);
	// 	cv::GaussianBlur(result, result, cv::Size(blur_width, blur_width), blurStd, blurStd);
	// }

	output = result;
	// cv::resize(result, output, input.size());
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// work on one projection


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
	bool equirectangular;
}; 



void processJob(int workerID, int nb_shift, cv::Mat &input, std::vector<cv::Mat> &outputs, Configuration &conf) {
	cv::Mat inputImage = shiftImage<unsigned char>(input, workerID * input.cols / nb_shift, 0);

	process(inputImage, outputs[workerID], conf.maxDim, conf.dilatationWidth1, conf.dilatationWidth2, conf.normalize, conf.handleBorder, conf.colorSpace, conf.whitening, conf.sampleStep, conf.blurStd, conf.equirectangular);
}







// ------------------------------------------------------------------------------------------------------------------------------------------------------


int main(int argc, char **argv) {


	// ------------------------------------------------------------------------------------------------------------------------------------------
	// parse parameters

	namespace po = boost::program_options;

	po::positional_options_description p;
	p.add("input-file", -1);

	po::options_description desc("Allowed options");
	desc.add_options()
			("help", "produce help message")
			("input-file,i", po::value< std::string >(), "Input image to process.")
			("output-file,o", po::value< std::string >(), "Output image.")
	;

	po::options_description sl("Per-projection saliency estimation options");
	sl.add_options()
			("step-size", po::value< int >(), "Steps in the generation of the boolean maps. [default]: 8")
			("max-dim", po::value< int >(), "Maximum dimension of the image used for estimating Saliency. [default]: 400 ")
			("dilatation-width-1", po::value< int >(), "Size of the dilatation kernel performed before L2 normalization: [default]: max(round(7*max_dim/400),1); ")
			("dilatation-width-2", po::value< int >(), "Size of the dilatation kernel performed before the Gaussian blurring kernel: [default]: round(9*max_dim/400); sigma.")
			("blur-std", po::value< float >(), "Standard deviation final overall Gaussian blur processing. ")
			("normalize", po::value< int >(), "Normalize the saliency map. ")
			("handle-border", po::value< int >(), "Handle the border of the images (convolution issues). ")
			("color-space", po::value< int >(), "1) RGB, 2) Lab, 4) Luv [default]: 2")
			("whitening", po::value< int >(), "Do color whitening. [default]: 1")
			("equatorial-prior", "Apply an equatorial-prior to the images. This was designed for equirectangular images. It may not be a good idea for rectilinear images.")
			("apply-fms", po::value< int >(), "Compute the FMS model: apply the saliency algorithm on several shifted images. The provided parameter is the number of projection (4 recommended)")
			("equirectangular-mode", "Consider an equirectangular image as input")
	;


	desc.add(sl);

	po::variables_map vm;
	try {
		po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
		po::notify(vm);
	} catch(boost::exception &) {
		std::cerr << "Error incorect program options. See --help... \n";
		return 0;
	}

	if (vm.count("help")) {
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "\t\tTest saliency estimation on 2D images\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "\n\n";
		std::cout << desc << "\n";
		return 1;
	}

	
	std::string inputPath;
	std::string outputPath;

	int sampleStep = 8;
	int dilatationWidth1 = 3;
	int dilatationWidth2 = 11;
	float blurStd = 20;
	int normalize = 1;
	int handleBorder = 0;
	int colorSpace = CL_Lab;
	bool whitening = true;
	int maxDim = 400;
	bool equatorialPrior = false;
	
	if (vm.count("input-file")) {
		inputPath = vm["input-file"].as< std::string >();
	} else {
		std::cerr << "It is required to provide the an input image. See --help\n";
		return 0;
	}

	if (vm.count("output-file")) {
		outputPath = vm["output-file"].as< std::string >();
	} 


	// ---------------------------------------------------------------------------------------------------
	// Saliency BMS settings



	if (vm.count("step-size")) {
		sampleStep = vm["step-size"].as< int >();
	} 


	if (vm.count("max-dim")) {
		maxDim = vm["max-dim"].as< int >();
		dilatationWidth1 = static_cast<int>(fmax(round(7*maxDim/400.f),1.f));
		dilatationWidth2 = static_cast<int>(fmax(round(9*maxDim/400.f),1.f));
		blurStd = round(9*maxDim/400.f);
	} 

	if (vm.count("dilatation-width-1")) {
		dilatationWidth1 = vm["dilatation-width-1"].as< int >();
	} 

	if (vm.count("dilatation-width-2")) {
		dilatationWidth2 = vm["dilatation-width-2"].as< int >();
	} 

	if (vm.count("blur-std")) {
		blurStd = vm["blur-std"].as< float >();
	} 

	if (vm.count("normalize")) {
		normalize = vm["normalize"].as< int >() == 1;
	} 

	if (vm.count("handle-border")) {
		handleBorder = vm["handle-border"].as< int >() == 1;
	} 

	if (vm.count("color-space")) {
		colorSpace = vm["color-space"].as< int >();
	} 

	if (vm.count("whitening")) {
		whitening = vm["whitening"].as< int >() == 1;
	} 

	if(vm.count("equatorial-prior")) {
		equatorialPrior = true;
	}

	int nb_projections = 1;
	if(vm.count("apply-fms")) {
		nb_projections = vm["apply-fms"].as< int >();
	}

	bool equirectangularMode = false;
	if(vm.count("equirectangular-mode")){
		equirectangularMode = true;
	}


	// ---------------------------------------------------------------------------------------------------
	// start program


	cv::Mat inputImage = cv::imread(inputPath);
	cv::Mat sMap;


    if(inputImage.empty()) {
        std::cerr << "cannot open: "<< inputPath << std::endl;
        return 0;
    }

    Configuration conf;

    conf.sampleStep = sampleStep;
	conf.dilatationWidth1 = dilatationWidth1;
	conf.dilatationWidth2 = dilatationWidth2;
	conf.blurStd = blurStd;
	conf.normalize = normalize;
	conf.handleBorder = handleBorder;
	conf.colorSpace = colorSpace;
	conf.whitening = whitening;
	conf.maxDim = maxDim;
	conf.equatorialPrior = equatorialPrior;
	conf.equirectangular = equirectangularMode;


    std::vector<cv::Mat> outputs(nb_projections);
	if(nb_projections > 1) {
		boost::thread_group g;
		for(int i = 0 ; i < nb_projections ; ++i) {
			g.create_thread(boost::bind(processJob, i, nb_projections, boost::ref(inputImage), boost::ref(outputs), boost::ref(conf)));
		}
		g.join_all();
	} else {
		processJob(0, nb_projections, inputImage, outputs, conf);
	}


    sMap = outputs[0];
    for(int i = 1 ; i < nb_projections ; ++i) {
    	sMap = sMap + shiftImage<float>(outputs[i], -i * outputs[i].cols / nb_projections, 0);
    }

	if(normalize) {
		if (blurStd > 0) {
			int blur_width = (int)MIN(floor(blurStd) * 4 + 1, 51);
			cv::GaussianBlur(sMap, sMap, cv::Size(blur_width, blur_width), blurStd, blurStd);
		}

		if(nb_projections > 0) {
			double mn, mx;
			cv::minMaxLoc(sMap, &mn, &mx);
			sMap = ((sMap-mn) / (mx - mn)) ;
		}
	}

	cv::resize(sMap, sMap, inputImage.size());

	if(equatorialPrior)
		applyEquatorialPrior(sMap);

	if(!outputPath.empty()) {
		cv::Mat tmp;
		cv::cvtColor(sMap, tmp, CV_GRAY2BGR);
		tmp *= 255;
		tmp.convertTo(tmp, CV_8UC3);
		cv::imwrite(outputPath, tmp);
		
	} else {
		imshow("saliency", sMap);
		cv::waitKey();
	}


	return 0;
}

