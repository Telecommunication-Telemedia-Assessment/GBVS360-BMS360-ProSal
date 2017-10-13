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
			("input-saliency,s", po::value< std::string >(), "Input saliency map to process.")
			("output-file,o", po::value< std::string >(), "Output image.")
			("salmapmaxsize", po::value< int >(), "Maximum size of the feature map after back projection. Don't set it to a too large value. [default]: 42")
			("show", "display the rescaled saliency map")
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
		std::cout << "\t\tConversion of the ground truth to csv for fitting purpose\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "\n\n";
		std::cout << desc << "\n";
		return 1;
	}

	
	std::string inputPath;
	std::string inputSaliencyPath;
	std::string outputPath;

	int salmapmaxsize = 42;
	
	if (vm.count("input-file")) {
		inputPath = vm["input-file"].as< std::string >();
	} else {
		std::cerr << "It is required to provide the an input image. See --help\n";
		return 0;
	}

	if (vm.count("input-saliency")) {
		inputSaliencyPath = vm["input-saliency"].as< std::string >();
	} else {
		std::cerr << "It is required to provide the an input image. See --help\n";
		return 0;
	}

	if (vm.count("output-file")) {
		outputPath = vm["output-file"].as< std::string >();
	} 

	if (vm.count("salmapmaxsize")) {
		salmapmaxsize = vm["salmapmaxsize"].as< int >();
	} 



	// ---------------------------------------------------------------------------------------------------
	// start program


	cv::Mat inputImage = cv::imread(inputPath);

	if(inputImage.empty()) {
		std::cerr << "cannot open: " << inputPath << std::endl;
		return 0;
	}

	cv::Mat sMap(inputImage.size(), CV_32FC1);

	FILE *f = fopen(inputSaliencyPath.c_str(), "rb");
	if(f == NULL) {
		std::cerr << "cannot open: " << inputSaliencyPath << std::endl;
		return 0;
	}

	fread(sMap.data, sizeof(float), inputImage.cols*inputImage.rows, f);
	fclose(f);


	float scale = static_cast<float>(salmapmaxsize) / std::fmax(inputImage.cols,inputImage.rows);

	std::vector<int> salmapmaxsize_v;
	salmapmaxsize_v.push_back(static_cast<int>(round(inputImage.rows * scale)));
	salmapmaxsize_v.push_back(static_cast<int>(round(inputImage.cols  * scale)));


	cv::Mat lowResSMap;
	cv::resize(sMap, lowResSMap, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);

	if (vm.count("show")) {
		cv::imshow("sMap", lowResSMap);
		cv::waitKey();
	}

	if(outputPath.empty()) return 0;


	f = fopen(outputPath.c_str(), "w");
	if (f == NULL) {
		return 0;
	}

	for (int i = 0; i < lowResSMap.rows; ++i) {
		for (int j = 0; j < lowResSMap.cols-1; ++j) {
			fprintf(f, "%20.20lf, ", lowResSMap.at<float>(i, j));
		}
		fprintf(f, "%20.20lf\n", lowResSMap.at<float>(i, lowResSMap.cols - 1));
	}

	fclose(f);


	return 0;
}

