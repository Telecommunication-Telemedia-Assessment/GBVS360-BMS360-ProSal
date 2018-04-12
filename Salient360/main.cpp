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

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <Options.h>
#include <Projection.h>
#include <BMSSaliency.h>
#include <GBVSSaliency.h>
#include <Saliency360.h>

#define SUBMISSION 1


void ouputScan(std::ostream& os, const cv::Mat &scan) {
	for(int i = 0 ; i < scan.rows ; ++i) {
		for(int j = 0 ; j < scan.cols -1 ; ++j) {
			os << scan.at<float>(i, j) << ", ";
		}
		os << scan.at<float>(i, scan.cols -1) << "\n";
	}
}


int main(int argc, char **argv) {


	// ------------------------------------------------------------------------------------------------------------------------------------------
	// parse parameters

	namespace po = boost::program_options;

	po::positional_options_description p;
	p.add("input-file", -1);

#ifndef SUBMISSION

	po::options_description desc("General options");
	desc.add_options()
		("help", "produce help message")
		("input-file,i", po::value< std::string >(), "Input image to process. /!\\ Very important: only equirectangular images are supported!")
		("output-file,o", po::value< std::string >(), "Output file (image or text depending on the model. See --general-model).")
		("threads,t", po::value< int>(),"Number of threads [default]: 8")
		("general-model", po::value< int >(), "Select the type of result from the model: [1] Head saliency maps. [2] Head/Eye saliency maps (GBVS360). [3] Scan path. [4] Head/Eye saliency maps (Projected saliency). [5] Head/Eye saliency maps (Average between model [2] and [4]). [6] BMS360 mode. default: 2")
		("equatorial-prior", "Add an equatorial prior to the saliency map")
	;

	po::options_description dsp("Visualization of results options");
	dsp.add_options()
		("explore", "Disable saliency estimation and only show the rectilinear projection of the pictures.")
		("reference-saliency", po::value< std::string >(),"Show in parallel the saliency map related to the projection.")
		("studied-saliency", po::value< std::string >(),"Show in parallel the saliency map from one of the models.")

		("scan-path", po::value< std::string >(),"Show the reference scan path.")
		("scan-path-user", po::value< int >(),"Select which user from the scan path will be displayed. default: 0")
	;

	po::options_description sp("Equirectangular/rectilinear projection options");
	sp.add_options()
		("aperture,u", po::value< double>(),"Aperture of the projections. ")
		("rect-width,k", po::value< int>(),"Width of the per-projection rectilinear images, in pixels")
		("rect-height,l", po::value< int>(),"Height of the per-projection rectilinear images, in pixels")
	;


	po::options_description sl("Parameters model type 1");
	sl.add_options()
		("bms-step-size", po::value< int >(), "Steps in the generation of the boolean maps. [default]: 8")
		("bms-max-dim", po::value< float >(), "Maximum dimension of the image used for estimating Saliency. [default]: 400 ")
		("bms-dilatation-width-1", po::value< int >(), "Size of the dilatation kernel performed before L2 normalization: [default]: max(round(7*max_dim/400),1); ")
		("bms-dilatation-width-2", po::value< int >(), "Size of the dilatation kernel performed before the Gaussian blurring kernel: [default]: round(9*max_dim/400); ")
		("bms-blur-std", po::value< float >(), "Standard deviation final overall Gaussian blur processing. ")
		("bms-normalize", po::value< int >(), "Normalize the saliency map. ")
		("bms-handle-border", po::value< int >(), "Handle the border of the images (convolution issues). ")
		("bms-color-space", po::value< int >(), "1) RGB, 2) Lab, 4) Luv [default]: 2")
		("bms-whitening", po::value< int >(), "Apply whitening of the map before processing. [default]: 1")
		("bms-fms", po::value< int >(), "Number of projections performed in the Fusion Map Saliency (FMS) model. [default]: 4")
	;


	po::options_description sl360("Parameters model type 2 & 3. Method GBVS360");
	sl360.add_options()
		("gbvs360-blurfrac", po::value< float >(), "Standard deviation final overall Gaussian blur processing. Set it to 0 to disable it. [default]: 0.02")
		("gbvs360-salmapmaxsize", po::value< int >(), "Maximum size of the feature map after back projection. Don't set it to a too large value. [default]: 60")
		("gbvs360-featureScaling", po::value< float >(), "Control the size of the feature maps in rectilinear domain. map size defined by: rect-width / (max(level)*scaling) . [default]: 1")
		("gbvs360-channels", po::value< std::string >(), "Channels used by GBVS360.  Possible options are D,I,O,R,C,P,B,F. [default]: DIOF")
		("gbvs360-channels-description", "Show a description of the different channels options")
		("gbvs360-hmd-sim", "Enable the simulation mode for HMD devices: images are preprocessed to add screen door effect, color bleeding, ...")
		("gbvs360-distScaling", po::value< float >(), "Multiply the distance with a scaling factor . [default]: 1")
	;


	po::options_description slScan("Parameters model type 3");
	slScan.add_options()
		("precomputed-saliency", "The input image is a pre-computed saliency map")
		("scan-path-nb-rep", po::value< int >(), "Number of reppetition (number of users). [default]: 1")
		("scan-path-nb-point", po::value< int >(), "Number of point in the scan path prediciton (per user). [default]: 40")
		("scan-path-duration", po::value< int >(), "Duration in second of the scan (per user). [default]: 40")
	;


	po::options_description projSal("Parameters model type 2. Method projected saliency");
	projSal.add_options()
		("projected-saliency-model", po::value< int >(), "Choose the saliency model to be employed per rectilinear frame. 1) Boolean Map Approach (BMS), 2) Graph Based Visual Saliency (GBVS). [default]: 1")
		("proj-max-dim", po::value< int >(), "Restrict the size of the back-projected saliency map (in pixels). Use -1 to disable it. Default: 2000. ")
		("gbvs-blurfrac",  po::value< float >(), "Blurring performed on the saliency map. [default]: 0.02 ")
		("gbvs-salmapmaxsize", po::value< int >(), "Maximum size of the feature map after back projection. Don't set it to a too large value. [default]: 42")
		("gbvs-channels", po::value< std::string >(), "Channels used by GBVS")

	;


	desc.add(dsp).add(sp).add(sl).add(sl360).add(projSal).add(slScan);

#else

	po::options_description desc("General options");
	desc.add_options()
		("help", "produce help message")
		("input-file,i", po::value< std::string >(), "Input image to process.")
		("output-file,o", po::value< std::string >(), "Output file. Image or binary file (depending on the extension).")
		("threads,t", po::value< int>(),"Number of threads [default]: 8")
		("general-model", po::value< int >(), "Select the type of result from the model: [1] Head saliency maps. [2] Head/Eye saliency maps. default: 1")
		("legacy-icme-2017", "Use the models submitted at the ICME2017 Grand Challenge: Salient360! Please note that the latest version of the model (without this option) outperform this former version.")
		("target-width", po::value< int>(), "Generate a saliency map with a given width. If not set, same as source")
		("target-height", po::value< int>(), "Generate a saliency map with a given height. If not set, same as source")
	;

#endif



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
		std::cout << "\t\t\t\tsalient\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "\tModel for estimating visual attention in 360 images\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << " Contact for this program:\n";
		std::cout << "\tPierre Lebreton, Zhejiang University, China, lebreton.pier@gmail.com\n";
		std::cout << "\tAlexander Raake, TU Ilmenau, Germany, alexander.raake@tu-ilmenau.de\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "\n";
		std::cout << desc << "\n";
		return 1;
	}

	if (vm.count("gbvs360-channels-description")) {
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "\t\tGBVS360 -- Supported channels\n";
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
	
	Saliency360 saliency360;

	
	if (vm.count("input-file")) {
		Option::inputPath = vm["input-file"].as< std::string >();
	} else {
		std::cerr << "It is required to provide the an input image. See --help\n";
		return 0;
	}

	if (vm.count("output-file")) {
		Option::outputPath = vm["output-file"].as< std::string >();
	} 

	if(vm.count("general-model")) {	
		saliency360.model = vm["general-model"].as< int >();
	}

	if(vm.count("equatorial-prior")) {
		saliency360.equatorialPrior = true;
	}

	if(vm.count("legacy-icme-2017")) {
		saliency360.bms360 = false;
	}

	#ifdef SUBMISSION
		if(saliency360.model == 1)
			saliency360.equatorialPrior = true;
		else
			saliency360.equatorialPrior = false;

		Option::distScaling = 60;

	#else
		saliency360.bms360 = false;
	#endif


	bool explore = false;
	if(vm.count("explore")) {
		explore = true;
	} 

	std::string saliencyRefPath;
	if(vm.count("reference-saliency")) {
		saliencyRefPath = vm["reference-saliency"].as< std::string >();
	}


	std::string saliencyStudiedPath;
	if(vm.count("studied-saliency")) {
		saliencyStudiedPath = vm["studied-saliency"].as< std::string >();
	}


	if(vm.count("scan-path")) {
		Option::scanPath = vm["scan-path"].as< std::string >();
	}

	if(vm.count("proj-max-dim")) {
		saliency360.projMaxDim = vm["proj-max-dim"].as< int >();
	}
	


	// ---------------------------------------------------------------------------------------------------
	// Projection settings


	if (vm.count("aperture")) {
		saliency360.getProjection()->nrApper = vm["aperture"].as< double >();
	} 
	if (vm.count("azimuth")) {
		saliency360.getProjection()->nrAzim = vm["azimuth"].as< double >();
	}
	if (vm.count("elevation")) {
		saliency360.getProjection()->nrElev = vm["elevation"].as< double >();
	}
	if (vm.count("roll")) {
		saliency360.getProjection()->nrRoll = vm["roll"].as< double >();
	}
	if (vm.count("rect-width")) {
		saliency360.getProjection()->nrrWidth = vm["rect-width"].as< int >();
	} 
	if (vm.count("rect-height")) {
		saliency360.getProjection()->nrrHeight = vm["rect-height"].as< int >();
	} 
	if (vm.count("threads")) {
		saliency360.getProjection()->nrThread = vm["threads"].as< int >();
		Option::threads = saliency360.getProjection()->nrThread;
	} else {
		Option::threads = 8;
	}


	// ---------------------------------------------------------------------------------------------------
	// Saliency BMS settings


	if(vm.count("projected-saliency-model")) {
		int model = vm["projected-saliency-model"].as< int >();
		if(model == 1) {
			BMSSaliency *bms = new BMSSaliency(true);
			bms->nb_projections = 1;
			saliency360.setSaliency(boost::shared_ptr<BMSSaliency>(bms));
		} else
			saliency360.setSaliency(boost::shared_ptr<GBVSSaliency>(new GBVSSaliency()));
	}


	if (vm.count("bms-step-size")) {
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency360.getSaliency());
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --bms-step-size have no effect. See --help \n";
		} else {
			m->sampleStep = vm["bms-step-size"].as< int >();
		}
	} 

	if (vm.count("bms-max-dim")) {
		
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency360.getSaliency());
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
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency360.getSaliency());
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --bms-dilatation-width-1 have no effect. See --help \n";
		} else {
			m->dilatationWidth1 = vm["bms-dilatation-width-1"].as< int >();
		}	
	} 

	if (vm.count("bms-dilatation-width-2")) {
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency360.getSaliency());
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --bms-dilatation-width-2 have no effect. See --help \n";
		} else {
			m->dilatationWidth2 = vm["bms-dilatation-width-2"].as< int >();
		}
	} 

	if (vm.count("bms-blur-std")) {
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency360.getSaliency());
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --bms-blur-std have no effect. See --help \n";
		} else {
			m->blurStd = vm["bms-blur-std"].as< float >();
		}
	} 

	if (vm.count("bms-normalize")) {
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency360.getSaliency());
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --bms-normalize have no effect. See --help \n";
		} else {
			m->normalize = vm["bms-normalize"].as< int >() == 1;
		}
	} 

	if (vm.count("bms-handle-border")) {
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency360.getSaliency());
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --bms-normalize have no effect. See --help \n";
		} else {
			m->handleBorder = vm["bms-handle-border"].as< int >() == 1;
		}
	} 

	if (vm.count("bms-color-space")) {
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency360.getSaliency());
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --bms-color-space have no effect. See --help \n";
		} else {
			m->colorSpace = vm["bms-color-space"].as< int >();
		}
	} 

	if (vm.count("bms-whitening")) {
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency360.getSaliency());
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --bms-whitening have no effect. See --help \n";
		} else {
			m->whitening = vm["bms-whitening"].as< int >() == 1;
		}
	} 

	if (vm.count("bms-fms")) {
		boost::shared_ptr<BMSSaliency> m = boost::dynamic_pointer_cast<BMSSaliency>(saliency360.getSaliency());
		if(!m) {
			std::cerr << "[W] The model BMS was not selected. --bms-fms have no effect. See --help \n";
		} else {
			m->nb_projections = vm["bms-fms"].as< int >() == 1;
		}
	} 



	// ---------------------------------------------------------------------------------------------------
	// Saliency GBVS360


	if (vm.count("gbvs360-blurfrac")) {
		saliency360.blurfrac = vm["gbvs360-blurfrac"].as< float >();
	} 

	if (vm.count("gbvs360-salmapmaxsize")) {
		saliency360.salmapmaxsize = vm["gbvs360-salmapmaxsize"].as< int >();
	} 

	if (vm.count("gbvs360-featureScaling")) {
		saliency360.featureScaling = vm["gbvs360-featureScaling"].as< float >();
	} 

	if (vm.count("gbvs360-distScaling")) {
		Option::distScaling = vm["gbvs360-distScaling"].as< float >();
	} 

	if (vm.count("gbvs360-channels")) {
		saliency360.channels = vm["gbvs360-channels"].as< std::string >();
	} 

	if(vm.count("gbvs360-hmd-sim")) {
		saliency360.hmdMode = true;
	}


	if(vm.count("scan-path-nb-point")) {
		Option::numberFixations = vm["scan-path-nb-point"].as< int >();
	}
	
	if(vm.count("scan-path-duration")) {
		Option::experimentLength = vm["scan-path-duration"].as< int >();
	}

	if(vm.count("scan-path-nb-rep")) {
		Option::experimentRepppetition = vm["scan-path-nb-rep"].as< int >();
	}

	if(vm.count("precomputed-saliency")) {
		saliency360.precomputedSaliency = true;
	}


	// ---------------------------------------------------------------------------------------------------
	// Projected saliency / GBVS model

	if (vm.count("gbvs-blurfrac")) {
		boost::shared_ptr<GBVSSaliency> m = boost::dynamic_pointer_cast<GBVSSaliency>(saliency360.getSaliency());
		if(m) {
			m->setBlurFrac(vm["gbvs-blurfrac"].as< float >());
		}
	} 

	if (vm.count("gbvs-salmapmaxsize")) {
		boost::shared_ptr<GBVSSaliency> m = boost::dynamic_pointer_cast<GBVSSaliency>(saliency360.getSaliency());
		if(m) {
			m->setSalmapmaxsize(vm["gbvs-salmapmaxsize"].as< int >());
		}
	} 

	if (vm.count("gbvs-channels")) {
		boost::shared_ptr<GBVSSaliency> m = boost::dynamic_pointer_cast<GBVSSaliency>(saliency360.getSaliency());
		if(m) {
			m->setChanels(vm["gbvs-channels"].as< std::string >());
		}
	} 


	

	// ---------------------------------------------------------------------------------------------------
	// Global model




	if (vm.count("general-model")) {
		saliency360.model = vm["general-model"].as< int >();
	}


	int targetWidth = -1;
	if (vm.count("target-width")) {
		targetWidth = vm["target-width"].as< int >();
	}

	int targetHeight = -1;
	if (vm.count("target-height")) {
		targetHeight = vm["target-height"].as< int >();
	}




	// ---------------------------------------------------------------------------------------------------
	// start program


	bool videoMode = false;

	// check what kind of input is provided: video or image. 
	size_t ext_pos = Option::inputPath.find_last_of(".");
	std::string ext;
	if(ext_pos > 0) {
		ext = Option::inputPath.substr(ext_pos+1);
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	}

	if(ext == "mp4" || ext == "avi" || ext == "mkv" || ext == "mpg") {
		videoMode = true;
	}


	// ---------------------------------------------------------------------------------------------------
	// process videos... 
	

	
	if(videoMode) {
		cv::VideoCapture inputVideo(Option::inputPath);
		if(!inputVideo.isOpened()) {
			std::cerr << "Cannot open: " << Option::inputPath << std::endl;
			return -1;
		}

		cv::VideoWriter outputVideo;
		if(!Option::outputPath.empty()) {
			cv::Size S = cv::Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                  (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

			outputVideo.open(Option::outputPath, CV_FOURCC('X', '2', '6', '4'), inputVideo.get(CV_CAP_PROP_FPS), S, false);

			if(!outputVideo.isOpened()) {
				std::cerr << "Cannot write: " << Option::outputPath << std::endl;
				return -1;
			}
		}

		cv::Mat inputImage;
		cv::Mat saliency;

		inputVideo >> inputImage;
		int count = 0;
		while(!inputImage.empty()) {
			saliency360.estimate(inputImage, saliency);

			if(saliency.empty()) return 0;

			if (targetHeight != -1 && targetWidth != -1) {
				cv::resize(saliency, saliency, cv::Size(targetWidth, targetHeight));
			}

			if(!Option::outputPath.empty()) {
				saliency = saliency * 255;
				saliency.convertTo(saliency, CV_8UC1);
				outputVideo << saliency;
			} else {
				cv::imshow("saliency", saliency);
				cv::waitKey(30);
			}

			inputVideo >> inputImage;

			++count;
			// if(count == 15) return 0;
		}

		return 0;
	}



	// ---------------------------------------------------------------------------------------------------
	// process still images... 


	cv::Mat inputImage = cv::imread(Option::inputPath);
	cv::Mat sMap;
	cv::Mat outImage;

	if(inputImage.empty()) {
		std::cerr << "Cannot open input image: " << Option::inputPath << std::endl;
		return 0;
	}


	if(!explore) {
		
		saliency360.estimate(inputImage, outImage);

		if (targetHeight != -1 && targetWidth != -1) {
			cv::resize(outImage, outImage, cv::Size(targetWidth, targetHeight));
		}

		// do not crash while debug
		if(outImage.empty()) return 0;

		// if the result is a saliency map, show or write the image
		if(saliency360.model != 3) {
			if (!Option::outputPath.empty()) {

				bool isBinary = false;
				if (Option::outputPath.size() > 4) {
					size_t length = Option::outputPath.length();
					if (Option::outputPath[length - 4] == '.' && Option::outputPath[length - 3] == 'b' && Option::outputPath[length - 2] == 'i' && Option::outputPath[length - 1] == 'n') {
						isBinary = true;
					}
				}

				if(!isBinary) {

					cv::Mat tmp;
					cv::cvtColor(outImage, tmp, CV_GRAY2BGR);
					tmp *= 255;
					tmp.convertTo(tmp, CV_8UC3);
					cv::imwrite(Option::outputPath, tmp);
					
				} else {
					FILE *f = NULL;
					f = fopen(Option::outputPath.c_str(), "wb");
					if (f == NULL) return 0;

					// sum of all saliency values should be 1.
					double s = cv::sum( outImage )[0];
					outImage /= s;

					fwrite(outImage.data, sizeof(float), outImage.cols*outImage.rows, f);

					fclose(f);
				}



			} else {
				cv::imshow("Saliency", outImage);
				cv::waitKey();
			}
		} else {
			// if the result is a scan path, show or write the result

			if (!Option::outputPath.empty()) {

				bool isBinary = false;
				if (Option::outputPath.size() > 4) {
					size_t length = Option::outputPath.length();
					if (Option::outputPath[length - 4] == '.' && Option::outputPath[length - 3] == 'b' && Option::outputPath[length - 2] == 'i' && Option::outputPath[length - 1] == 'n') {
						isBinary = true;
					}
				}

				if (!isBinary) {
					std::ofstream ofs(Option::outputPath.c_str());
					ouputScan(ofs, outImage);
				}
				else {
					FILE *f = NULL;
					f = fopen(Option::outputPath.c_str(), "wb");
					if (f == NULL) return 0;

					fwrite(outImage.data, sizeof(float), outImage.cols*outImage.rows, f);

					fclose(f);
				}

			} else {
				ouputScan(std::cout, outImage);
			}
		}
		

		return 0;
	}





	// ---------------------------------------------------------------------------------------------------
	// explore the content of the picture... 
	


	cv::Mat nroImage(saliency360.getProjection()->nrrHeight, saliency360.getProjection()->nrrWidth, CV_8UC3, inputImage.channels());
	if(nroImage.empty()) {
		std::cerr << "main::bad alloc... " << std::endl;
		return -1;
	}

	cv::Mat refSaliencyImage;
	cv::Mat nroImageSRef;
	if(!saliencyRefPath.empty()) {
		refSaliencyImage = cv::imread(saliencyRefPath);
	}

	cv::Mat studiedSaliencyImage;
	cv::Mat nroImageSStud;
	if(!saliencyStudiedPath.empty()) {
		studiedSaliencyImage = cv::imread(saliencyStudiedPath);
	}

	saliency360.getProjection()->equirectangularToRectilinear(inputImage, nroImage);
	
	if(!saliencyRefPath.empty() && !refSaliencyImage.empty()) {
		nroImageSRef = cv::Mat(saliency360.getProjection()->nrrHeight, saliency360.getProjection()->nrrWidth, CV_8UC3, inputImage.channels());
		saliency360.getProjection()->equirectangularToRectilinear(refSaliencyImage, nroImageSRef);
	}

	if(!saliencyStudiedPath.empty() && !studiedSaliencyImage.empty()) {
		nroImageSStud = cv::Mat(saliency360.getProjection()->nrrHeight, saliency360.getProjection()->nrrWidth, CV_8UC3, inputImage.channels());
		saliency360.getProjection()->equirectangularToRectilinear(studiedSaliencyImage, nroImageSStud);
	}

	//cv::imshow("inputImage", inputImage);
	
	std::cout << "[I] exploration mode initialized. You can use \"AWSD\" on the keyboard to explore the 360 image. Use \"Q\" to stop the exploration. \n";
	if(refSaliencyImage.empty() && studiedSaliencyImage.empty()) {
		std::cout << "[I] You can also choose to display in parallel a saliency map to check the ground truth or the results of your model. See the section visualization in --help\n";
	}

	char key = ' ';
	while(key != 'q' && key != 27) {
		cv::imshow("output", nroImage);

		if(!nroImageSRef.empty())
			cv::imshow("Ref Saliency", nroImageSRef);

		if(!nroImageSStud.empty())
			cv::imshow("Model Saliency", nroImageSStud);

		key = cv::waitKey(10);

		bool needUpdate = false;
		if(key == 'a') {
			saliency360.getProjection()->nrAzim -= saliency360.getProjection()->nrApper/4;
			needUpdate = true;
		}
		if(key == 'd') {
			saliency360.getProjection()->nrAzim += saliency360.getProjection()->nrApper/4;
			needUpdate = true;
		}
		if(key == 'w') {
			saliency360.getProjection()->nrElev += saliency360.getProjection()->nrApper/4;
			needUpdate = true;
		}
		if(key == 's') {
			saliency360.getProjection()->nrElev -= saliency360.getProjection()->nrApper/4;
			needUpdate = true;
		}

		if(needUpdate) {
			saliency360.getProjection()->equirectangularToRectilinear(inputImage, nroImage);

			if(!refSaliencyImage.empty()) {
				saliency360.getProjection()->equirectangularToRectilinear(refSaliencyImage, nroImageSRef);
			}

			if(!studiedSaliencyImage.empty()) {
				saliency360.getProjection()->equirectangularToRectilinear(studiedSaliencyImage, nroImageSStud);
			}
		}
	}


	return 0;
}


