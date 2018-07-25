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

#include "GBVS.h"




// ------------------------------------------------------------------------------------------------------------------------------------------------------
// perform a shift the image

template<typename T>
struct ColorTrait{
	static int Value; 
	typedef cv::Point3_<T> TPoint;
};

template<typename T>
int ColorTrait<T>::Value = CV_8UC3;

template<>
struct ColorTrait<unsigned char>{
	static int Value; 
	typedef cv::Point3_<unsigned char> TPoint;
};

int ColorTrait<unsigned char>::Value = CV_8UC3;

template<>
struct ColorTrait<float>{
	static int Value; 
	typedef float TPoint;
};

int ColorTrait<float>::Value = CV_32FC1;



template<typename T>
cv::Mat shiftImage(const cv::Mat &input, int x, int y) {
	if(x == 0 && y == 0) return input;


	cv::Mat shiftedImage(input.size(), ColorTrait<T>::Value);

	for(int i = 0 ; i < input.rows ; ++i) {
		for(int j = 0 ; j < input.cols ; ++j) {
			int xoff = j+x;
			if(xoff >= 0) xoff = xoff % input.cols;
			else xoff = input.cols + xoff;

			int yoff = i+y;
			if(yoff >= 0) yoff = yoff % input.rows;
			else yoff = input.rows + yoff;

			shiftedImage.at< typename ColorTrait<T>::TPoint >(i,j) = input.at< typename ColorTrait<T>::TPoint >(yoff, xoff);
		}
	}

	return shiftedImage;
}



void processJob(int workerID, int nb_shift, const cv::Mat &input, std::vector<cv::Mat> &outputs, std::vector<GBVS> &workers) {
	cv::Mat inputImage = shiftImage<unsigned char>(input, workerID * input.cols / nb_shift, 0);

	workers[workerID].compute(inputImage, outputs[workerID]);

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
		("salmapmaxsize,s", po::value< int >(), "size of output saliency maps (maximum dimension) don't set this too high (e.g., >60).")
		("blurfrac,b", po::value< float >(), "final blur to apply to master saliency map.")
		("channels,c", po::value< std::string >(), "Channels to study. Possible options are D,I,O,R,C,P,B,F. Default \"DIO\"")
		("levels,l", po::value< std::vector<int> >(), "Levels for the multi-resolution feature pyramid.")
		("disable-central-prior", "Change the cyclic type in the GBVS model. It makes width+1 == 0, and then remove the central assumption. In most cases, it is not a good idea.")
		("equatorial-prior", "Apply an equatorial-prior to the images. This was designed for equirectangular images. It may not be a good idea for rectilinear images.")
		("apply-fms", po::value< int >(), "Compute the FMS model: apply the saliency algorithm on several shifted images. The provided parameter is the number of projection (4 recommended)")
		("disable-csf", "Disable the contrast sensitivity function. This was not part of the orginal GBVS model, and may cause a crash if you don't have enough memory to allocate enough _ALIGNED_ memory required by the Fourier transform.")
		("channels-description", "Show a description of the different channels options")
	;

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
		std::cout << "\t\t\t\tGBVS\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "This is an open-source implementation of the Graph-Based Visual Saliency model (GBVS)\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "\n\n";
		std::cout << desc << "\n";
		return 1;
	}

	if (vm.count("channels-description")) {
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "\t\tGBVS -- Supported channels\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "D: is for DKL Color (Derrington Krauskopf Lennie) feature\n";
		std::cout << "I: is for Intensity\n";
		std::cout << "O: is for Orientation\n";
		std::cout << "R: is for contrast\n";
		std::cout << "C: is for Color (worse than \'D\', you should better consider \'D\')\n";
		std::cout << "P: is the linear perspective: find vanishing points\n";
		std::cout << "B: is blur from defocus\n";
		std::cout << "F: is facial feature detection (Faces and Eyes)\n";
		return 1;
	}

	std::string inputPath;
	std::string outputPath;
	GBVS gbvs;


	if (vm.count("input-file")) {
		inputPath = vm["input-file"].as< std::string >();
	} else {
		std::cerr << "It is required to provide the an input image. See --help\n";
		return 0;
	}

	if (vm.count("output-file")) {
		outputPath = vm["output-file"].as< std::string >();
	} 

	if(vm.count("salmapmaxsize")) {
		gbvs.salmapmaxsize = vm["salmapmaxsize"].as< int >();
	}

	if(vm.count("blurfrac")) {
		gbvs.blurfrac = vm["blurfrac"].as< float >();
	}

	if(vm.count("channels")) {
		gbvs.channels = vm["channels"].as< std::string >();
	}

	if(vm.count("levels")) {
		gbvs.levels = vm["levels"].as< std::vector<int> >();
	}

	if(vm.count("disable-csf")) {
		gbvs.useCSF = false;
	}

	if(vm.count("disable-central-prior")) {
		gbvs.cyclic_type = 1;
	}

	if(vm.count("equatorial-prior")) {
		gbvs.equatorialPrior = true;
	}

	int nb_projections = 1;
	if(vm.count("apply-fms")) {
		nb_projections = vm["apply-fms"].as< int >();
	}


	cv::Mat inputImage = cv::imread(inputPath);
	cv::Mat saliency;

	if(inputImage.empty()) {
		std::cerr << "cannot open: " << inputPath << std::endl;
		return 0;
	}

	if(nb_projections == 1) {
		gbvs.compute(inputImage, saliency);
	} else {
		std::vector<cv::Mat> outputs(nb_projections);

		// std::vector<GBVS> workers(nb_projections);
		// for(int i = 0 ; i < nb_projections ; ++i) {
		// 	workers[i].salmapmaxsize = gbvs.salmapmaxsize;
		// 	workers[i].blurfrac = gbvs.blurfrac;
		// 	workers[i].channels = gbvs.channels;
		// 	workers[i].levels = gbvs.levels;
		// 	workers[i].useCSF = gbvs.useCSF;
		// 	workers[i].cyclic_type = gbvs.cyclic_type;
		// 	workers[i].equatorialPrior = gbvs.equatorialPrior;
		// }
		
		// boost::thread_group g;
	 //    for(int i = 0 ; i < nb_projections ; ++i) {
	 //    	g.create_thread(boost::bind(processJob, i, nb_projections, boost::ref(inputImage), boost::ref(outputs), boost::ref(workers)));

	 //    }
	 //    g.join_all();

		for(int i = 0 ; i < nb_projections ; ++i) {
			// processJob(i, nb_projections, inputImage, outputs, workers);
			// GBVS lgbvs;
			// lgbvs.salmapmaxsize = 42;
			// lgbvs.equatorialPrior = true;
			cv::Mat input = shiftImage<unsigned char>(inputImage, i * inputImage.cols / nb_projections, 0);
			gbvs.compute(input, outputs[i]);
		}


	    saliency = outputs[0];
	    for(int i = 1 ; i < nb_projections ; ++i) {
	    	saliency = saliency + shiftImage<float>(outputs[i], -i * inputImage.cols / nb_projections, 0);
	    }

	    saliency /= nb_projections;

	}


	if(!outputPath.empty()) {
		cv::Mat tmp;
		cv::cvtColor(saliency, tmp, cv::COLOR_GRAY2BGR);
		tmp *= 255;
		tmp.convertTo(tmp, CV_8UC3);
		cv::imwrite(outputPath, tmp);
	} else {
		cv::imshow("input", inputImage);
		cv::imshow("saliency", saliency);
		cv::waitKey();
	}


	return 0;
}
