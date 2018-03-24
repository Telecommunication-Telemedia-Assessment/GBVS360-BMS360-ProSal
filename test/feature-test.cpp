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
#include <fstream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <list>

#include <Segm.h>
#include <VPD.h>


void objectFeature(cv::Mat &colorImage, cv::Mat& output) {
	//int msseg(cv::Mat &I, float spatialBW, float rangeBW, unsigned int minArea, cv::Mat &seg);
	cv::Mat labels;

	cv::resize(colorImage, colorImage, colorImage.size()/5);
	
	for(int i = 0 ; i < 8 ; ++i) {
		msseg(colorImage, 3, 5, 200, labels);
	}
		

	cv::imshow("segmentation", colorImage);

	double mx, mn;
	cv::minMaxLoc(labels, &mn, &mx);
	std::cout << "Number of features: "<< mx << std::endl;

	output = colorImage;
	//cv::cvtColor(colorImage, output, CV_BGR2GRAY);
}

void vanishingLineFeatureMapBind(const cv::Mat &image, cv::Mat &output) {
	output = vanishingLineFeatureMap(image);
}

int main(int argc, char **argv) {


	// ------------------------------------------------------------------------------------------------------------------------------------------
	// parse parameters

	namespace po = boost::program_options;

	po::positional_options_description p;
	p.add("input-file", -1);

	po::options_description desc("Allowed options");
	desc.add_options()
			("help", "produce help message")
			("input-file1", po::value< std::string >(), "Saliency input image.")
			("input-file2", po::value< std::string >(), "Saliency input image 2.")
			("output-file,o", po::value< std::string >(), "output.")
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
		std::cout << "\t\tApply the equatorial prior on saliency maps\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "\n\n";
		std::cout << desc << "\n";
		return 1;
	}

	
	std::string inputImagePath1;
	std::string inputImagePath2;
	std::string inputColorImagePath;
	std::string outputPath;

	
	if (vm.count("input-file1")) {
		inputImagePath1 = vm["input-file1"].as< std::string >();
	} else {
		std::cerr << "It is required to provide the an input 1. See --help\n";
		return 0;
	}

	if (vm.count("input-file2")) {
		inputImagePath2 = vm["input-file2"].as< std::string >();
	} else {
		std::cerr << "It is required to provide the an input 2. See --help\n";
		return 0;
	}


	if (vm.count("output-file")) {
		outputPath = vm["output-file"].as< std::string >();
	} 



	// ---------------------------------------------------------------------------------------------------
	// Get statistics

	cv::Mat image1 = cv::imread(inputImagePath1);
	if(image1.empty()) {
		std::cerr << "Cannot open: " << inputImagePath1 << std::endl;
		return 0;
	}

	cv::Mat image2 = cv::imread(inputImagePath2);
	if(image2.empty()) {
		std::cerr << "Cannot open: " << inputImagePath2 << std::endl;
		return 0;
	}

	cv::cvtColor(image1, image1, CV_BGR2GRAY);
	cv::cvtColor(image2, image2, CV_BGR2GRAY);

	image1.convertTo(image1, CV_32FC1);
	image2.convertTo(image2, CV_32FC1);

	image1 /= 255;
	image2 /= 255;


	cv::Mat output = image1.mul(image2);

	double mx, mn;
	cv::minMaxLoc(output, &mn, &mx);
	output = (output - mn) / (mx - mn);

	// objectFeature(image, output);

	// cv::resize(image, image, cv::Size(1200, static_cast<int>((1000.f/image.cols) * image.rows)));

	// boost::thread_group g;
	// cv::Mat res1, res2, res3;
	// g.create_thread(boost::bind(&vanishingLineFeatureMapBind, boost::ref(image), boost::ref(res1)));
	// g.create_thread(boost::bind(&vanishingLineFeatureMapBind, boost::ref(image), boost::ref(res2)));
	// g.create_thread(boost::bind(&vanishingLineFeatureMapBind, boost::ref(image), boost::ref(res3)));
	// g.join_all();

	// output = (res1 + res2 + res3) / 3;

	// if(output.empty()) return 0;


	if(!outputPath.empty()) {
		cv::Mat tmp;
		cv::cvtColor(output, tmp, CV_GRAY2BGR);
		tmp *= 255;
		tmp.convertTo(tmp, CV_8UC3);
		cv::imwrite(outputPath, tmp);
		
		return 0;
	} else {
		cv::imshow("feature map", output);
		cv::waitKey();
	}


	return 0;
}

