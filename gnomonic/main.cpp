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
#include <string>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <opencv2/highgui.hpp>


#include "Gnomonic.h"

int main(int argc, char **argv) {


	// ------------------------------------------------------------------------------------------------------------------------------------------
	// parse parameters

	namespace po = boost::program_options;

	po::positional_options_description p;
	p.add("input-file", -1);


	po::options_description desc("General options");
	desc.add_options()
		("help", "produce help message")
		("input-file,i", po::value< std::string >(), "Input file to be used by the tool. An image to decompose, or a json configuration file for back projection")	
		("output-dir,o", po::value< std::string >(), "Choose which folder will contains the output")
		("width,w", po::value< int >(), "Width of the projected image. (default: 720)")	
		("height,h", po::value< int >(), "Height of the projected image. (default: 720)")	
		("fov,f", po::value< float >(), "Set size of the FOV (in degree, default: 110).")
		("threads,t", po::value< int >(), "Set the number of threads used by the program while running --decompose. (default: 12).")

		("decompose", "Automatic decomposition of the equirectangular image into several rectilinear images")
		("to-rectilinear", "Perform a single projection from equirectangular to rectilinear domain")
		("to-equirectangular", "Perform a single projection from rectilinear to equirectangular domain")
		("pitch", po::value< int >(), "Value of the pitch for the projection (in degree)")
		("yaw", po::value< int >(), "Value of the yaw for the projection (in degree)")
		("roll", po::value< int >(), "Value of the roll for the projection (in degree)")
		("output-file", po::value< std::string >(), "Choose the output file name (for back projection to equirectangular, or single projections)")
		("average-overlap", "while performing back projection from JSON file, average overalapping images (to be used with saliency map _only_. The channels R and G will be lost in the recomposition.)")
		("equatorial-regions", "Perform the processing --decompose only with a pitch of 0")
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
		std::cout << "\t\t\t\tGnomonic\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "Tool to perform gnomonic projection between equirectangular - rectilinear images\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "\n";
		std::cout << desc << "\n";
		return 1;
	}

	Gnomonic gnomonic;

	std::string inputFile;
	if (vm.count("input-file")) {
		inputFile = vm["input-file"].as< std::string >();
	} else {
		std::cerr << "It is required to provide an imput file. See --help\n";
		return 0;
	}

	if (vm.count("output-dir")) {
		gnomonic.setOutputDir(vm["output-dir"].as< std::string >());
	}
	
	if (vm.count("width")) {
		gnomonic.setNrrWidth(vm["width"].as< int >());
	}

	if (vm.count("height")) {
		gnomonic.setNrrHeight(vm["height"].as< int >());
	}

	if (vm.count("fov")) {
		gnomonic.setFOV(vm["fov"].as< float >());
	}

	if (vm.count("threads")) {
		gnomonic.setNbThreads(vm["threads"].as< int >());
	}

	std::string outputFile;
	if(vm.count("output-file")) {
		outputFile = vm["output-file"].as< std::string >();
	}

	if(vm.count("average-overlap")) {
		gnomonic.setAverageOverlap(true);
	}

	if(vm.count("equatorial-regions")) {
		gnomonic.setEquatorialRegion(true);
	}

	// ----------------------------------------------------------------------------------------------------------------
	// start the program

	if(boost::filesystem::extension(inputFile) == ".json") {
		
		gnomonic.getEquirectangular(inputFile, outputFile);

	} else {
		cv::Mat image = cv::imread(inputFile);
		if(image.empty()) {
			std::cerr << "Cannot open file: " << inputFile << std::endl;
			return 0;
		}

		if(vm.count("to-rectilinear") || vm.count("to-equirectangular")) {
			int pitch = 0;
			int yaw = 0;
			int roll = 0;

			if (vm.count("pitch")) {
				pitch = vm["pitch"].as< int >();
			} else {
				std::cout << "[E] The pitch should be defined, see --help" << std::endl;
				return 0;
			}

			if (vm.count("yaw")) {
				yaw = vm["yaw"].as< int >();
			} else {
				std::cout << "[E] The yaw should be defined, see --help" << std::endl;
				return 0;
			}

			if (vm.count("roll")) {
				roll = vm["roll"].as< int >();
			} 

			cv::Mat outputFrame(gnomonic.getHeight(), gnomonic.getWidth(), CV_8UC3, cv::Scalar(0,0,0));

			if(vm.count("to-rectilinear")) {
				gnomonic.equirectangularToRectilinear(image, outputFrame, yaw, pitch, roll);
			} else {
				gnomonic.rectilinearToEquirectangular(image, outputFrame, yaw, pitch, roll);
			}

			cv::imwrite(outputFile, outputFrame);

		} else {
			gnomonic.getRectilinearFrames(image, inputFile);
		}
		
	}

	return 0;
}


