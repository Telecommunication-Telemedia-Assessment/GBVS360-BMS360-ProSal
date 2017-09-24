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



}; 


#endif

