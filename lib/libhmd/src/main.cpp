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
#include <boost/shared_ptr.hpp>

#include "HMDSim.h"

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
		std::cout << "\t\tHMD Simulator\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "Model the distortion generated by the usage of a HMD (in 2017...)\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "\n\n";
		std::cout << desc << "\n";
		return 1;
	}


    std::string inputPath;
    std::string outputPath;
	if (vm.count("input-file")) {
        inputPath = vm["input-file"].as< std::string >();
	} else {
		std::cerr << "It is required to provide the an input image. See --help\n";
		return 0;
	}

	if (vm.count("output-file")) {
		outputPath = vm["output-file"].as< std::string >();
	} 
    

	// ------------------------------------------------------------------------------------------------------------------------------------------
	// run program
    

    cv::Mat inputImage = cv::imread(inputPath);
    if(inputImage.empty()) {
        std::cerr << "Cannot open: " << inputPath << std::endl;
        return -1;
    }



    cv::Mat result;

    HMDSim simulator;
    simulator.applyFilter(inputImage, result);




	// ------------------------------------------------------------------------------------------------------------------------------------------
	// output
    

    if(inputImage.empty())
        return 0;

    bool enabled = true;

    if(outputPath.empty()) {
        cv::imshow("result", result);
		char c = 'p';
		while(c != 'q' && c != ' ') {
			c = cv::waitKey(30);

			if(c >= '1' && c <= '4') {
				simulator.paramBuilder(static_cast<HMD>(static_cast<int>(c)-static_cast<int>('1')));
				simulator.applyFilter(inputImage, result);
				cv::imshow("result", result);
			}

			if(c == 'o') {
				enabled = !enabled;
				if(enabled) 
					cv::imshow("result", result);
				else
					cv::imshow("result", inputImage);
			}

			if(c == 'e') {
				simulator.switchSD();
				std::cout << "Screendoor effect: " << simulator.isSD() << std::endl;
				simulator.applyFilter(inputImage, result);
				cv::imshow("result", result);
			}
		} 
        
    } else {
        cv::imwrite(outputPath, result*255);
    }

    return 0;
}
