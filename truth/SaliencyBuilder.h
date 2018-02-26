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




#ifndef _SaliencyBuilder_
#define _SaliencyBuilder_

#include <string>
#include <opencv2/core.hpp>
#include <boost/thread/mutex.hpp>

#include <map>
#include <vector>
#include <list>

struct Job {
    bool                 m_done;
    float                m_yaw;
    float                m_roll;
    float                m_pitch;
    int                  m_frameIdx;
    cv::Mat              m_frame;
};

struct Record {
    std::string          m_name;
    std::string          m_hmd;
    std::vector<float>   m_pitch;
    std::vector<float>   m_yaw;
    std::vector<float>   m_roll;
    std::vector<float>   m_sec;
}; 

class SaliencyBuilder {
    int     m_cols;
    int     m_rows;
    int     m_framerate;
    float   m_fov;
    int     m_threads;
    std::string m_fourcc;
    bool    m_show;
    int     m_nbFrames;
    bool    m_mergeHMDs;
	int		m_videoOverlayType;
    bool    m_skipExisting;
    bool    m_inversePitchAxis;
    bool    m_fixationMaps;
    std::string m_processedVideo;
    std::string m_filterHMD;
    std::string m_videoOverlay;
    std::list<cv::Mat> m_temporalStack;

    std::map<std::string, std::vector<Record> > m_logs;
    boost::mutex							    m_mutex;

private:
    void applyLog                       (const std::vector<Record>& records);
    void rectilinearToEquirectangular   (const cv::Mat& inputImage, cv::Mat& output, float azim, float elev, float roll);
    void projectionJob                  (std::vector<Job>& jobs, const cv::Mat& gauss);

public:
    SaliencyBuilder                     (int rows, int cols, int framerate, float fov);

    void loadLog                        (const std::string& filename);
    void build                          ();


    inline void setThreadCounts (int t)                     { m_threads = t; }
    inline void setFramerate    (int f)                     { m_framerate = f; }
    inline void setFOV          (float f)                   { m_fov = f; }
    inline void setFourCC       (const std::string& fcc)    { m_fourcc = fcc; }
    inline void setShow         (bool show)                 { m_show = show;}
    inline void setNbFrames     (int nbFrames)              { m_nbFrames = nbFrames; }
    inline void setMergeHMDs    (bool merge)                { m_mergeHMDs = merge; }
    inline void setFilterHMD    (const std::string& hmd)    { m_filterHMD = hmd;}
    inline void setVideoOveraly (const std::string& folder) { m_videoOverlay = folder; }
	inline void setVideoOveralyType(int type)				{ m_videoOverlayType = type;  }
    inline void setSkipExisting (bool skip)                 { m_skipExisting = skip; }
    inline void setProcessedVideo(const std::string& vid)   { m_processedVideo = vid; }
    inline void setInversePitchAxis(bool value)             { m_inversePitchAxis = value; }
    inline void setFixationMaps (bool value)                { m_fixationMaps = value; }



}; 


#endif

