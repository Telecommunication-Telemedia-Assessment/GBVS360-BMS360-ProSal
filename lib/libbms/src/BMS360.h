#ifndef _BMS360_
#define _BMS360_

#include "BMS.h"

class BMS360 : public BMS {

public:
    BMS360 (const cv::Mat& src, int dw1, bool nm, bool hb, int colorSpace, bool whitening) : BMS(src, dw1, nm, hb, colorSpace, whitening) {};

protected:
    virtual void bmsNormalize(cv::Mat& mat1, cv::Mat& mat2);

private:
    void scaleBooleanMap(cv::Mat& map);

};

#endif


