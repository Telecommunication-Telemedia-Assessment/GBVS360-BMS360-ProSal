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




#include "SaliencyBuilder.h"
#include <iostream>
#include <fstream>
#include <json_spirit.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <gnomonic-all.h>
#include "common-method.h"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

SaliencyBuilder::SaliencyBuilder(int rows, int cols, int framerate, float fov) {
    m_rows = rows;
    m_cols = cols;
    m_framerate = framerate;
    m_fov = fov;
    m_threads = 12;
    m_fourcc = "X264";
    m_show = false;
    m_mergeHMDs = false;
    m_inversePitchAxis = false;
    m_fixationMaps = false;
    m_nbFrames = -1;
	m_videoOverlayType = 1;
    m_rawOutputMode = 0;
}


void SaliencyBuilder::loadLog(const std::string& filename) {
    std::cout << filename << std::endl;

    std::ifstream is(filename.c_str());
    json_spirit::Value value;
    json_spirit::read( is, value );



    json_spirit::Object& alldatas = value.get_obj();
    for( json_spirit::Object::size_type j = 0; j != alldatas.size(); ++j ) {
        
        if(alldatas[j].name_ != "data") {
            continue;
        }

        const json_spirit::Pair&   videosDataPair  = alldatas[j];
        const json_spirit::Value&  videosDataValue = videosDataPair.value_;


        const json_spirit::Array& videosData = videosDataValue.get_array();
        for(unsigned int i = 0 ; i < videosData.size() ; ++i) {

            const json_spirit::Object& videoData = videosData[i].get_obj();

            Record record;

            for(json_spirit::Object::size_type k = 0 ; k != videoData.size() ; ++k) {
                const json_spirit::Pair& pair = videoData[k];
                const std::string& name  = pair.name_;
                const json_spirit::Value& localV = pair.value_;

	                
                if(name == "filename") {
                    record.m_name = localV.get_str();
                }

                if(name == "hmd") {
                    record.m_hmd = localV.get_str();
                }

                if(name == "pitch_yaw_roll_data" || name == "pitch_yaw_roll_data_hmd") {
                    const json_spirit::Array& fixData = localV.get_array();

                    for(unsigned int f = 0 ; f < fixData.size() ; ++f) {

                        const json_spirit::Object& fixObj = fixData[f].get_obj();

                        for(json_spirit::Object::size_type k2 = 0 ; k2 != fixObj.size() ; ++k2) {
                            const json_spirit::Pair& fixPair = fixObj[k2];
                            const std::string& fixName  = fixPair.name_;
                            const json_spirit::Value& fixV = fixPair.value_;

                            if(fixName == "pitch") {
                                record.m_pitch.push_back(fixV.get_real());
                            }

                            if(fixName == "roll") {
                                record.m_roll.push_back(fixV.get_real());
                            }

                            if(fixName == "yaw") {
                                record.m_yaw.push_back(fixV.get_real());
                            }

                            if(fixName == "sec") {
                                record.m_sec.push_back(fixV.get_real());
                            }
                        }
                    }
                }
            }

            if(!m_filterHMD.empty() && m_filterHMD != record.m_hmd) continue;

            if(!m_processedVideo.empty() && m_processedVideo != record.m_name) continue;

            if(m_mergeHMDs)
                m_logs[record.m_name].push_back(record);
            else
                m_logs[record.m_hmd + "_" + record.m_name].push_back(record);
        }
    }
}


void SaliencyBuilder::applyLog(const std::vector<Record>& records) {
	m_temporalStack.clear();

    cv::Mat gauss(240, 324, CV_32FC1, cv::Scalar(0.f));

    for(int i = 0 ; i < 2 ; ++i) {
        for(int j = 0 ; j < 2 ; ++j) {
            gauss.at<float>(119+i,161+j) = 1.f;
        }
    }

    cv::GaussianBlur(gauss, gauss, cv::Size(203,203), 20, 20);
    cv::normalize(gauss, gauss, 0, 1, cv::NORM_MINMAX);


    cv::VideoWriter outputVideo;
    cv::VideoCapture inputVideo;
    cv::Size S = cv::Size(m_cols, m_rows);
    FILE *binFile = NULL;

    // --------------------------------------------------------------------------------------------------------
    // define output path

    std::string outputFile;
    std::string extension;

    if(m_rawOutputMode > 0)
        extension = ".bin";
    else
        extension = ".mkv";

    if(m_mergeHMDs)
        outputFile = records[0].m_name + extension;
    else
        outputFile = records[0].m_hmd + "_" + records[0].m_name + extension;


    if(m_skipExisting) {
        if(boost::filesystem::exists( outputFile )) {
            std::cout << "[I] Skip : " << outputFile << std::endl;
            return;
        }  
    }



    // --------------------------------------------------------------------------------------------------------
    // Open a video and show the saliency map as on overlay of this video
    

    

    if(!m_videoOverlay.empty()) {
        inputVideo.open(m_videoOverlay + "/" + records[0].m_name);
        if(!inputVideo.isOpened()) {
            std::cerr << "Cannot open: " << (m_videoOverlay + "/" + records[0].m_name) << std::endl;
        }
    }

    // --------------------------------------------------------------------------------------------------------
    // handle output video

    if(m_rawOutputMode == 0) {
        outputVideo.open(outputFile, CV_FOURCC(m_fourcc[0], m_fourcc[1], m_fourcc[2], m_fourcc[3]), m_framerate, S, !m_videoOverlay.empty()); // !m_videoOverlay.empty() -> if there is no RGB video, we can only use a B&W output

        if(!outputVideo.isOpened()) {
            std::cerr << "Cannot write: " << (records[0].m_name + ".mkv") << std::endl;
        }
    } else {
        binFile = fopen(outputFile.c_str(), "wb");
        if(binFile == NULL) {
            std::cerr << "Cannot write: " << (records[0].m_name + ".bin") << std::endl;
        }
    }


    // --------------------------------------------------------------------------------------------------------
    // Find all fixation data for every frame



    int nbFrames = 0; 
    for(size_t k = 0 ; k < records.size() ; ++k) {
        if(!records[k].m_sec.empty())
            nbFrames = std::max(nbFrames, static_cast<int>(records[k].m_sec.back() * m_framerate));
    }



    printf("[%04d/%04d]", 0, nbFrames);

    for(int frame = 0 ; frame < nbFrames ; ++frame) {
        
        // find all fixations, and create Gaussian projection jobs 
        std::vector<Job> jobs;
        for(size_t i = 0 ; i < records.size() ; ++i) { // look in each user log
            bool found = false;
            int  prevPos = 0;

            for(size_t k = 0 ; k < records[i].m_sec.size() ; ++k) { // for each user, look if there is fixation data for the frame "frame"
                
                if(static_cast<int>(records[i].m_sec[k]*m_framerate) == frame) {
                    found = true;
                    Job job;
                    job.m_done = false;
                    job.m_yaw = records[i].m_yaw[k];
                    job.m_pitch = records[i].m_pitch[k];
                    job.m_roll = records[i].m_roll[k];
                    job.m_frameIdx = k;

                    jobs.push_back(job);
                }

                if(!found) {
                    if(records[i].m_sec[k]*m_framerate < frame)
                        prevPos = k;
                }
            }

            // if no fixation data is available for the given frame, apply the previous fixation
            // This is because the sampling rate of the HMD is lower than the video frame rate. 
            // fixations data may only be available for frames 1, 5, 10, ... so we need to propagate the data from frame 1 to 2-4 (to avoid blank frames)
            if(!found) {
                if(!records[i].m_yaw.empty()) {
                    Job job;
                    job.m_done = false;
                    job.m_yaw = records[i].m_yaw[prevPos];
                    job.m_pitch = records[i].m_pitch[prevPos];
                    job.m_roll = records[i].m_roll[prevPos];
                    job.m_frameIdx = prevPos;

                    jobs.push_back(job);
                }
            }
        }


        // Output saliency map
        cv::Mat salMap(m_rows, m_cols, CV_32FC1, cv::Scalar(0.f));


        if(m_fixationMaps) {

            // --------------------------------------------------------------------------------------------------------
            // If we aim at generating a fixation map

            
            for( Job &job : jobs) {
                float pitch = -job.m_pitch;
                
                if(m_inversePitchAxis) pitch = -pitch;

                while(pitch < -90.f)  pitch += 180.f;
                while(pitch >  90.f) pitch -= 180.f;

                pitch = pitch / 180.f;
                int y = salMap.rows/2 + static_cast<int>(pitch*salMap.rows);
                if(y >= salMap.rows) y = salMap.rows - 1;
                if(y < 0) y = 0;


                float yaw = job.m_yaw;
                while(yaw < -180.f)  yaw += 360.f;
                while(yaw >  180.f)  yaw  -= 360.f;
                yaw = yaw / 360.f;

                int x = salMap.cols/2 + static_cast<int>(yaw*salMap.cols);
                if(x >= salMap.cols) x = salMap.cols - 1;
                if(x < 0) x = 0;

                salMap.at<float>(y, x) = 1.0f;
                
            }


        } else {

            // --------------------------------------------------------------------------------------------------------
            // If we aim at generating a saliency map

            // do the processing in m_threads threads
            boost::thread_group g;
            for(int i = 0 ; i < m_threads ; ++i) {
                g.create_thread(boost::bind(&SaliencyBuilder::projectionJob, this, boost::ref(jobs), boost::ref(gauss)));
            }
            g.join_all();


            // finally, we merge all the equirectangular images (one for each fixation)
            for( Job &job : jobs) {
                salMap += job.m_frame;
            }

            cv::normalize(salMap, salMap, 0, 1, cv::NORM_MINMAX);



            // --------------------------------------------------------------------------------------------------------
            // Add a temporal smoothing: the frame N is averaged with frames N-1, N-2, N3, N-4 with a Gaussian weighting

            m_temporalStack.push_back(salMap.clone());
            if(m_temporalStack.size() > static_cast<unsigned int>(m_framerate/5)) {
                m_temporalStack.pop_front();
            }

            auto it = m_temporalStack.rbegin();
            ++it;
            for(int depth = 1 ; it != m_temporalStack.rend() ; ++it) {
                salMap += (*it) * exp(-(depth*depth)/(2*5));
                ++depth;
            }

            cv::normalize(salMap, salMap, 0, 1, cv::NORM_MINMAX);

        }



        // --------------------------------------------------------------------------------------------------------
        // If the saliency is on top of a video, we read the frame and apply the overlay


        if(!m_videoOverlay.empty() && inputVideo.isOpened()) {
            cv::Mat colorImage;
            inputVideo >> colorImage;

			if (colorImage.empty()) {
				
                colorImage = cv::Mat(salMap.size(), CV_8UC3);
                colorImage.forEach< cv::Point3_<unsigned char> >([salMap](cv::Point3_<unsigned char> &p, const int * position) -> void {
                    float sal = salMap.at<float>(position[0], position[1]);
                    p.x = sal * 255;
                    p.y = sal * 255;
                    p.z = sal * 255;
                });
                salMap = colorImage;

			}
			else {
                
				cv::resize(colorImage, colorImage, salMap.size(), 0, 0);

				if (m_videoOverlayType == 1) {
                    colorImage.forEach< cv::Point3_<unsigned char> >([salMap](cv::Point3_<unsigned char> &p, const int * position) -> void {
                        float sal = salMap.at<float>(position[0], position[1]);
                        p.x = static_cast<int>(p.x) / 2;
                        p.y = static_cast<int>(p.y) / 2;
                        p.z = std::min(255.f, static_cast<int>(p.z) / 2 + 255 * sal);                        
                    });

				} else {
                    colorImage.forEach< cv::Point3_<unsigned char> >([salMap](cv::Point3_<unsigned char> &p, const int * position) -> void {
                        float sal = salMap.at<float>(position[0], position[1]);
                        p.x = static_cast<int>(p.x) * sal;
                        p.y = static_cast<int>(p.y) * sal;
                        p.z = static_cast<int>(p.z) * sal;                        
                    });
				}

				salMap = colorImage;
			}
        }


        // --------------------------------------------------------------------------------------------------------
        // Display processing status


        printf("\b\b\b\b\b\b\b\b\b\b\b[%04d/%04d]", frame, nbFrames);
        fflush(stdout);

        if(m_show) {
            cv::imshow("Saliency map", salMap);
            cv::waitKey(10);
        }


        // --------------------------------------------------------------------------------------------------------
        // write output video

        if(m_videoOverlay.empty()) {

            if(m_rawOutputMode == 0) {
                salMap = salMap * 255;
                salMap.convertTo(salMap, CV_8UC1);
            }
            
        } else {
            if(!inputVideo.isOpened()) {
                cv::Mat colorImage(salMap.size(), CV_8UC3);
                colorImage.forEach< cv::Point3_<unsigned char> >([salMap](cv::Point3_<unsigned char> &p, const int * position) -> void {
                    float sal = salMap.at<float>(position[0], position[1]);
                    p.x = sal * 255;
                    p.y = sal * 255;
                    p.z = sal * 255;
                });
                salMap = colorImage;
            }
        }

        if(m_rawOutputMode == 0) {
            outputVideo << salMap;
        } else {
            switch(m_rawOutputMode) {
                case 1: 
                    cv::normalize(salMap, salMap, 0, 1, cv::NORM_MINMAX);
                    salMap = salMap * 255;
                    salMap.convertTo(salMap, CV_8UC1);
                    fwrite(salMap.data, 1, salMap.cols * salMap.rows, binFile);
                    break;

                case 2:
                default:
                    fwrite(salMap.data, sizeof(float), salMap.cols * salMap.rows, binFile);
                    break;
            }
        }
        

        if(frame == m_nbFrames) break;
    }

    if(binFile != NULL) {
        fclose(binFile);
    }

}

void SaliencyBuilder::projectionJob(std::vector<Job>& jobs, const cv::Mat& gauss) {
    bool taskFound = true;
    int taskId = 0;
    

    while(taskFound) {
        taskFound = false;

        m_mutex.lock();
        for(size_t i = 0 ; i < jobs.size() ; ++i) {
            if(!jobs[i].m_done) {
                taskFound = true;
                jobs[i].m_done = true;
                taskId = i;
                break;
            }
        }
        m_mutex.unlock();

        if(taskFound) {
            jobs[taskId].m_frame = cv::Mat(m_rows, m_cols, CV_32FC1, cv::Scalar(0.f));

            if(m_inversePitchAxis)
                rectilinearToEquirectangular(gauss, jobs[taskId].m_frame, 180.f + jobs[taskId].m_yaw, jobs[taskId].m_pitch, jobs[taskId].m_roll);
            else
                rectilinearToEquirectangular(gauss, jobs[taskId].m_frame, 180.f + jobs[taskId].m_yaw, -jobs[taskId].m_pitch, jobs[taskId].m_roll);
        }
    }

}

void SaliencyBuilder::build() {

    for(auto &pairs : m_logs) {
        applyLog(pairs.second);
    }

}



void SaliencyBuilder::rectilinearToEquirectangular(const cv::Mat& inputImage, cv::Mat& output, float azim, float elev, float roll) {
    lg_gte_apperturep_f( 
        ( float * ) output.data,
        output.cols,
        output.rows,
        1,
        ( float * ) inputImage.data,
        inputImage.cols,
        inputImage.rows,
        1,
        azim  * ( LG_PI / 180.0 ),
        elev  * ( LG_PI / 180.0 ),
        roll  * ( LG_PI / 180.0 ),
        m_fov * ( LG_PI / 180.0 ),
        lc_method_f( "bicubicf" ),
        8
    );
}