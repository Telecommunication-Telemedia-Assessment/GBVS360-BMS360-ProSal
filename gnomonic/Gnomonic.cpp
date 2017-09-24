#include "Gnomonic.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <gnomonic-all.h>
#include "common-method.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <json_spirit.h>

#include <string>
#include <fstream>

Gnomonic::Gnomonic() {
    m_fov = 110.f;
    m_threads = 12;
    m_nrrHeight = 720;
    m_nrrWidth = 720;
	m_average = false;
}

void Gnomonic::getRectilinearFrames(const cv::Mat &inputImage, const std::string &name) {

	float scaling_x = 2;
	float scaling_y = 1;


	int nb_projections_w = static_cast<int>(std::ceil(360.f / static_cast<float>(m_fov/scaling_x)));
	int nb_projections_h = static_cast<int>(std::ceil(180.f / static_cast<float>(m_fov/scaling_y)));


	// prepare all the projections: what needs to be done.
	if(m_equatorialRegion) {
		for(int i = 0 ; i < nb_projections_w ; ++i) {

			m_ProjectedFrames.push_back(ProjectedFrame());
			ProjectedFrame &frame = m_ProjectedFrames.back();

			frame.rectilinearFrame = cv::Mat(m_nrrHeight, m_nrrWidth, CV_8UC3, inputImage.channels());
			if(frame.rectilinearFrame.empty()) {
				throw std::logic_error(std::string("Gnomonic::getRectilinearFrames bad alloc..."));
				return ;
			}

			frame.nrElev = static_cast<int>(0);
			frame.nrAzim = static_cast<int>(i * m_fov/scaling_x);
			frame.taskDone = false;
		}
	} else {
		for(int j = -nb_projections_h / 2  ; j <= nb_projections_h/2 ; ++j) {
			for(int i = 0 ; i < nb_projections_w ; ++i) {

				m_ProjectedFrames.push_back(ProjectedFrame());
				ProjectedFrame &frame = m_ProjectedFrames.back();

				frame.rectilinearFrame = cv::Mat(m_nrrHeight, m_nrrWidth, CV_8UC3, inputImage.channels());
				if(frame.rectilinearFrame.empty()) {
					throw std::logic_error(std::string("Gnomonic::getRectilinearFrames bad alloc..."));
					return ;
				}

				frame.nrElev = static_cast<int>(j * m_fov/scaling_y);
				frame.nrAzim = static_cast<int>(i * m_fov/scaling_x);
				frame.taskDone = false;
			}
		}
	}
	


	// Run `Option::threads` threads to do all the projections.
	boost::thread_group group;
	for(int i = 0 ; i < m_threads ; ++i) {
		group.create_thread(boost::bind(&Gnomonic::getRectilinearFramesJob, this, boost::ref(inputImage)));
	}
	group.join_all();


	// reset all task flags, to be ready for the next task
    int imageId = 0;
	std::string json = "{\n\t\"name\": \"" + name + "\",\n\t\"width\": " + boost::lexical_cast<std::string>(inputImage.cols) + ",\n\t\"height\": " + boost::lexical_cast<std::string>(inputImage.rows) + ",\n\t\"fov\": " + boost::lexical_cast<std::string>(m_fov) +  ",\n\t\"data\": [\n";
	for(std::list<ProjectedFrame>::iterator it = m_ProjectedFrames.begin() ; it != m_ProjectedFrames.end() ; ++it) {
		it->taskDone = false;
		std::ostringstream ss;
		ss << std::setw(3) << std::setfill('0') << imageId;
		const std::string str = ss.str();

        cv::imwrite(m_outputDir + str + ".jpg", it->rectilinearFrame);

		json += "\t\t{\"file\": \""+ m_outputDir + str + ".jpg" + "\", \"pitch\": " + boost::lexical_cast<std::string>(it->nrElev) + ", \"yaw\": " + boost::lexical_cast<std::string>(it->nrAzim) + ", \"roll\": 0}, \n";

        ++imageId;
	}
	json += "\t]\n}";

	std::ofstream f((m_outputDir + "projection.json").c_str());
	f << json;
	f.close();

}



void Gnomonic::getRectilinearFramesJob(const cv::Mat &inputImage) {

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
			equirectangularToRectilinear(inputImage, projectedFrame->rectilinearFrame, static_cast<float>(projectedFrame->nrAzim), static_cast<float>(projectedFrame->nrElev));
		}
	}
}


void Gnomonic::getEquirectangular(const std::string &filename, const std::string& outputFile) {
	m_ProjectedFrames.clear();

	std::ifstream is(filename.c_str());
    json_spirit::Value value;
    json_spirit::read( is, value );

	std::string outputName;
	int nre_width = 0;
	int nre_height = 0;

    json_spirit::Object& alldatas = value.get_obj();
    for( json_spirit::Object::size_type j = 0; j != alldatas.size(); ++j ) {
        
        if(alldatas[j].name_ == "name") {
            outputName = alldatas[j].value_.get_str();
        }

		if(alldatas[j].name_ == "width") {
            nre_width = alldatas[j].value_.get_int();
        }

		if(alldatas[j].name_ == "height") {
            nre_height = alldatas[j].value_.get_int();
        }

		if(alldatas[j].name_ == "fov") {
            m_fov = alldatas[j].value_.get_int();
        }

		if(alldatas[j].name_ == "data") {
			const json_spirit::Pair&   videosDataPair  = alldatas[j];
			const json_spirit::Value&  videosDataValue = videosDataPair.value_;


			const json_spirit::Array& videosData = videosDataValue.get_array();
			for(unsigned int i = 0 ; i < videosData.size() ; ++i) {

				const json_spirit::Object& videoData = videosData[i].get_obj();

				ProjectedFrame proj;
				proj.taskDone = false;

				for(json_spirit::Object::size_type k = 0 ; k != videoData.size() ; ++k) {
					const json_spirit::Pair& pair = videoData[k];
					const std::string& name  = pair.name_;
					const json_spirit::Value& localV = pair.value_;

						
					if(name == "file") {
						proj.rectilinearFrame = cv::imread(localV.get_str());
						if(proj.rectilinearFrame.empty()) {
							std::cout << "[W] could not open: " << localV.get_str() << std::endl;
						}
					}

					if(name == "pitch") {
						proj.nrElev = localV.get_int();
					}

					if(name == "yaw") {
						proj.nrAzim = localV.get_int();
					}

					if(name == "roll") {
						proj.nrRoll = localV.get_int();
					}
				}

				m_ProjectedFrames.push_back(proj);
			}
		}
	}

	cv::Mat equiRect(nre_height, nre_width, CV_8UC3, cv::Scalar(0,0,0));

	if(!m_average) {
		for(ProjectedFrame &proj : m_ProjectedFrames) {
			// std::cout << proj.rectilinearFrame.cols << ", " << proj.nrElev << ", " << proj.nrAzim << ", " << proj.nrRoll << std::endl;
			if(proj.rectilinearFrame.empty()) continue;

			rectilinearToEquirectangular(proj.rectilinearFrame, equiRect, proj.nrAzim, proj.nrElev, proj.nrRoll);
		}

	} else {

		boost::thread_group group;
		for(int i = 0 ; i < m_threads ; ++i) {
			group.create_thread(boost::bind(&Gnomonic::getEquirectangularFramesJob, this, nre_height, nre_width));
		}
		group.join_all();

		cv::Mat equiRect2(nre_height, nre_width, CV_32FC1, cv::Scalar(0));
		for(ProjectedFrame &proj : m_ProjectedFrames) {

			cv::Mat &eqr = proj.rectilinearFrame;

			equiRect2.forEach< float >([eqr](float &p, const int* position) {
				const unsigned char &lp = eqr.at< unsigned char >(position[0], position[1]);

				p += static_cast<float>(lp) / 255.f;
			});
		}

		cv::normalize(equiRect2, equiRect2, 0, 1, cv::NORM_MINMAX);


		equiRect.forEach< cv::Point3_<unsigned char> >([equiRect2](cv::Point3_<unsigned char> &p, const int* position) {
			const float &lp = equiRect2.at< float >(position[0], position[1]);
			unsigned char v = 255 * lp;

			p.x = v;
			p.y = v;
			p.z = v;
		});

	}

	if(outputFile.empty())
		cv::imwrite(outputName + ".backproj.png", equiRect);
	else
		cv::imwrite(outputFile, equiRect);


}

void Gnomonic::getEquirectangularFramesJob(int nre_height, int nre_width) {
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
			cv::Mat gray(projectedFrame->rectilinearFrame.size(), CV_8UC1);
			cv::Mat &src = projectedFrame->rectilinearFrame;
			gray.forEach< unsigned char >([src](unsigned char &p, const int* position) {
				p = src.at< cv::Point3_<unsigned char> >(position[0], position[1]).x;
			} );

			cv::Mat equiRect(nre_height, nre_width, CV_8UC1, cv::Scalar(0));
			rectilinearToEquirectangular(projectedFrame->rectilinearFrame, equiRect, projectedFrame->nrAzim, projectedFrame->nrElev, projectedFrame->nrRoll);
			projectedFrame->rectilinearFrame = equiRect;
		}
	}

}

void Gnomonic::rectilinearToEquirectangular(const cv::Mat& inputImage, cv::Mat& output, float azim, float elev, float roll) {
    lg_gte_apperturep( 
        ( inter_C8_t * ) output.data,
        output.cols,
        output.rows,
        output.channels(),
        ( inter_C8_t * ) inputImage.data,
        inputImage.cols,
        inputImage.rows,
        inputImage.channels(),
        azim  * ( LG_PI / 180.0 ),
        elev  * ( LG_PI / 180.0 ),
        roll  * ( LG_PI / 180.0 ),
        m_fov * ( LG_PI / 180.0 ),
        lc_method( "bicubicf"  ),
        8
    );
}

void Gnomonic::equirectangularToRectilinear(const cv::Mat& inputImage, cv::Mat& output, float azim, float elev, float roll) {
    lg_etg_apperturep( 
        ( inter_C8_t * ) inputImage.data,
        inputImage.cols,
        inputImage.rows,
        inputImage.channels(),
        ( inter_C8_t * ) output.data,
        output.cols,
        output.rows,
        output.channels(),
        azim    * ( LG_PI / 180.0 ),
        elev    * ( LG_PI / 180.0 ),
        roll    * ( LG_PI / 180.0 ),
        m_fov * ( LG_PI / 180.0 ),
        lc_method( "bicubicf" ),
        8
    );
}
