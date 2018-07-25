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




#include "HMDSim.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>


HMDSim::HMDSim() {
    m_renderW = 1280;
    m_renderH = 800;

    paramBuilder(CV1);
}

void HMDSim::paramBuilder(HMD hmd) {
    m_nIntensity = 0.5f;
    m_sIntensity = 0.2f;
    m_time = 0.f;
    m_enableSD = true;
    m_blurKernelSize = 0;
    m_blurKernelSigma = 0.f;

    switch(hmd) {
        case DK1:
            m_sCount = 600;
            m_resolution = 2;
            m_opacity = 0.05f;
            m_resScaling = .5f;
            break;

        case DK2:
            m_sCount = 400;
            m_resolution = 8.f;
            m_opacity = 0.03f;
            m_resScaling = .67f;
            break;

        case CV1:
            m_sCount = 800;
            m_resolution =  4.f;
            m_opacity = 0.03f;
            m_resScaling = 0.5f;
            break;

        case FK:
            m_sCount = 2400;
            m_resolution = 1.0f;
            m_nIntensity = 0.f;
            m_sIntensity = 0.f;
            m_opacity = 0.06f;
            m_resScaling = 1.35;
            m_enableSD = false;

    }
}


void HMDSim::applyFilter(const cv::Mat& input, cv::Mat& result) {
    input.convertTo(result, CV_32FC3);
    result /= 255;

    double mxO, mnO;
    cv::minMaxLoc(result, &mnO, &mxO);


    // add blur.
    if(m_blurKernelSize > 0 && m_blurKernelSigma > 0)
        cv::GaussianBlur(result, result, cv::Size(m_blurKernelSize, m_blurKernelSize), m_blurKernelSigma, m_blurKernelSigma, cv::BORDER_REFLECT);
    

    int windowH = static_cast<int>(m_renderH * m_resScaling);
    int windowW = static_cast<int>(m_renderW * m_resScaling);

    // create an image which size is a multiple of windowW x windowH
    cv::Mat paddedInput(static_cast<int>(std::ceil(static_cast<float>(input.rows) / windowH)) * windowH, 
                        static_cast<int>(std::ceil(static_cast<float>(input.cols) / windowW)) * windowW,
                        CV_32FC3,
                        cv::Scalar(0,0,0));

    for(int i = 0 ; i < result.rows ; ++i) {
        for(int j = 0 ; j < result.cols ; ++j) {
            paddedInput.at<cv::Point3f>(i, j) = result.at<cv::Point3f>(i,j);
        }
    }

    // Do the processing by block of windowW x windowH to nomalize the effect of the filters. 
    for(int wi = 0 ; wi < static_cast<int>(paddedInput.rows / windowH) ; ++wi) {
        for(int wj = 0 ; wj < static_cast<int>(paddedInput.cols / windowW) ; ++wj) {

            cv::Mat study(windowH, windowW, CV_32FC3);
            for(int i = 0 ; i < windowH ; ++i) {
                for(int j = 0 ; j < windowW ; ++j) {
                    study.at<cv::Point3f>(i,j) = paddedInput.at<cv::Point3f>(i+wi*windowH, j+wj*windowW);
                }
            }

            process(study);

            for(int i = 0 ; i < windowH ; ++i) {
                for(int j = 0 ; j < windowW ; ++j) {
                    paddedInput.at<cv::Point3f>(i+wi*windowH, j+wj*windowW) = study.at<cv::Point3f>(i,j);
                }
            }
        }
    }

    // Retrieve the image
    for(int i = 0 ; i < result.rows ; ++i) {
        for(int j = 0 ; j < result.cols ; ++j) {
            result.at<cv::Point3f>(i,j) = paddedInput.at<cv::Point3f>(i, j);
        }
    }


    // Make sure that the gamma is the same at the end of the processing as the input image.
    double mxE, mnE;
    cv::minMaxLoc(result, &mnE, &mxE);

    result = ((result - mnE) / (mxE-mnE)) * (mxO - mnO) + mnO;

}

void HMDSim::process(cv::Mat& result) {

    filmShader(result, m_time, false, m_nIntensity, m_sIntensity, m_sCount);

    if(m_enableSD) 
        screenDoorShader(result, m_resolution, m_opacity);

}


/**
 * @author alteredq / http://alteredqualia.com/
 *
 * Film grain & scanlines shader
 *
 * - ported from HLSL to WebGL / GLSL
 * http://www.truevision3d.com/forums/showcase/staticnoise_colorblackwhite_scanline_shaders-t18698.0.html
 *
 * Screen Space Static Postprocessor
 *
 * Produces an analogue noise overlay similar to a film grain / TV static
 *
 * Original implementation and noise algorithm
 * Pat 'Hawthorne' Shearon
 *
 * Optimized scanlines + noise version with intensity scaling
 * Georg 'Leviathan' Steinrohder
 *
 * This version is provided under a Creative Commons Attribution 3.0 License
 * http://creativecommons.org/licenses/by/3.0/
 */


void HMDSim::filmShader(cv::Mat &image, float time, bool grayscale, float nIntensity, float sIntensity, float sCount) {

    for(int i = 0 ; i < image.rows ; ++i) {
        for(int j = 0 ; j < image.cols ; ++j) {
            cv::Point3_<float> &pt = image.at< cv::Point3_<float> >(i,j);

            float vUv_y = static_cast<float>(i)/image.rows;
            float vUv_x = static_cast<float>(j)/image.cols;


            float x = vUv_x * vUv_y * time * 1000.0f;
            x = static_cast<float>( static_cast<int>(x) % 13 *  static_cast<int>(x) % 130 );
            float dx = x - 100*static_cast<int>(x / 100); 

            cv::Point3_<float> res = pt + pt * std::max(0.f, std::min(1.0f, 0.1f * dx * 100.0f));
            float sc_x = std::sin(vUv_y * sCount);
            float sc_y = std::cos(vUv_y * sCount); 

            res.x += pt.x * sc_x * sIntensity;
            res.y += pt.y * sc_y * sIntensity;
            res.z += pt.z * sc_x * sIntensity;

            float clampIntensity = std::max(0.f, std::min(1.f, nIntensity));

            res.x = pt.x + clampIntensity * (res.x - pt.x);
            res.y = pt.y + clampIntensity * (res.y - pt.y);
            res.z = pt.z + clampIntensity * (res.z - pt.z);

            if(grayscale) {
                res.x = res.z * .3f + res.y * .59f + res.x * .11f;
                res.y = res.x;
                res.z = res.x;
            }

            pt = res;
        }
    }
}




void HMDSim::screenDoorShader(cv::Mat& image, float resolution, float opacity) {


    for(int i = 0 ; i < image.rows ; ++i) {
        for(int j = 0 ; j < image.cols ; ++j) {
            cv::Point3_<float> &pt = image.at< cv::Point3_<float> >(i,j);

            float gl_FragCoord_x = static_cast<float>(j) + .5f;
            float gl_FragCoord_y = static_cast<float>(i) + .5f;

            float delta = (gl_FragCoord_x - static_cast<int>(gl_FragCoord_x/resolution)*resolution) < 1.0f ? opacity : 0.0f;
            pt.x -= delta; pt.y -= delta; pt.z -= delta;

            delta = (gl_FragCoord_y - static_cast<int>(gl_FragCoord_y/resolution)*resolution) < 1.0f ? opacity : 0.0f;
            pt.x -= delta; pt.y -= delta; pt.z -= delta;
        }
    }

}







