#ifndef _Gnomonic_
#define _Gnomonic_

#include <opencv2/core.hpp>
#include <boost/thread/mutex.hpp>
#include <list>

struct ProjectedFrame {
	cv::Mat 					rectilinearFrame;
	int 						nrElev;
	int 						nrAzim;
    int                         nrRoll;
	bool						taskDone;
} ;




class Gnomonic {
    std::list<ProjectedFrame>   m_ProjectedFrames;
    float                       m_fov;
    int                         m_nrrHeight;
    int                         m_nrrWidth;
    int                         m_threads;
    bool                        m_average;
    bool                        m_equatorialRegion;

    boost::mutex				m_mutex;
    std::string                 m_outputDir;

public:
                    Gnomonic                ();
	void 			getRectilinearFrames	(const cv::Mat &inputImage, const std::string &name = "");
    void            getEquirectangular      (const std::string &filename, const std::string& outputFile);


    inline void     setFOV                  (float fov)                         { m_fov = fov; }
    inline void     setNrrHeight            (int height)                        { m_nrrHeight = height; }
    inline void     setNrrWidth             (int width)                         { m_nrrWidth = width; }
    inline void     setNbThreads            (int threads)                       { m_threads = threads; }
    inline void     setOutputDir            (const std::string& dir)            { m_outputDir = dir; }
    inline void     setAverageOverlap       (bool avg)                          { m_average = avg; }
    inline void     setEquatorialRegion     (bool eq )                          { m_equatorialRegion = eq; }

    inline int      getHeight               () const                            { return m_nrrHeight; }
    inline int      getWidth                () const                            { return m_nrrWidth; }

    void            rectilinearToEquirectangular(const cv::Mat& inputImage, cv::Mat& output, float azim, float elev, float roll = 0.f);
    void            equirectangularToRectilinear(const cv::Mat& inputImage, cv::Mat& output, float azim, float elev, float roll = 0.f);

private:

	void			getRectilinearFramesJob	    (const cv::Mat &inputImage);
    void			getEquirectangularFramesJob	(int nre_height, int nre_width);




} ; 



#endif
