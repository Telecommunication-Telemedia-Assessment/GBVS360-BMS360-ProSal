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

