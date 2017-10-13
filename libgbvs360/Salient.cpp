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



#include "Salient.h"
#include "Options.h"
#include "Projection.h"
#include "BMSSaliency.h"
#include "GBVSSaliency.h"
#include "Saliency360.h"


// ---------------------------------------------------------------------------------------------------------------------------------------
// Forward declaration


void pointerToCVMat(const unsigned char *imgRGB, int cols, int rows, cv::Mat &img);
void cvMatToPointer(const cv::Mat &img, float *imgRGB);





// ---------------------------------------------------------------------------------------------------------------------------------------
// Model function



/* Model type 1: Head saliency map
* \param imgRGB: input equirectangular image organised first by color (order RGB), then by row and then by column.  (raster scan order)
* \param cols: number of column in the input image
* \param rows: number of lines in the input image
* \param saliency: output saliency map of the model. It has the same size as the input image. It is ordered by row and then by column.  (raster scan order)
* \param nbThreads: number of threads used by the model.
*/
void computeHS(const unsigned char *imgRGB, int cols, int rows, float *saliency, int nbThreads) {
    cv::Mat input;
    pointerToCVMat(imgRGB, cols, rows, input);

    Option::threads = nbThreads;

    cv::Mat salmap;
    Saliency360 saliency360;
    saliency360.model = 1;
    saliency360.equatorialPrior = true;
    saliency360.estimate(input, salmap);


    cvMatToPointer(salmap, saliency);
}





/* Model type 2: Head/Eye saliency map
* \param imgRGB an input equirectangular image organised first by color (order RGB), then by row and then by column. (raster scan order)
* \param cols: number of column in the input image
* \param rows: number of lines in the input image
* \param saliency: output saliency map of the model. It has the same size as the input image. It is ordered by row and then by column.  (raster scan order)
* \param nbThreads: number of threads used by the model.
*/
void computeHES(const unsigned char *imgRGB, int cols, int rows, float *saliency, int nbThreads) {

    cv::Mat input;
    pointerToCVMat(imgRGB, cols, rows, input);

    Option::threads = nbThreads;

    cv::Mat salmap;
    Saliency360 saliency360;
    saliency360.model = 4;
    saliency360.equatorialPrior = true;
    saliency360.estimate(input, salmap);


    cvMatToPointer(salmap, saliency);

}





/* Model type 3: Scan path prediction
* \param imgRGB an input equirectangular image organised first by color (order RGB), then by row and then by column.  (raster scan order)
* \param cols: number of column in the input image
* \param rows: number of lines in the input image
* \param scanPath: output scan path of the model. [4,N] matrix containing the (x,y,Tst,Tend).   (raster scan order)
* \param n: number of reppetition of the model (number of modelled users).
* \param nbThreads: number of threads used by the model.
*/

void computeScanPath    (const unsigned char *imgRGB, int cols, int rows, float *scanPath, int n, int nbThreads) {

    cv::Mat input;
    pointerToCVMat(imgRGB, cols, rows, input);

    Option::threads = nbThreads;
    Option::experimentRepppetition = n;


    cv::Mat scanPathMat;
    Saliency360 saliency360;
    saliency360.model = 3;
    saliency360.equatorialPrior = true;
    saliency360.estimate(input, scanPathMat);


    cvMatToPointer(scanPathMat, scanPath);

}






// ---------------------------------------------------------------------------------------------------------------------------------------
// Utility functions


void pointerToCVMat(const unsigned char *imgRGB, int cols, int rows, cv::Mat &img) {
    img = cv::Mat(rows, cols, CV_8UC3);

    int gIdx = 0;
    for(int i = 0 ; i < rows ; ++i) {
        for(int j = 0 ; j < cols ; ++j) {
            cv::Point3_<unsigned char> &pt = img.at< cv::Point3_<unsigned char> >(i, j);
            pt.z = imgRGB[gIdx]; ++gIdx;
            pt.y = imgRGB[gIdx]; ++gIdx;
            pt.x = imgRGB[gIdx]; ++gIdx;
        }
    }
}



void cvMatToPointer(const cv::Mat &img, float *saliency) {

    int gIdx = 0;
    for(int i = 0 ; i < img.cols ; ++i) {
        for(int j = 0 ; j < img.rows ; ++j) {
            saliency[gIdx] = img.at<float >(i, j);
            ++gIdx;
        }
    }
}






