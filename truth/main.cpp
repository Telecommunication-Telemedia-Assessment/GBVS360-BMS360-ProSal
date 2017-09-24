#include <iostream>
#include <string>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "SaliencyBuilder.h"

int main(int argc, char **argv) {


	// ------------------------------------------------------------------------------------------------------------------------------------------
	// parse parameters

	namespace po = boost::program_options;

	po::positional_options_description p;
	p.add("input-dir", -1);


	po::options_description desc("General options");
	desc.add_options()
		("help", "produce help message")
		("input-dir,i", po::value< std::string >(), "Directory which contrains all the ground truth data")	
		("width,w", po::value< int >(), "Width of the saliency maps. (default: 1280)")	
		("height,h", po::value< int >(), "Height of the saliency maps. (default: 720)")	
		("framerate,r", po::value< int >(), "Set the framerate of the videos. (default: 30)")	
		("fov,f", po::value< float >(), "Set size of the FOV (in degree, default: 110).")
		("threads,t", po::value< int >(), "Set the number of threads used by the program. (default: 12).")
		("fourcc", po::value< std::string >(), "A string of 4 characters which describes the output video codec. (default: X264).")
		("show", "show in a window the computed saliency map")
		("nb-frames", po::value< int >(), "Process only a limited number of frames. (default: -1).")
		("merge-hmds", "Do not make a distinction between from which HMD the data comes from (rift, vive, ...)")
		("filter-hmd", po::value< std::string >(), "Only consider the given HMD")
		("video-overlay-dir", po::value< std::string >(), "Directory which contrains the RGB videos")	
		("overlay-type", po::value< int >(), "select the type of overlay: 1) red mask, 2) show only salient regions")
		("skip-existing", "Generate only the missing saliency maps")
		("select-video", po::value< std::string >(), "Generate the saliency map only for the requested video.")	
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
		std::cout << "\t\t\t\tSalient-truth\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "\tEstimation of the ground truth saliency maps from fixation data\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "\n";
		std::cout << desc << "\n";
		return 1;
	}


	std::string inputDir;
	if (vm.count("input-dir")) {
		inputDir = vm["input-dir"].as< std::string >();
	} else {
		std::cerr << "It is required to provide the directory which contains the json data. See --help\n";
		return 0;
	}

	int width = 1280;
	int height = 720;
	
	if (vm.count("width")) {
		width = vm["width"].as< int >();
	}

	if (vm.count("height")) {
		height = vm["height"].as< int >();
	}

	SaliencyBuilder builder(height, width, 30, 110.f);

	if (vm.count("fov")) {
		builder.setFOV(vm["fov"].as< float >());
	}

	if (vm.count("fps")) {
		builder.setFramerate(vm["fps"].as< int >());
	}

	if (vm.count("threads")) {
		builder.setThreadCounts(vm["threads"].as< int >());
	}

	if (vm.count("fourcc")) {
		std::string fcc = vm["fourcc"].as< std::string >();

		if(fcc.size() != 4) {
			std::cerr << "[E] The input FourCC format is not a 4 character long chain. Possible inputs are: X264, MJPG, MPEG, ..." << std::endl;
			return 0;
		}

		builder.setFourCC(fcc);
	}

	if(vm.count("show")) {
		builder.setShow(true);
	}

	if(vm.count("merge-hmds")) {
		builder.setMergeHMDs(true);
	}

	if(vm.count("nb-frames")) {
		builder.setNbFrames(vm["nb-frames"].as< int >());
	}

	if(vm.count("filter-hmd")) {
		builder.setFilterHMD(vm["filter-hmd"].as< std::string >());
	}

	if(vm.count("video-overlay-dir")) {
		builder.setVideoOveraly(vm["video-overlay-dir"].as< std::string >());
	}

	if (vm.count("overlay-type")) {
		builder.setVideoOveralyType(vm["overlay-type"].as< int >());
	}

	if(vm.count("skip-existing")) {
		builder.setSkipExisting(true);
	}

	if(vm.count("select-video")) {
		builder.setProcessedVideo(vm["select-video"].as< std::string >());
	}



	// ----------------------------------------------------------------------------------------------------------------
	// list all the JSON files, and add the files to the saliency map builder

	namespace fs = boost::filesystem;
	fs::directory_iterator end_iter;

	fs::path rootDir(inputDir);
	if ( fs::exists(rootDir) && fs::is_directory(rootDir)) {
		for( fs::directory_iterator dir_iter(rootDir) ; dir_iter != end_iter ; ++dir_iter) {
			if (fs::is_regular_file(dir_iter->status()) && boost::filesystem::extension(*dir_iter) == ".json") {

				builder.loadLog(dir_iter->path().string());

			}
		}

	} else {
		std::cerr << "The provided directory does not exist or isn't a directory" << std::endl;
		return 0;
	}

	builder.build();

	return 0;

}


