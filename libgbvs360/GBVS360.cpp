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




#include "GBVS360.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <limits>
#include <set>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <stdlib.h>  
#include <time.h>

#include <HMDSim.h>
#include <VPD.h>

#include "Projection.h"
#include "Options.h"
#include "CSVReader.h"

#define STUDY_FIX_PREDICTION 1

GBVS360::GBVS360(boost::shared_ptr<Projection> projection) {
	m_Projection = projection;

	blurfrac		= 0.02f;
	salmapmaxsize 	= 60;
	featureScaling	= 1.0f;
	hmdMode = false;
}


void GBVS360::compute(const cv::Mat& input, cv::Mat &output, bool normalize) {

	assert(input.channels() == 3 && input.type() == CV_8UC3);

	nbThreads = std::max<int>(2, static_cast<int>(Option::threads / 3.f));

	// reset the framework.... 
	m_ProjectedFrames.clear();
	featureDone.clear();
	m_GBVSWorkers.clear();
	clear(); 					// reset GBVS features and keep init done. 

	m_InputImage = input;
	
	// Let's start the analysis
	m_EquirectangularFrameSize = input.size();
	

	std::cout << "[I] Init graph framework -- scheduled" << std::endl;
	boost::thread_group group;
	group.create_thread(boost::bind(&GBVS360::initGBVS, this, boost::ref(input)));

	std::cout << "[I] Getting rectilinear frames" << std::endl;
	getRectilinearFrames(input);

	// get feature maps for each equililnear frame
	std::cout << "[I] Getting features per frame" << std::endl;
	getRectilinearFeatures();

	// back project feature maps to equirectangular coordinate
	std::cout << "[I] Getting back-projected features" << std::endl;
	getEquirectangularFeatures();


	std::cout << "[I] Waiting graph initialization to finish" << std::endl;
	group.join_all();

	
	std::cout << "[I] Applying GBVS Activation, Normalization & Pooling" << std::endl;

	// GBVS regular flow
	gbvsActivation(input, output, normalize);


	if(equatorialPrior) {
		applyEquatorialPrior(output, input);

		if(normalize) {
			double mn, mx;
			cv::minMaxLoc(output, &mn, &mx);
			output = (output - mn) / (mx - mn);
		}
		
	}
	

	if (Option::exportRawFeatures) {
		for (std::list<FeatureMap>::const_iterator it = allmaps.begin(); it != allmaps.end(); ++it) {
			FILE *f = fopen((Option::prefix + "_" + boost::lexical_cast<std::string>(it->channel) + "_" + boost::lexical_cast<std::string>(it->level) + "_" + boost::lexical_cast<std::string>(it->type) + ".csv").c_str(), "w");
			if (f == NULL) {
				continue;
			}

			for (int i = 0; i < it->map.rows; ++i) {
				for (int j = 0; j < it->map.cols-1; ++j) {
					fprintf(f, "%20.20lf, ", it->map.at<double>(i, j));
				}
				fprintf(f, "%20.20lf\n", it->map.at<double>(i, it->map.cols - 1));
			}

			fclose(f);
		}
	}

}





struct FixationOptionComparison {
	bool operator()(const FixationOption &a, const FixationOption &b) const {
		if(std::abs(a.value - b.value) < 0.000000001f) {
			if (std::abs(a.probability - b.probability) < 0.000000001f) {
				if(a.x == b.x) {
					return a.y < b.y;
				} else {
					return a.x < b.x;
				}
			} else {
				return (a.probability < b.probability);
			}
		}
		return a.value < b.value;
	}
};


struct FixationOptionComparison2 {
	bool operator()(const FixationOption &a, const FixationOption &b) const {
		if (std::abs(a.probability - b.probability) < 0.000000001f) {
			if(std::abs(a.value - b.value) < 0.000000001f) {
				if(a.x == b.x) {
					return a.y < b.y;
				} else {
					return a.x < b.x;
				}
			} else {
				return a.value < b.value;
			}
		}
		return (a.probability < b.probability);
		
	}
};


typedef std::set<FixationOption, FixationOptionComparison2> TFixationsSet;


void GBVS360::scanPath(const cv::Mat& imgBGR, cv::Mat &out, bool inputSaliencyMap) {

	// -------------------------------------------------------------------------------------------
	// First, we need the saliency map


	// if the input provided is not already a saliency map, then compute it using the GBVS360 framework
	if(!inputSaliencyMap) {
		cv::Mat saliency;
		compute(imgBGR, saliency);
	}


	// Check if the GBVS360 framework was initialized (it is the case if it computed the saliency map, it was provided as input the initialization is required)
	if(!initDone)
		initGBVS(imgBGR);


	CSVReader<float> csv;
	if(!Option::scanPath.empty()) {
		if(!csv.open(Option::scanPath)) {
			std::cout << "[I] cannot open csv file: " << Option::scanPath << std::endl;
		}
	}

	std::vector<FixationOption> groundTruthFixations;
	if(csv.getCols() > 0) {
		int obsIdx = -1;
		for(size_t i = 0 ; i < csv[0].size() ; ++i) {
			if(csv[0][i] == 1) {
				++obsIdx;
			}

			if(obsIdx == Option::scanPathUserIdx) {
				FixationOption f;
				f.x = static_cast<int>(csv[2][i]);
				f.y = static_cast<int>(csv[3][i]);
				f.value = csv[0][i];
				f.probability = csv[1][i];

				groundTruthFixations.push_back(f);
			}
		}
	}


	// Check the input: if a saliency map was provided, it can be in BGR or gayscale. 
	cv::Mat saliency;
	if(inputSaliencyMap) {
		saliency = imgBGR.clone();
		if(imgBGR.channels() == 3 && imgBGR.type() == CV_8UC3) {
			cv::cvtColor(saliency, saliency, CV_BGR2GRAY);
			saliency.convertTo(saliency, CV_64FC1);
			saliency /= 255;
		} else {
			saliency.convertTo(saliency, CV_64FC1);
		}
		

		cv::resize(saliency, master_map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);
	}

#ifdef STUDY_FIX_PREDICTION

	cv::resize(saliency, saliency, saliency.size()/5, 0, 0, cv::INTER_AREA);

#endif

	// -------------------------------------------------------------------------------------------
	// prepare the state transition matrix

	const cv::Mat &lx = grframe.lx;

	// form a multiresolution pyramid of feature maps according to multilevels
	std::vector<cv::Mat> apyr;
	std::vector< std::pair<int, int> > dims;
	formMapPyramid(master_map, grframe.multilevels, apyr, dims);

	// assign a linear index to each node
	std::vector<double> AL;
	arrangeLinear(apyr , dims, AL);

	// create the state transition matrix between nodes
	cv::Mat mm(lx.rows, lx.rows, CV_64FC1, cvScalar(0.f)); 

	// assign edge weights based on distances between nodes and algtype
	
	for(int c = 0 ; c < static_cast<int>(AL.size()) ; ++c) {
		for(int r = 0 ; r < static_cast<int>(AL.size()) ; ++r) {
			mm.at<double>(r,c) =  std::abs( AL[r] - AL[c] );
		}
	}

	// make it a markov matrix (so each column sums to 1)
	columnNormalize(mm);

	// inverse the weight, as we want to convert normalized distance to probability of transition
	// two elements with a distance of 0 should have a high chance to be connected 
	mm = 1 - mm;

	
	// -------------------------------------------------------------------------------------------
	// Use the state transition matrix to predict scan path
	
	srand(time(NULL));

#ifdef STUDY_FIX_PREDICTION

	saliency.convertTo(saliency, CV_32FC3);

#endif
	
	out = cv::Mat(Option::numberFixations * Option::experimentRepppetition, 4, CV_32FC1, cvScalar(0.f));
	for(int i = 0 ; i < Option::experimentRepppetition ; ++i) {
		int initPosition = static_cast<int>((i * static_cast<float>(master_map.cols) / static_cast<float>(Option::experimentRepppetition)) * master_map.rows + master_map.rows / 2);
		cv::Mat result = runScanPath(saliency, imgBGR, groundTruthFixations, lx, mm, initPosition);

		for(int ii = 0 ; ii < result.rows ; ++ii) {
			for(int jj = 0 ; jj < result.cols ; ++jj) {
				out.at<float>(i*Option::numberFixations + ii, jj) = result.at<float>(ii,jj);
			}
		}
	}

}




void GBVS360::getTransitionMatrix(const cv::Mat &lx, const cv::Mat &mm, cv::Mat &distanceMatrix, int position, float scaling) const {
	double sig = sigma_frac_act * static_cast<double>(master_map.cols + master_map.rows)/2.f;

	for(int l = 0 ; l < lx.rows ; ++l) {
		distanceMatrix.at<float>(l % master_map.rows, l / master_map.rows) = static_cast<float>(mm.at<double>(l, position)); //
	}

	double mx, mn;
	cv::minMaxLoc(distanceMatrix, &mn, &mx);

	distanceMatrix = (distanceMatrix - mn) / (mx - mn);

	for(int l = 0 ; l < lx.rows ; ++l) {

		float distX = static_cast<float>(std::min( std::abs((l / master_map.rows)  -  (position / master_map.rows)) , std::abs( std::abs((l / master_map.rows)  -  (position / master_map.rows)) - master_map.cols ) ));
		float distY = static_cast<float>(std::min( std::abs((l % master_map.rows)  -  (position % master_map.rows)) , std::abs( std::abs((l % master_map.rows)  -  (position % master_map.rows)) - master_map.rows ) ));
		float dist = (distX*distX + distY*distY);

		distanceMatrix.at<float>(l % master_map.rows, l / master_map.rows) = static_cast<float>(std::exp( -1 *( dist / scaling ) / (2 * sig*sig)) * distanceMatrix.at<float>(l % master_map.rows, l / master_map.rows)); //
	}

	distanceMatrix = distanceMatrix * (mx - mn) + mn;
}


#ifdef STUDY_FIX_PREDICTION
cv::Mat GBVS360::runScanPath(const cv::Mat& saliency, const cv::Mat& imgBGR, const std::vector<FixationOption>& groundTruthFixations, const cv::Mat &lx, const cv::Mat &mm, int initPosition) {
#else
cv::Mat GBVS360::runScanPath(const cv::Mat& , const cv::Mat& imgBGR, const std::vector<FixationOption>& , const cv::Mat &lx, const cv::Mat &mm, int initPosition) {
#endif

	cv::Mat out = cv::Mat(Option::numberFixations, 4, CV_32FC1, cvScalar(0.f));

	// start from the center
	int position = initPosition;


#ifdef STUDY_FIX_PREDICTION

	float scaling = static_cast<float>(saliency.cols / master_map.cols);


	cv::Mat saliency2 = saliency.clone();

#endif

	// if start from a non salient area, find one.
	if(master_map.at<double>(position % master_map.rows, position / master_map.rows) < 0.7) {
		float localMax = 0;
		int pos_x = position % master_map.cols;

		for(int ii = master_map.rows/3 ; ii < master_map.rows - master_map.rows/3 ; ++ii) {
			for(int jj = std::max(0, pos_x-master_map.cols/6) ; jj < std::min(master_map.cols/6, pos_x+master_map.cols/6) ; ++jj) {
				
				if(master_map.at<double>(ii,jj) > localMax) {
					position = jj * master_map.rows + ii;
					localMax = static_cast<float>(master_map.at<double>(ii,jj));
				}
			}

			if((pos_x-master_map.cols/6) < 0) {
				for(int jj = master_map.cols + (pos_x-master_map.cols/6) ; jj < master_map.cols ; ++jj) {
				
					if(master_map.at<double>(ii,jj) > localMax) {
						position = jj * master_map.rows + ii;
						localMax = static_cast<float>(master_map.at<double>(ii,jj));
					}
				}
			}

			if((pos_x+master_map.cols/6) > master_map.cols) {
				for(int jj = 0 ; jj < (pos_x+master_map.cols/6) - master_map.cols ; ++jj) {
				
					if(master_map.at<double>(ii,jj) > localMax) {
						position = jj * master_map.rows + ii;
						localMax = static_cast<float>(master_map.at<double>(ii,jj));
					}
				}
			}


		}
	}
		
	// find local maxiam
	{
		int lx = position / master_map.rows;
		int ly = position % master_map.rows;

		int plx = -1;
		int ply = -1;
		while(plx != lx && ply != ly) {
			plx = lx; ply = ly;
			if((lx + 1 < master_map.cols) && master_map.at<double>(ly, lx+1) > master_map.at<double>(ly, lx)) {
				++lx;
			}
			if((lx - 1 > 0) && master_map.at<double>(ly, lx-1) > master_map.at<double>(ly, lx)) {
				--lx;
			}
			if((ly + 1 < master_map.rows) && master_map.at<double>(ly+1, lx) > master_map.at<double>(ly, lx)) {
				++ly;
			}
			if((ly - 1 > 0) && master_map.at<double>(ly-1, lx) > master_map.at<double>(ly, lx)) {
				--ly;
			}
		}

		position = lx * master_map.rows + ly;
	}




	double smx, smn;
	cv::minMaxLoc(master_map, &smn, &smx);


	std::list<cv::Point2i> fixationHistory;
	fixationHistory.push_back(cv::Point2i(position / master_map.rows, position % master_map.rows));

	cv::Mat distanceMatrix(master_map.size(), CV_32FC1, cv::Scalar(0));
	for(int i = 0 ; i < Option::numberFixations ; ++i) {
		// lookup the transition probability for all other points


		getTransitionMatrix(lx, mm, distanceMatrix, position, 1);

		double mx, mn;
		cv::minMaxLoc(distanceMatrix, &mn, &mx);

		double localThreshold = mn + (mx - mn) * 0.9;

		int nextX = 0;
		int nextY = 0;
		TFixationsSet fixations;
		int nbIter = 0;
		int nbIter2 = 0;
		float salRequired = static_cast<float>(0.7 * smx);
		while(fixations.empty() && nbIter != 15 && nbIter2 < 10) {
			
			for(int ii = 0 ; ii < distanceMatrix.rows ; ++ii) {
				for(int jj = 0 ; jj < distanceMatrix.cols ; ++jj) {
					
					if(distanceMatrix.at<float>(ii,jj) > localThreshold) {

						bool keep = true;

						int nbPrevious = 0;
						for(std::list<cv::Point2i>::reverse_iterator it = fixationHistory.rbegin() ; it != fixationHistory.rend() && keep && nbPrevious < 20; ++it) {
							if(it->x == jj && it->y == ii) {
								keep = false;
							}
							++nbPrevious;
						}

						if(master_map.at<double>(ii,jj) < salRequired) keep = false;
						
						if(keep) {
							FixationOption f;
							f.x 			= jj;
							f.y 			= ii;
							f.value 		= static_cast<float>(master_map.at<double>(ii,jj));
							f.probability 	= distanceMatrix.at<float>(ii,jj);

							fixations.insert(f);
						}
					}
				}
			}

			if(fixations.empty()) {
				if(nbIter == 14) {
					salRequired = static_cast<float>(0.1 * smx); // relax constrain, if we cannot find a suitable position
					++nbIter2; 				 // make sure it will end.
					nbIter = 0;
				} else {
					salRequired = static_cast<float>(0.5 * smx);
				}
				++nbIter;
				getTransitionMatrix(lx, mm, distanceMatrix, position, 10.f * nbIter);
			}
		}
		

		int next_idx = rand() % 4;

		// if no fixation could be found, go to the most salient point nearby... 
		if(fixations.empty()) {
			float localMax = 0;
			for(int ii = 0 ; ii < distanceMatrix.rows ; ++ii) {
				for(int jj = 0 ; jj < distanceMatrix.cols ; ++jj) {
					
					if(distanceMatrix.at<float>(ii,jj) > localMax) {
						if(!fixationHistory.empty() && fixationHistory.back().x == jj && fixationHistory.back().y == ii) continue;
						nextX = jj;
						nextY = ii;
						localMax = distanceMatrix.at<float>(ii,jj);
					}
				}
			}

			FixationOption f;
			f.x 			= nextX;
			f.y 			= nextY;
			f.value 		= static_cast<float>(master_map.at<double>(nextY,nextX));
			f.probability 	= distanceMatrix.at<float>(nextY,nextX);

			fixations.insert(f);
			
		}

		TFixationsSet::reverse_iterator it = fixations.rbegin();
		if(next_idx == 0) {
			nextX = it->x;
			nextY = it->y;
		} else {
			TFixationsSet::reverse_iterator prev_it = it;
			++it;

			for(int ii = 0 ; ii < next_idx && it != fixations.rend() ; ++ii) {
				prev_it = it;
				++it;
			}

			if(it != fixations.rend()) {
				nextX = it->x;
				nextY = it->y;
			} else {
				nextX = prev_it->x;
				nextY = prev_it->y;
			}
		}
		
		fixationHistory.push_back(cv::Point2i(nextX, nextY));

		position = nextX * distanceMatrix.rows + nextY;

#ifdef STUDY_FIX_PREDICTION

		cv::circle(saliency2, cv::Point2i(scaling * nextX, scaling * nextY), 6, cv::Scalar(1,1,0));

		if(i < static_cast<int>(groundTruthFixations.size()))
			cv::circle(saliency2, cv::Point2i(saliency2.cols * groundTruthFixations[i].x / static_cast<float>(imgBGR.cols), saliency2.rows * groundTruthFixations[i].y / static_cast<float>(imgBGR.rows)), 3, cv::Scalar(1,1,0));
		

		cv::imshow("saliency", saliency2);
		cv::waitKey();

		saliency2 = saliency.clone();

#endif

		out.at<float>(i, 2) = static_cast<float>(static_cast<int>(nextX * static_cast<float>(imgBGR.cols) / static_cast<float>(master_map.cols)));
		out.at<float>(i, 3) = static_cast<float>(static_cast<int>(nextY * static_cast<float>(imgBGR.cols) / static_cast<float>(master_map.cols)));
		out.at<float>(i, 1) = fixations.rbegin()->value;
		out.at<float>(i, 0) = static_cast<float>(i);
	}

	float sumProb = 0;
	for(int i = 1 ; i < out.rows ; ++i) {
		sumProb += out.at<float>(i, 1);
	}

	out.at<float>(0, 1) = 0;
	for(int i = 1 ; i < out.rows ; ++i) {
		out.at<float>(i, 1) = out.at<float>(i-1, 1) + Option::experimentLength * out.at<float>(i, 1) / sumProb;
	}

	return out;
}



// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



void GBVS360::getRectilinearFrames(const cv::Mat &inputImage) {
	if(!m_Projection) {
		std::logic_error(std::string("Saliency360::getEquilinarFrames: The projection model was not allocated. Quit"));;
		return;
	}


	if(m_Projection->nreWidth == 0 || m_Projection->nreHeight == 0) {
		m_Projection->nreWidth = inputImage.cols;
		m_Projection->nreHeight = inputImage.rows;
	}


	float scaling_x = 2;
	float scaling_y = 1;


	int nb_projections_w = static_cast<int>(std::ceil(360.f / static_cast<float>(m_Projection->nrApper/scaling_x)));
	int nb_projections_h = static_cast<int>(std::ceil(180.f / static_cast<float>(m_Projection->nrApper/scaling_y)));


	// prepare all the projections: what needs to be done.
	for(int j = -nb_projections_h / 2 ; j <= nb_projections_h/2 ; ++j) { // for(int j = -nb_projections_h / 2 -1 ; j <= nb_projections_h/2 + 1; ++j) {
		for(int i = 0 ; i < nb_projections_w ; ++i) {

			m_ProjectedFrames.push_back(ProjectedFrame());
			ProjectedFrame &frame = m_ProjectedFrames.back();

			frame.rectilinearFrame = cv::Mat(m_Projection->nrrHeight, m_Projection->nrrWidth, CV_8UC3, inputImage.channels());
			if(frame.rectilinearFrame.empty()) {
				throw std::logic_error(std::string("Saliency360::getRectilinearFrames bad alloc..."));
				return ;
			}

			// frame.nrElev = static_cast<int>(j * m_Projection->nrApper/scaling_x);
			// frame.nrAzim = static_cast<int>(i * m_Projection->nrApper/scaling_y);
			frame.nrAzim = static_cast<int>(i * m_Projection->nrApper/scaling_x);
			frame.nrElev = static_cast<int>(j * m_Projection->nrApper/scaling_y);
			frame.taskDone = false;
		}
	}


	// Run `Option::threads` threads to do all the projections.
	boost::thread_group group;
	for(size_t i = 0 ; i < Option::threads ; ++i) {
		group.create_thread(boost::bind(&GBVS360::getRectilinearFramesJob, this, boost::ref(inputImage)));
	}
	group.join_all();


	// reset all task flags, to be ready for the next task
	for(std::list<ProjectedFrame>::iterator it = m_ProjectedFrames.begin() ; it != m_ProjectedFrames.end() ; ++it) {
		it->taskDone = false;
	}

}



void GBVS360::getRectilinearFramesJob(const cv::Mat &inputImage) {

	bool taskFound = true;

	while(taskFound) {
		taskFound = false;
		ProjectedFrame *projectedFrame = NULL;
		m_mutex.lock();
		int taskId = 0;
		for(std::list<ProjectedFrame>::iterator it = m_ProjectedFrames.begin() ; it != m_ProjectedFrames.end() ; ++it) {
			if(!it->taskDone) {
				taskFound = true;
				projectedFrame = &(*it);

				it->taskDone = true;		// marked the currently processed frame as estimated
				break;
			}
			++taskId;
		}

		m_mutex.unlock();


		// if there is still something to do, do the job
		if(taskFound) {
			m_Projection->equirectangularToRectilinear(inputImage, projectedFrame->rectilinearFrame, static_cast<float>(projectedFrame->nrAzim), static_cast<float>(projectedFrame->nrElev));

			if(hmdMode) {
				HMDSim simulator;
				cv::Mat result;
    			simulator.applyFilter(projectedFrame->rectilinearFrame, result);
    			result = 255*result;
    			result.convertTo(projectedFrame->rectilinearFrame, CV_8UC3);
			}
		}
	}
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 


void GBVS360::getRectilinearFeatures	() {
	if(m_ProjectedFrames.empty()) return;

	int maxLevel = 0;
	for(size_t i = 0 ; i < levels.size() ; ++i) {
		if(levels[i] > maxLevel)
			maxLevel = levels[i];
	}

	if(maxLevel == 0) return;

	maxcomputelevel = maxLevel;
	int height = static_cast<int>(m_ProjectedFrames.front().rectilinearFrame.rows / (maxLevel*featureScaling));
	int width = static_cast<int>(m_ProjectedFrames.front().rectilinearFrame.cols  / (maxLevel*featureScaling));


	bool faceDetection = false;
	bool segmentation = false;
	bool linPerspective = false;
	std::string workChannels; // forward the requested channels to the workers. Unfortunately, Haarcascades classifiers does not seems to like multithreading. So, it will be needed to that in the main threads. 
	for(size_t i = 0 ; i < channels.size() ; ++i) {
		if(channels[i] != 'F' &&  channels[i] != 'S'  &&  channels[i] != 'P') 
			workChannels += channels[i];
		else {
			if(channels[i] == 'F')
				faceDetection = true;

			if(channels[i] =='S')
				segmentation = true;

			if(channels[i] == 'P') 
				linPerspective = true;
		}
			
	}

	channels = workChannels + (linPerspective ? "P" : "") + (faceDetection ? "F" : "") + (segmentation ? "S" : "");	// make sure that F, S, P are in the last channel. 

	for(size_t i = m_GBVSWorkers.size() ; i < Option::threads ; ++i) {		// instantiate all the workers;
		m_GBVSWorkers.push_back(boost::shared_ptr<GBVS>(new GBVS()));	
		m_GBVSWorkers.back()->useCSF = false;
		m_GBVSWorkers.back()->initDone = true;		// the workers will not generate saliency maps, no need to initialize the graph
		m_GBVSWorkers.back()->maxcomputelevel = maxLevel; // we need to know what the deepest level. Normally this is estimated by initGBVS()
		m_GBVSWorkers.back()->makeGaborFilters();	// we need the Gabor filter 
		m_GBVSWorkers.back()->salmapmaxsize_v.clear();
		m_GBVSWorkers.back()->salmapmaxsize_v.push_back(height);	
		m_GBVSWorkers.back()->salmapmaxsize_v.push_back(width);
		m_GBVSWorkers.back()->channels = workChannels;	

	}


	boost::thread_group group;
	for(int i = 0 ; i < static_cast<int>(m_GBVSWorkers.size()) ; ++i) {
		group.create_thread(boost::bind(&GBVS360::getRectilinearFeaturesJob, this, i));
	}
	group.join_all();


	// Face detection was requested, we now do it in the main thread.
	if(faceDetection) {
		getFaceDetectionFeature(height, width);
	}

		
}


void GBVS360::getRectilinearFeaturesJob(int workerID) {
	boost::shared_ptr<GBVS> &saliency = m_GBVSWorkers[workerID];


	// look for a frame which need to be processed
	bool taskFound = true;

	while(taskFound) {
		taskFound = false;
		ProjectedFrame *projectedFrame = NULL;
		m_mutex.lock();
		for(std::list<ProjectedFrame>::iterator it = m_ProjectedFrames.begin() ; it != m_ProjectedFrames.end() ; ++it) {
			if(!it->taskDone) {
				taskFound = true;
				projectedFrame = &(*it);

				it->taskDone = true;		// marked the currently processed frame as estimated
				break;
			}
		}

		m_mutex.unlock();


		// if there is still something to do, do the job
		if(taskFound) {
			saliency->computeFeatures(projectedFrame->rectilinearFrame);
			projectedFrame->features = saliency->features;
		} 
	}
}


// Do the face detection in the mean thread as the haarcascades classifiers does not like multi-threading... 
void GBVS360::getFaceDetectionFeature(int height, int width) {

	for(std::list<ProjectedFrame>::iterator it = m_ProjectedFrames.begin() ; it != m_ProjectedFrames.end() ; ++it) {
		it->taskDone = true;		// marked the currently processed frame as estimated

		ProjectedFrame *projectedFrame = &(*it);

		int channelNumber = 0;
		for(size_t i = 0 ; i < channels.size() ; ++i) {
			if(channels[i] == 'F') {
				break;
			}
			++channelNumber;
		}

		Feature f;
		f.description = "Face detector";
		f.weight = faceFeatureWeight;
		f.channel = channelNumber;

		cv::Mat featureMap = faceFeaturesDectection(projectedFrame->rectilinearFrame);

		
		if(!featureMap.empty()) {
			FeatureMap fm;
			fm.type  = 0;
			fm.level = 0;
			fm.channel = static_cast<int>(channels.size())-1;

			cv::resize(featureMap, fm.map, cv::Size(width, height), 0, 0, cv::INTER_AREA);
			f.maps.push_back(fm);

			projectedFrame->features.push_back(f);
		}
	}
}

void GBVS360::getSegmentationFeature(const cv::Mat &inputImage, Feature &segFeature) {
	cv::Mat input = inputImage.clone();
	input.convertTo(input, CV_64FC3);
	input /= 255;
	segFeature = segmentationFeature(input);

	int featureIndex = 0;
	for(size_t i = 0 ; i < channels.size() ; ++i) {
		if(channels[i] == 'S') {
			featureIndex = static_cast<int>(i);
			break;
		}
	}

	segFeature.channel = featureIndex;
	for(std::list<FeatureMap>::iterator it = segFeature.maps.begin() ; it !=  segFeature.maps.end() ; ++it) {
		it->channel = featureIndex;
	}
}

void GBVS360::getLinPerFeature(const cv::Mat &inputImage, Feature &linPerFeature) {
	cv::Mat image;
	cv::resize(inputImage, image, cv::Size(1200, static_cast<int>((1000.f/inputImage.cols) * inputImage.rows)));

	std::vector<double> reliability(4);
    std::vector<cv::Mat> outputs(4);
    for(size_t i = 0 ; i < outputs.size() ; ++i) {
		outputs[i] = vanishingLineFeatureMap(image, &reliability[i]);
    }

    for(size_t i = 1 ; i < outputs.size() ; ++i) {
        outputs[0] += outputs[i];
        reliability[0] += reliability[i];
    }
    outputs[0] /= static_cast<double>(outputs.size());
    reliability[0] /= static_cast<double>(outputs.size());

	FeatureMap fm;
	fm.type  = 0;
	fm.level = 0;
	for(size_t i = 0 ; i < channels.size() ; ++i) {
		if(channels[i] == 'P') {
			fm.channel = static_cast<int>(i);
		}
	}

	if(reliability[0] < 0.0015) {	// 0.0038 if the feature map does not contains much information, drop it.
		fm.map = cv::Mat(cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), CV_64FC1, cv::Scalar(0.f));
	} else {
		cv::resize(outputs[0], fm.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);
	}

	linPerFeature.maps.push_back(fm);
	linPerFeature.description = "Linear perspective";
	if(reliability[0] >= 0.0038)
		linPerFeature.weight = std::min(100.f, std::max(0.f, static_cast<float>(-2.824f + 3.105f * exp(662.373f * reliability[0])))) / 10.f; //30 / 70
	else
		linPerFeature.weight = std::min(100.f, std::max(0.f, static_cast<float>(-3.276f + 2.936f * exp(689.485f * reliability[0])))) / 140.f; //100

	linPerFeature.channel = fm.channel;
}

void GBVS360::getEquirectangularFeatures() {

	if(m_ProjectedFrames.empty()) { std::cout << "[I] No projected frames... " << std::endl; return; } 					// If there are no frames available, stop.
	ProjectedFrame &frame = *m_ProjectedFrames.begin(); 	
	if(frame.features.empty()) { std::cout << "[I] No features" << std::endl; return; };						// If there are no features computed, stop.

	int mxType = 0;
	int mxLevel = 0;
	int mxChannel = 0;
	// allocate enough memory for all features in equirectangular domain.
	for(std::list<Feature>::iterator it = frame.features.begin() ; it != frame.features.end() ; ++it) {
		features.push_back(Feature());
		features.back().weight 		= it->weight;
		features.back().channel 	= it->channel;
		features.back().description = it->description;

		for(std::list< FeatureMap >::iterator mapIt = it->maps.begin() ; mapIt != it->maps.end() ; ++mapIt) {
			features.back().maps.push_back(FeatureMap());
			features.back().maps.back().type 	= mapIt->type;
			features.back().maps.back().level 	= mapIt->level;
			features.back().maps.back().channel = mapIt->channel;

			// the interpolation in the projection function needs an image in with a power of 2.
			int rows = 4 * (static_cast<int>(m_EquirectangularFrameSize.height / (maxcomputelevel*featureScaling)) / 4); // 0
			int cols = 4 * (static_cast<int>(m_EquirectangularFrameSize.width / (maxcomputelevel*featureScaling)) / 4); // 1

			features.back().maps.back().map = cv::Mat(rows, cols, CV_64FC1, cv::Scalar(0));

			mxType = std::max(mxType, mapIt->type);
			mxLevel = std::max(mxLevel, mapIt->level);
			mxChannel = std::max(mxChannel, mapIt->channel);
		}
	}


	// add flags to know if a feature was projected to equirectangular domain.
	// order is : featureDone[CHANNEL][LEVEL][TYPE]
	featureDone.clear();
	++mxChannel; ++mxLevel; ++mxType;
	featureDone.resize(mxChannel);
	for(int i = 0 ; i < mxChannel ; ++i) {
		featureDone[i].resize(mxLevel);
		for(int j = 0 ; j < mxLevel ; ++j) {
			featureDone[i][j].resize(mxType, false);
		}
	}

	// Run `Option::threads` threads to do all the projections.
	boost::thread_group group;
	for(size_t i = 0 ; i < Option::threads ; ++i) {
		group.create_thread(boost::bind(&GBVS360::getEquirectangularFeaturesJob, this));
	}


	// if the feature S is requested, we compute it directly in equirectangular domain.
	bool doSegmentation = false;
	Feature segFeature;
	for(size_t i = 0 ; i < channels.size() && !doSegmentation ; ++i) {
		if(channels[i] == 'S') {
			group.create_thread(boost::bind(&GBVS360::getSegmentationFeature, this, boost::ref(m_InputImage), boost::ref(segFeature)));
			doSegmentation = true;
		}
	}

	// if the feature S is requested, we compute it directly in equirectangular domain.
	bool doPerspective = false;
	Feature linPerFeature;
	for(size_t i = 0 ; i < channels.size() && !doPerspective ; ++i) {
		if(channels[i] == 'P') {
			//getLinPerFeature(m_InputImage, linPerFeature);
			group.create_thread(boost::bind(&GBVS360::getLinPerFeature, this, boost::ref(m_InputImage), boost::ref(linPerFeature)));
			doPerspective = true;
		}
	}


	group.join_all();

	// add the segmentation feature maps which was estimated in parallel.
	if(doSegmentation) {
		features.push_back(segFeature);
	}

	if(doPerspective) {
		double mx, mn;
		cv::minMaxLoc(linPerFeature.maps.back().map, &mn, &mx);
		if(mx > 0.1) {
			features.push_back(linPerFeature);
			// allmaps.push_back(linPerFeature.maps.front());
		}
	}

}


void GBVS360::getEquirectangularFeaturesJob() {

	bool taskFound = true;
	while(taskFound) {
		taskFound = false;

		// algo v1, easy, without packing of multiple features with the same resolution to save computational costs...
		if(m_ProjectedFrames.empty()) break; 					// If there are no frames available, stop.
		ProjectedFrame &frame = *m_ProjectedFrames.begin(); 	
		if(frame.features.empty()) break;						// If there are no features computed, stop.



		int channel = 0;
		int level = 0;
		int type = 0;

		// ----------------------------------------------------------------------------------
		// identify what task need to be done


		m_mutex.lock();
		
		// iterate each feature channel
		for(std::list<Feature>::iterator it = frame.features.begin() ; it != frame.features.end() && !taskFound ; ++it) {
			// iterate each feature map
			for(std::list< FeatureMap >::iterator mapIt = it->maps.begin() ; mapIt != it->maps.end()  && !taskFound ; ++mapIt) {
				if(!featureDone[mapIt->channel][mapIt->level][mapIt->type]) {

					// we found one feature map which needs to be back projected. 
					channel = mapIt->channel;
					level = mapIt->level;
					type = mapIt->type;

					// we mark it as `done`, e.g. in progress...
					featureDone[mapIt->channel][mapIt->level][mapIt->type] = true;

					// a task was found.
					taskFound = true;
				}
			}
		}

		m_mutex.unlock();

		// if nothing to do, then close the thread.
		if(!taskFound) {
			return;
		}

		// ----------------------------------------------------------------------------------
		// back-project the identified feature map.



		// Find the feature map between the equirectilinear maps
		for(std::list<Feature>::iterator featureItEquirect = features.begin() ; featureItEquirect != features.end() ; ++featureItEquirect) {
			if(featureItEquirect->channel != channel) continue; // if we are not on the right channel, no need to check the maps

			// iterate each feature map
			for(std::list< FeatureMap >::iterator mapItEquirectilinear = featureItEquirect->maps.begin() ; mapItEquirectilinear != featureItEquirect->maps.end() ; ++mapItEquirectilinear) {
				if(mapItEquirectilinear->level == level && mapItEquirectilinear->type == type && mapItEquirectilinear->channel == channel) {


					cv::Mat nbProj(mapItEquirectilinear->map.rows, mapItEquirectilinear->map.cols, CV_8UC1, cv::Scalar(0));

					for(std::list< ProjectedFrame >::iterator projIt = m_ProjectedFrames.begin() ; projIt != m_ProjectedFrames.end() ; ++projIt) {
						



						// Find the feature map between the equilinear maps
						for(std::list<Feature>::iterator featureIt = projIt->features.begin() ; featureIt != projIt->features.end() ; ++featureIt) {

							if(featureIt->channel != channel) continue; // if we are not on the right channel, no need to check the maps

							// iterate each feature map
							for(std::list< FeatureMap >::iterator mapItEquilinear = featureIt->maps.begin() ; mapItEquilinear != featureIt->maps.end() ; ++mapItEquilinear) {
								
								if((mapItEquilinear->level == level && mapItEquilinear->type == type && mapItEquilinear->channel == channel)) {

									cv::Mat tempEquirect(mapItEquirectilinear->map.rows, mapItEquirectilinear->map.cols, CV_32FC3, cv::Scalar(0.f,0.f,0.f));
									cv::Mat floatFeature(mapItEquilinear->map.rows, mapItEquilinear->map.cols, CV_32FC3, cv::Scalar(0.f,0.f,0.f));

									for(int i = 0 ; i < mapItEquilinear->map.rows ; ++i) {
										for(int j = 0 ; j < mapItEquilinear->map.cols ; ++j) {
											cv::Point3_<float> &dst = floatFeature.at< cv::Point3_<float> >(i,j);
											dst.x = static_cast<float>(mapItEquilinear->map.at<double>(i,j));
											dst.y = 1;
											dst.z = 0;
										}
									}


									m_Projection->rectilinearToEquirectangularFC3(floatFeature, tempEquirect, static_cast<float>(projIt->nrAzim), static_cast<float>(projIt->nrElev));

									for(int i = 0 ; i < tempEquirect.rows ; ++i) {
										for(int j = 0 ; j < tempEquirect.cols ; ++j) {
											cv::Point3_<float> &src = tempEquirect.at< cv::Point3_<float> >(i,j);

											// if that is not part of the feature, skip
											if(src.y < 0.999f) continue;

											double v = mapItEquirectilinear->map.at<double>(i,j);

											v += src.x;

											++nbProj.at<unsigned char>(i,j);

											mapItEquirectilinear->map.at<double>(i,j) = v;

										}
									} 
								}
							}
						}

					} // end loop projIt


					for(int i = 0 ; i < mapItEquirectilinear->map.rows ; ++i) {
						for(int j = 0 ; j < mapItEquirectilinear->map.cols ; ++j) {
							mapItEquirectilinear->map.at<double>(i,j) /= nbProj.at<unsigned char>(i,j);
						}
					}

					cv::Mat tmp;
					cv::resize(mapItEquirectilinear->map, tmp, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);
					mapItEquirectilinear->map = tmp;
				}
			}
		}



	} // end while task


}


// ----------------------------------------------------------------------------------------------------------------------------------------------------
// redefine GBVS functions 

cv::Mat GBVS360::simpledistance	(const std::pair<int, int>& dim, int ) const {

	return Option::distScaling * GBVS::simpledistance(dim, 0);
	
	int N = dim.first*dim.second;
	cv::Mat d(N, N, CV_64FC1, cvScalar(0.f));

	double l0 = static_cast<float>(dim.second)/2;
	double p0 = static_cast<float>(dim.first)/2;

	#define idx(mi,mj) ((mj)*dim.first+(mi))

	#ifndef PI
	#define PI 3.14159265358979116f
	#endif

	for(int i = 0 ; i < dim.second ; ++i) {
		for(int j = 0 ; j < dim.first ; ++j) {
			for(int ii = 0 ; ii < dim.second ; ++ii) {
				for(int jj = 0 ; jj < dim.first ; ++jj) {

					// replace the distance by the haversine distance to account for the non-linearity of the grid in equirectangular domain. 
					if(d.at<double>(idx(j,i), idx(jj,ii)) == 0) {

						double haversine = 0;

						// TO CHECK:
						// shall this be " - p0 " & "/ dim.first" as j loop between [0-dim.first] ? 
						// I think that mistake added an equatorial prior which is a positive thing... 
                        double phy1   = PI*(j + .5  - p0) / static_cast<double>(dim.first);
                        double phy2   = PI*(jj + .5 - p0) / static_cast<double>(dim.first);
                        double lambda1 =  PI*(i + .5 - l0) / static_cast<double>(dim.second)/2;
                        double lambda2 =  PI*(ii + .5 - l0) / static_cast<double>(dim.second)/2;


                        double sn1 = std::sin((phy2-phy1)/2);
                        double sn2 = std::sin((lambda2-lambda1)/2);

                        haversine = sqrt(sn1*sn1 + std::cos(phy1)*std::cos(phy2)*sn2*sn2);
                        if(haversine > 1.0)  haversine = 1.0;	// this can happens due to rounding error... 
                        if(haversine < -1.0) haversine = -1.0;

                        // haversine = m_Projection->nrrWidth*dim.first*std::asin(haversine); //dim.first*
						haversine = Option::distScaling * dim.first*std::asin(haversine); //dim.first*
						// haversine = dim.first*std::asin(haversine);

						// haversine = dim.first * 2 * std::atan2(std::sqrt(haversine), std::sqrt(1-haversine));
						

						d.at<double>(idx(j,i), idx(jj,ii)) = haversine;
						d.at<double>(idx(jj,ii), idx(j,i)) = haversine;
					}
				}
			}
		}
	}

	#undef idx

	return d;
}


void GBVS360::attenuateBordersGBVS(cv::Mat &, int ) const {
	// do nothing -- remove central prior

	// if(size == 4) return;

	// if(size == 13) GBVS::attenuateBordersGBVS(image, 5);
}



const cv::Mat * GBVS360::findMap(char channel, int level, int type) const {
	int channelId = -1;
	for(int i = 0 ; i < static_cast<int>(channels.size()) ; ++i) {
		if(channels[i] == channel) {
			channelId = i;
			break;
		}
	}

	if(channelId == -1) return NULL;

	for(std::list<FeatureMap>::const_iterator it = allmaps.begin() ; it != allmaps.end() ; ++it) {
		if(it->channel == channelId && it->type == type && it->level == level) 
			return &it->map;
	}

	return NULL;
}







