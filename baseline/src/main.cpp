#include <iostream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <Options.h>
#include <Projection.h>
#include <BMSSaliency.h>
#include <GBVSSaliency.h>
#include <ProjectedSaliency.h>



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
		("threads,t", po::value< int>(),"Number of threads")
		("saliency-model", po::value< int >(), "Choose the saliency model to be employed. 1) Boolean Map Approach (BMS), 2) Graph Based Visual Saliency (GBVS). [default]: 2")
		("blur-std", po::value< float >(), "Standard deviation final overall Gaussian blur processing. ")
	;

	po::options_description sp("Equirectangular/rectilinear projection options");
	sp.add_options()
		("aperture,u", po::value< double>(),"Aperture-specific projection")
		("rect-width,k", po::value< int>(),"Rectilinear output image width, in pixels")
		("rect-height,l", po::value< int>(),"Rectilinear output image height, in pixels")
	;



	po::options_description sl("BMS Options");
	sl.add_options()
		("bms-step-size", po::value< int >(), "Steps in the generation of the boolean maps. [default]: 8")
		("bms-max-dim", po::value< float >(), "Maximum dimension of the image used for estimating Saliency. [default]: 400 ")
		("bms-dilatation-width-1", po::value< int >(), "Size of the dilatation kernel performed before L2 normalization: [default]: max(round(7*max_dim/400),1); ")
		("bms-dilatation-width-2", po::value< int >(), "Size of the dilatation kernel performed before the Gaussian blurring kernel: [default]: round(9*max_dim/400); sigma.")
		("bms-normalize", po::value< int >(), "Normalize the saliency map. ")
		("bms-handle-border", po::value< int >(), "Handle the border of the images (convolution issues). ")
		("bms-color-space", po::value< int >(), "1) RGB, 2) Lab, 4) Luv [default]: 2")
		("bms-whitening", po::value< int >(), "Apply whitening of the map before processing. [default]: 1")
	;


	po::options_description sl360("GBVS Options");
	sl360.add_options()
		("gbvs-blurfrac",  po::value< float >(), "Blurring performed on the saliency map. [default]: 0.02 ")
		("gbvs-salmapmaxsize", po::value< int >(), "Maximum size of the feature map after back projection. Don't set it to a too large value. [default]: 42")
		("gbvs-channels", po::value< std::string >(), "Channels used by GBVS")
	;



	desc.add(sp).add(sl).add(sl360);

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
		std::cout << "\t\tSalient\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "Model for estimating visual attention in 360 images\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "\n\n";
		std::cout << desc << "\n";
		return 1;
	}

	
	boost::shared_ptr<Projection> projection(new Projection());
	boost::shared_ptr<Saliency>   saliency(new GBVSSaliency());
	
	ProjectedSaliency saliency360;
	saliency360.setProjection(projection);
	saliency360.setSaliency(saliency);



	if (vm.count("input-file")) {
		Option::inputPath = vm["input-file"].as< std::string >();
	} else {
		if(!(vm.count("method") && vm["method"].as< int >() == 4)) { // do not quit if documentation is requested.... 
			std::cerr << "It is required to provide the an input image. See --help\n";
			return 0;
		}
	}

	if (vm.count("output-file")) {
		Option::outputPath = vm["output-file"].as< std::string >();
	} 

	if (vm.count("blur-std")) {
		saliency360.blurfrac = vm["blur-std"].as< float >();
	} 




	// ---------------------------------------------------------------------------------------------------
	// Projection settings


	if (vm.count("aperture")) {
		projection->nrApper = vm["aperture"].as< double >();
	} else {
		projection->nrApper = 110;
	}
	if (vm.count("rect-width")) {
		projection->nrrWidth = vm["rect-width"].as< int >();
	} else {
		projection->nrrWidth = 960;
	}
	if (vm.count("rect-height")) {
		projection->nrrHeight = vm["rect-height"].as< int >();
	} else {
		projection->nrrHeight = 960;
	}
	if (vm.count("threads")) {
		projection->nrThread = vm["threads"].as< int >();
		Option::threads = projection->nrThread;
	} else {
		Option::threads = 8;
	}


	// ---------------------------------------------------------------------------------------------------
	// Saliency BMS settings

	if(vm.count("saliency-model")) {
		int model = vm["saliency-model"].as< int >();
		if(model == 1) {
			BMSSaliency *bms = new BMSSaliency();
			bms->nb_projections = 1;
			saliency = boost::shared_ptr<Saliency>(bms);
		} else
			saliency = boost::shared_ptr<Saliency>(new GBVSSaliency());

		saliency360.setSaliency(saliency);

	}


	if (vm.count("bms-step-size")) {
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency);
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --bms-step-size have no effect. See --help \n";
		} else {
			m->sampleStep = vm["bms-step-size"].as< int >();
		}
	} 

	if (vm.count("bms-max-dim")) {
		
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency);
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --max-dim have no effect. See --help \n";
		} else {
			m->maxDim = vm["bms-max-dim"].as< float >();
			m->dilatationWidth1 = static_cast<int>(fmax(round(7*m->maxDim/400.f),1.f));
			m->dilatationWidth2 = static_cast<int>(fmax(round(9*m->maxDim/400.f),1.f));
			m->blurStd = round(9*m->maxDim/400);
		}		
	} 

	if (vm.count("bms-dilatation-width-1")) {
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency);
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --bms-dilatation-width-1 have no effect. See --help \n";
		} else {
			m->dilatationWidth1 = vm["bms-dilatation-width-1"].as< int >();
		}	
	} 

	if (vm.count("bms-dilatation-width-2")) {
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency);
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --bms-dilatation-width-2 have no effect. See --help \n";
		} else {
			m->dilatationWidth2 = vm["bms-dilatation-width-2"].as< int >();
		}
	} 

	if (vm.count("bms-normalize")) {
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency);
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --bms-normalize have no effect. See --help \n";
		} else {
			m->normalize = vm["bms-normalize"].as< int >() == 1;
		}
	} 

	if (vm.count("bms-handle-border")) {
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency);
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --bms-normalize have no effect. See --help \n";
		} else {
			m->handleBorder = vm["bms-handle-border"].as< int >() == 1;
		}
	} 

	if (vm.count("bms-color-space")) {
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency);
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --bms-color-space have no effect. See --help \n";
		} else {
			m->colorSpace = vm["bms-color-space"].as< int >();
		}
	} 

	if (vm.count("bms-whitening")) {
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency);
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --bms-whitening have no effect. See --help \n";
		} else {
			m->whitening = vm["bms-whitening"].as< int >() == 1;
		}
	} 



	// ---------------------------------------------------------------------------------------------------
	// Saliency GBVS360


	if (vm.count("gbvs-blurfrac")) {
		boost::shared_ptr<GBVSSaliency> m = boost::dynamic_pointer_cast<GBVSSaliency>(saliency);
		if(!m) {
			m->setBlurFrac(vm["gbvs-blurfrac"].as< float >());
		}
	} 

	if (vm.count("gbvs-salmapmaxsize")) {
		boost::shared_ptr<GBVSSaliency> m = boost::dynamic_pointer_cast<GBVSSaliency>(saliency);
		if(!m) {
			m->setSalmapmaxsize(vm["gbvs-salmapmaxsize"].as< int >());
		}
	} 

	if (vm.count("gbvs-channels")) {
		boost::shared_ptr<GBVSSaliency> m = boost::dynamic_pointer_cast<GBVSSaliency>(saliency);
		if(!m) {
			m->setChanels(vm["gbvs-channels"].as< std::string >());
		}
	} 





	// ---------------------------------------------------------------------------------------------------
	// start program


	cv::Mat inputImage = cv::imread(Option::inputPath);
	cv::Mat sMap;
	cv::Mat outImage;

	if(inputImage.empty()) {
		std::cerr << "Cannot open input image: " << Option::inputPath << std::endl;
		return 0;
	}


	saliency360.estimate(inputImage, outImage);

	// do not crash while debug
	if(outImage.empty()) return 0;


	if(!Option::outputPath.empty())
		cv::imwrite(Option::outputPath, 255*outImage); 
	else {
		cv::imshow("Saliency", outImage);
		cv::waitKey();
	}


	return 0;
}

