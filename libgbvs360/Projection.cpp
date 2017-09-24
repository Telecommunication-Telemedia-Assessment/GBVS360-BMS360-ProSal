#include "Projection.h"


#include <exception>


#include "Options.h"
#include <gnomonic-all.h>
#include "common-method.h"

Projection::Projection() {
    /* Image initialization variables */
    nrRed = 0;
    nrGreen = 0;
    nrBlue = 0;

    /* Projection float variables */
    nrApper  = 0.0;
    nrSightX = 0.0;
    nrSightY = 0.0;
    nrAzim   = 0.0;
    nrHead   = 0.0;
    nrElev   = 0.0;
    nrRoll   = 0.0;
    nrFocal  = 0.0;
    nrPixel  = 0.0;

    /* Projection integer variables */
    nrrWidth   = 0;
    nrrHeight  = 0;
    nrmWidth   = 0;
    nreHeight  = 0;
    nreWidth   = 0;
    nrmHeight  = 0;
    nrmCornerX = 0;
    nrmCornerY = 0;

    /* Exportation options variables */
    nrOption = -1;

    /* Parallel processing variables */
    nrThread = 1;

    /* projection method */
    projMethod = 3;
}

void Projection::equirectangularToRectilinear(const cv::Mat& inputImage, cv::Mat& nroImage) {
	
	if(nroImage.cols == 0 || nroImage.rows == 0) {
		throw std::logic_error(std::string("Projection::equirectangularToRectilinear : The output image was not allocated. Maybe you did not provided the size of the output image. Cannot continue.")); 
		return;
	}

	switch(projMethod) {
		case 0:
			lg_ttg_genericp(
                ( inter_C8_t * ) inputImage.data,
                inputImage.cols,
                inputImage.rows,
                inputImage.channels(),
                ( inter_C8_t * ) nroImage.data,
                nroImage.cols,
                nroImage.rows,
                nroImage.channels(),
                nrSightX,
                nrSightY,
                nrmWidth,
                nrmHeight,
                nrmCornerX,
                nrmCornerY,
                nrAzim  * ( LG_PI / 180.0 ),
                nrElev  * ( LG_PI / 180.0 ),
                nrRoll  * ( LG_PI / 180.0 ),
                nrFocal,
                nrPixel,
                lc_method( nrMethod.empty() ? "bicubicf" : nrMethod.c_str() ),
                nrThread
            );
            break;

        case 1:
        	lg_ttg_elphelp(
                 ( inter_C8_t * ) inputImage.data,
                inputImage.cols,
                inputImage.rows,
                inputImage.channels(),
                ( inter_C8_t * ) nroImage.data,
                nroImage.cols,
                nroImage.rows,
                nroImage.channels(),
                nrSightX,
                nrSightY,
                nrmWidth,
                nrmHeight,
                nrmCornerX,
                nrmCornerY,
                nrRoll * ( LG_PI / 180.0 ),
                nrAzim * ( LG_PI / 180.0 ),
                nrElev * ( LG_PI / 180.0 ),
                nrHead * ( LG_PI / 180.0 ),
                nrPixel,
                nrFocal,
                lc_method( nrMethod.empty() ? "bicubicf" : nrMethod.c_str() ),
                nrThread

            );
        break;

        case 2:
			lg_ttg_centerp(
                 ( inter_C8_t * ) inputImage.data,
                inputImage.cols,
                inputImage.rows,
                inputImage.channels(),
                ( inter_C8_t * ) nroImage.data,
                nroImage.cols,
                nroImage.rows,
                nroImage.channels(),
                nrmWidth,
                nrmHeight,
                nrmCornerX,
                nrmCornerY,
                nrAzim * ( LG_PI / 180.0 ),
                nrElev * ( LG_PI / 180.0 ),
                nrRoll * ( LG_PI / 180.0 ),
                nrFocal,
                nrPixel,
                lc_method( nrMethod.empty() ? "bicubicf" : nrMethod.c_str() ),
                nrThread

            );
        break;


        case 3:
	        lg_etg_apperturep( 
 				( inter_C8_t * ) inputImage.data,
                inputImage.cols,
                inputImage.rows,
                inputImage.channels(),
                ( inter_C8_t * ) nroImage.data,
                nroImage.cols,
                nroImage.rows,
                nroImage.channels(),
                nrAzim  * ( LG_PI / 180.0 ),
                nrElev  * ( LG_PI / 180.0 ),
                nrRoll  * ( LG_PI / 180.0 ),
                nrApper * ( LG_PI / 180.0 ),
                lc_method( nrMethod.empty() ? "bicubicf" : nrMethod.c_str() ),
                nrThread

            );
         break;

	}

}

void Projection::equirectangularToRectilinear(const cv::Mat& inputImage, cv::Mat& output, float azim, float elev, float roll) {

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
        nrApper * ( LG_PI / 180.0 ),
        lc_method( nrMethod.empty() ? "bicubicf" : nrMethod.c_str() ),
        nrThread

    );
}



void Projection::rectilinearToEquirectangular(const cv::Mat& inputImage, cv::Mat& output) {
    if(output.cols == 0 || output.rows == 0) {
        throw std::logic_error(std::string("Projection::rectilinearToEquirectangular : The output image was not allocated. Maybe you did not provided the size of the output image. Cannot continue.")); 
        return;
    }

    switch(projMethod) {
        case 0:
            lg_gtt_genericp(
                ( inter_C8_t * ) output.data,
                output.cols,
                output.rows,
                output.channels(),
                ( inter_C8_t * ) inputImage.data,
                inputImage.cols,
                inputImage.rows,
                inputImage.channels(),
                nrSightX,
                nrSightY,
                nrmWidth,
                nrmHeight,
                nrmCornerX,
                nrmCornerY,
                nrAzim  * ( LG_PI / 180.0 ),
                nrElev  * ( LG_PI / 180.0 ),
                nrRoll  * ( LG_PI / 180.0 ),
                nrFocal,
                nrPixel,
                lc_method( nrMethod.empty() ? "bicubicf" : nrMethod.c_str() ),
                nrThread
            );
            break;

        case 1:
            lg_gtt_elphelp(
                ( inter_C8_t * ) output.data,
                output.cols,
                output.rows,
                output.channels(),
                ( inter_C8_t * ) inputImage.data,
                inputImage.cols,
                inputImage.rows,
                inputImage.channels(),
                nrSightX,
                nrSightY,
                nrmWidth,
                nrmHeight,
                nrmCornerX,
                nrmCornerY,
                nrRoll * ( LG_PI / 180.0 ),
                nrAzim * ( LG_PI / 180.0 ),
                nrElev * ( LG_PI / 180.0 ),
                nrHead * ( LG_PI / 180.0 ),
                nrPixel,
                nrFocal,
                lc_method( nrMethod.empty() ? "bicubicf" : nrMethod.c_str() ),
                nrThread

            );
        break;

        case 2:
            lg_gtt_centerp(
                ( inter_C8_t * ) output.data,
                output.cols,
                output.rows,
                output.channels(),
                ( inter_C8_t * ) inputImage.data,
                inputImage.cols,
                inputImage.rows,
                inputImage.channels(),
                nrmWidth,
                nrmHeight,
                nrmCornerX,
                nrmCornerY,
                nrAzim * ( LG_PI / 180.0 ),
                nrElev * ( LG_PI / 180.0 ),
                nrRoll * ( LG_PI / 180.0 ),
                nrFocal,
                nrPixel,
                lc_method( nrMethod.empty() ? "bicubicf" : nrMethod.c_str() ),
                nrThread

            );
        break;


        case 3:
            lg_gte_apperturep( 
                ( inter_C8_t * ) output.data,
                output.cols,
                output.rows,
                output.channels(),
                ( inter_C8_t * ) inputImage.data,
                inputImage.cols,
                inputImage.rows,
                inputImage.channels(),
                nrAzim  * ( LG_PI / 180.0 ),
                nrElev  * ( LG_PI / 180.0 ),
                nrRoll  * ( LG_PI / 180.0 ),
                nrApper * ( LG_PI / 180.0 ),
                lc_method( nrMethod.empty() ? "bicubicf" : nrMethod.c_str() ),
                nrThread

            );
         break;

    }
}


void Projection::rectilinearToEquirectangular(const cv::Mat& inputImage, cv::Mat& output, float azim, float elev, float roll) {
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
        nrApper * ( LG_PI / 180.0 ),
        lc_method( nrMethod.empty() ? "bicubicf" : nrMethod.c_str() ),
        nrThread

    );
}



void Projection::rectilinearToEquirectangularFC3(const cv::Mat& inputImage, cv::Mat& output) {
    if(output.cols == 0 || output.rows == 0) {
        throw std::logic_error(std::string("Projection::rectilinearToEquirectangular : The output image was not allocated. Maybe you did not provided the size of the output image. Cannot continue.")); 
        return;
    }


    switch(projMethod) {
        case 0:
            lg_gtt_genericp_f(
                ( float * ) output.data,
                output.cols,
                output.rows,
                output.channels(),
                ( float * ) inputImage.data,
                inputImage.cols,
                inputImage.rows,
                inputImage.channels(),
                nrSightX,
                nrSightY,
                nrmWidth,
                nrmHeight,
                nrmCornerX,
                nrmCornerY,
                nrAzim  * ( LG_PI / 180.0 ),
                nrElev  * ( LG_PI / 180.0 ),
                nrRoll  * ( LG_PI / 180.0 ),
                nrFocal,
                nrPixel,
                lc_method_f( nrMethod.empty() ? "bicubicf" : nrMethod.c_str() ),
                nrThread
            );
            break;

        case 1:
            lg_gtt_elphelp_f(
                ( float * ) output.data,
                output.cols,
                output.rows,
                output.channels(),
                ( float * ) inputImage.data,
                inputImage.cols,
                inputImage.rows,
                inputImage.channels(),
                nrSightX,
                nrSightY,
                nrmWidth,
                nrmHeight,
                nrmCornerX,
                nrmCornerY,
                nrRoll * ( LG_PI / 180.0 ),
                nrAzim * ( LG_PI / 180.0 ),
                nrElev * ( LG_PI / 180.0 ),
                nrHead * ( LG_PI / 180.0 ),
                nrPixel,
                nrFocal,
                lc_method_f( nrMethod.empty() ? "bicubicf" : nrMethod.c_str() ),
                nrThread

            );
        break;

        case 2:
            lg_gtt_centerp_f(
                ( float * ) output.data,
                output.cols,
                output.rows,
                output.channels(),
                ( float * ) inputImage.data,
                inputImage.cols,
                inputImage.rows,
                inputImage.channels(),
                nrmWidth,
                nrmHeight,
                nrmCornerX,
                nrmCornerY,
                nrAzim * ( LG_PI / 180.0 ),
                nrElev * ( LG_PI / 180.0 ),
                nrRoll * ( LG_PI / 180.0 ),
                nrFocal,
                nrPixel,
                lc_method_f( nrMethod.empty() ? "bicubicf" : nrMethod.c_str() ),
                nrThread

            );
        break;


        case 3:
            lg_gte_apperturep_f( 
                ( float * ) output.data,
                output.cols,
                output.rows,
                output.channels(),
                ( float * ) inputImage.data,
                inputImage.cols,
                inputImage.rows,
                inputImage.channels(),
                nrAzim  * ( LG_PI / 180.0 ),
                nrElev  * ( LG_PI / 180.0 ),
                nrRoll  * ( LG_PI / 180.0 ),
                nrApper * ( LG_PI / 180.0 ),
                lc_method_f( nrMethod.empty() ? "bicubicf" : nrMethod.c_str() ),
                nrThread

            );
         break;

    }

    
}

void Projection::rectilinearToEquirectangularFC3(const cv::Mat& inputImage, cv::Mat& output, float azim, float elev, float roll) {
    lg_gte_apperturep_f( 
        ( float * ) output.data,
        output.cols,
        output.rows,
        output.channels(),
        ( float * ) inputImage.data,
        inputImage.cols,
        inputImage.rows,
        inputImage.channels(),
        azim  * ( LG_PI / 180.0 ),
        elev  * ( LG_PI / 180.0 ),
        roll  * ( LG_PI / 180.0 ),
        nrApper * ( LG_PI / 180.0 ),
        lc_method_f( nrMethod.empty() ? "bicubicf" : nrMethod.c_str() ),
        nrThread
    );
}





