#ifndef _HMDSim_
#define _HMDSim_

#include <opencv2/core.hpp>

enum HMD {
    DK1 = 0,
    DK2,
    CV1,
    FK
};


class HMDSim {

private:
    float m_time;
    float m_nIntensity;
    float m_sIntensity;
    float m_sCount;
    float m_resolution;
    float m_opacity;
    float m_resScaling;
    int   m_blurKernelSize;
    float m_blurKernelSigma;

    int m_renderW;
    int m_renderH;

    bool m_enableSD;

public:
    HMDSim                  ();
    void paramBuilder       (HMD hmd);
    void applyFilter        (const cv::Mat& input, cv::Mat& result);
    inline void switchSD    ()                                              { m_enableSD = !m_enableSD; }
    inline bool isSD        ()                                              { return m_enableSD; }


private:
    void process(cv::Mat& input);
    void filmShader(cv::Mat &image, float time, bool grayscale, float nIntensity, float sIntensity, float sCount);
    void screenDoorShader(cv::Mat& image, float resolution, float opacity);
};

#endif

