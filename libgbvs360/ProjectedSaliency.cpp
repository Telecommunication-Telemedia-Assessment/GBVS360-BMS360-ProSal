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




#include "ProjectedSaliency.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <limits>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "Projection.h"
#include "Options.h"


ProjectedSaliency::ProjectedSaliency() {

	blurfrac		= 20;
	maxDim			= -1;
}


boost::shared_ptr<Saliency> ProjectedSaliency::newInstance() {
	ProjectedSaliency *saliency = new ProjectedSaliency();
	saliency->m_Projection = m_Projection;

	if(m_Saliency)
		saliency->m_Saliency   = m_Saliency->newInstance();

	return boost::shared_ptr<Saliency>(saliency);
}




void ProjectedSaliency::process(const cv::Mat &input, cv::Mat &output, bool normalize) {
	// First project the equirectangular frame into several equilinear frames.
	
	std::cout << "[I] Getting rectilinear frames" << std::endl;
	getRectilinearFrames(input);

	// get saliency maps for each equililnear frames
	std::cout << "[I] Getting saliency per frame" << std::endl;
	getRectilinearSaliency();

	// back project saliency maps to equirectangular coordinate.
	std::cout << "[I] Getting back-projected saliency map" << std::endl;
	getEquirectangularSaliency();

	// compute the activation of the features.
	// std::cout << "[I] Computing activation" << std::endl;
	getActivation(output);

	// check if the dimensions matches. It can be different due to constrains brought by the projection algo. (image size multiple of 4)
	if(output.size() != input.size()) {
		cv::resize(output, output, input.size());
	}


	if(normalize) {
		double mn, mx;

		int blur_width = (int)MIN(floor(blurfrac) * 4 + 1, 51);
		cv::GaussianBlur(output, output, cv::Size(blur_width, blur_width), blurfrac, blurfrac);


		cv::minMaxLoc(output, &mn, &mx);
		output = (output - mn) / (mx - mn);


	}
}


// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



void ProjectedSaliency::getRectilinearFrames(const cv::Mat &inputImage) {
	if(!m_Projection) {
		std::logic_error(std::string("ProjectedSaliency::getRectilinearFrames: The projection model was not allocated. Quit"));;
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
	for(int j = -nb_projections_h / 2  ; j <= nb_projections_h/2 ; ++j) {
		for(int i = 0 ; i < nb_projections_w ; ++i) {

			m_ProjectedFrames.push_back(ProjectedFrame());
			ProjectedFrame &frame = m_ProjectedFrames.back();

			frame.rectilinearFrame = cv::Mat(m_Projection->nrrHeight, m_Projection->nrrWidth, CV_8UC3, inputImage.channels());
			if(frame.rectilinearFrame.empty()) {
				throw std::logic_error(std::string("getRectilinearFrames::getEquilinarFrames bad alloc..."));
				return ;
			}

			frame.nrElev = static_cast<int>(j * m_Projection->nrApper/scaling_y);
			frame.nrAzim = static_cast<int>(i * m_Projection->nrApper/scaling_x);
			frame.taskDone = false;
		}
	}


	// Run `Option::threads` threads to do all the projections.
	boost::thread_group group;
	for(size_t i = 0 ; i < Option::threads ; ++i) {
		group.create_thread(boost::bind(&ProjectedSaliency::getRectilinearFramesJob, this, boost::ref(inputImage)));
	}
	group.join_all();


	// reset all task flags, to be ready for the next task
	for(std::list<ProjectedFrame>::iterator it = m_ProjectedFrames.begin() ; it != m_ProjectedFrames.end() ; ++it) {
		it->taskDone = false;
	}

}



void ProjectedSaliency::getRectilinearFramesJob(const cv::Mat &inputImage) {

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
		}
	}
}



// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



void ProjectedSaliency::getRectilinearSaliency() {
	if(!m_Saliency) {
		std::logic_error(std::string("ProjectedSaliency::getRectilinearSaliency: The saliency model was not allocated. Quit"));;
		return;
	}


	// If we use multithreading, we initiate workers who will do all the jobs.
	//m_SaliencyWorkers.push_back(m_Saliency);		 															// the reference saliency is a worker;
	for(size_t i = 0 ; i < Option::threads && i < m_ProjectedFrames.size() ; ++i) {		// instantiate all the workers;
		m_SaliencyWorkers.push_back(m_Saliency->newInstance());	
	}


	boost::thread_group group;
	for(int i = 0 ; i < static_cast<int>(m_SaliencyWorkers.size()) ; ++i) {
		group.create_thread(boost::bind(&ProjectedSaliency::getRectilinearSaliencyJob, this, i));
	}
	group.join_all();	
}



void ProjectedSaliency::getRectilinearSaliencyJob(int workerID) {
	boost::shared_ptr<Saliency> &saliency = m_SaliencyWorkers[workerID];

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
			saliency->estimate(projectedFrame->rectilinearFrame, projectedFrame->saliency, false);
		}

	}
}



void ProjectedSaliency::getActivation(cv::Mat &output) {

	cv::Mat featureMap(m_EquirectangularSaliency.rows, m_EquirectangularSaliency.cols, CV_32FC1);
	for(int i = 0 ; i < m_EquirectangularSaliency.rows ; ++i) {
		for(int j = 0 ; j < m_EquirectangularSaliency.cols ; ++j) {
			cv::Point3_<float> &eqr = m_EquirectangularSaliency.at< cv::Point3_<float> >(i,j);
			featureMap.at<float>(i,j) = eqr.x;
		}
	}

	output = featureMap;

}



void ProjectedSaliency::getEquirectangularSaliency() {

	float scaling = 1.f;
	if(maxDim > 0) {
		int largeS = std::max(m_Projection->nreHeight, m_Projection->nreWidth);
		if(maxDim < largeS) {
			scaling = static_cast<float>(maxDim) / static_cast<float>(largeS);
		}
	}

	int rows = 4 * (static_cast<int>(m_Projection->nreHeight*scaling) / 4); // 0
	int cols = 4 * (static_cast<int>(m_Projection->nreWidth*scaling) / 4); // 1

	m_EquirectangularSaliency = cv::Mat(rows, cols, CV_32FC3, cv::Scalar(0.f,0.f,0.f));
	cv::Mat count(rows, cols, CV_32FC1, cv::Scalar(0.f));

	for(std::list<ProjectedFrame>::iterator it = m_ProjectedFrames.begin() ; it != m_ProjectedFrames.end() ; ++it) {
		cv::Mat tmp(rows, cols, CV_32FC3, cv::Scalar(0,0,0));

		m_Projection->nrAzim = it->nrAzim;
		m_Projection->nrElev = it->nrElev;

		cv::Mat toProjSailMap(it->saliency.rows, it->saliency.cols, CV_32FC3, cv::Scalar(0.f,0.f,0.f));
		for(int i = 0 ; i < toProjSailMap.rows ; ++i) {
			for(int j = 0 ; j < toProjSailMap.cols ; ++j) {
				cv::Point3_<float> &dst = toProjSailMap.at< cv::Point3_<float> >(i,j);
				dst.x = it->saliency.at<float>(i,j);
				dst.y = 1;
			}
		}


		m_Projection->rectilinearToEquirectangularFC3(toProjSailMap, tmp);

		for(int i = 0 ; i < m_EquirectangularSaliency.rows ; ++i) {
			for(int j = 0 ; j < m_EquirectangularSaliency.cols ; ++j) {
				cv::Point3_<float> &col = tmp.at< cv::Point3_<float> >(i,j);
				cv::Point3_<float> &dst = m_EquirectangularSaliency.at< cv::Point3_<float> >(i,j);
				
				if(col.y < 0.999) continue;

				++count.at<float>(i,j);
				dst.x += col.x;
			}
		}
	}



	for(int i = 0 ; i < m_EquirectangularSaliency.rows ; ++i) {
		for(int j = 0 ; j < m_EquirectangularSaliency.cols ; ++j) {
			cv::Point3_<float> &dst = m_EquirectangularSaliency.at< cv::Point3_<float> >(i,j);

			float c = count.at<float>(i,j);

			if(c < 1) continue;

			dst.x /= c;
		}
	}
}


// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



