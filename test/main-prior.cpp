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




#include <iostream>
#include <fstream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <boost/program_options.hpp>
#include <boost/numeric/ublas/lu.hpp>

#include <list>

template<typename T>
std::vector<T> polyfit( const std::vector<T>& oX, const std::vector<T>& oY, int nDegree ) {
	using namespace boost::numeric::ublas;
 
	if ( oX.size() != oY.size() )
		throw std::invalid_argument( "X and Y vector sizes do not match" );
 
	// more intuative this way
	nDegree++;
	
	size_t nCount =  oX.size();
	matrix<T> oXMatrix( nCount, nDegree );
	matrix<T> oYMatrix( nCount, 1 );
	
	// copy y matrix
	for ( size_t i = 0; i < nCount; i++ )
	{
		oYMatrix(i, 0) = oY[i];
	}
 
	// create the X matrix
	for ( size_t nRow = 0; nRow < nCount; nRow++ )
	{
		T nVal = 1.0f;
		for ( int nCol = 0; nCol < nDegree; nCol++ )
		{
			oXMatrix(nRow, nCol) = nVal;
			nVal *= oX[nRow];
		}
	}
 
	// transpose X matrix
	matrix<T> oXtMatrix( trans(oXMatrix) );
	// multiply transposed X matrix with X matrix
	matrix<T> oXtXMatrix( prec_prod(oXtMatrix, oXMatrix) );
	// multiply transposed X matrix with Y matrix
	matrix<T> oXtYMatrix( prec_prod(oXtMatrix, oYMatrix) );
 
	// lu decomposition
	permutation_matrix<T> pert(oXtXMatrix.size1());
	const std::size_t singular = lu_factorize(oXtXMatrix, pert);
	// must be singular
	if( singular != 0 ) {
		// then the regression cannot be done... return an empty vector
		return std::vector<T>();
	}
 
	// backsubstitution
	lu_substitute(oXtXMatrix, pert, oXtYMatrix);
 
	// copy the result to coeff
	return std::vector<T>( oXtYMatrix.data().begin(), oXtYMatrix.data().end() );
}


float simpleShape(cv::Mat &image) {
	if(image.empty()) return 0;

	cv::Mat gray;
	cv::cvtColor(image, gray, CV_BGR2GRAY);

	cv::Mat edges;
	cv::Canny(gray, edges, 200, 400);

	cv::Mat result(image.size(), CV_8UC3, cv::Scalar(0,0,0));

	std::vector<std::vector<cv::Point> > contours0;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
    cv::findContours( edges, contours0, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	std::cout << contours0.size() << " ";

	cv::drawContours( result, contours, 3, cv::Scalar(128,0,255),
                  3, cv::LINE_AA, hierarchy, std::abs(3) );

    contours.resize(contours0.size());
    for( size_t k = 0; k < contours0.size(); k++ )
        cv::approxPolyDP(cv::Mat(contours0[k]), contours[k], 1, true);
		

	cv::drawContours( result, contours, 3, cv::Scalar(128,255,255),
                  3, cv::LINE_AA, hierarchy, std::abs(3) );

	cv::imshow("result", edges);
	cv::waitKey();

	return 0;
}



float  horizonLine(cv::Mat &image) {
	if(image.empty()) return 0;


	std::vector<float> x, y;

	cv::Mat gray;
	cv::cvtColor(image, gray, CV_BGR2GRAY);

	cv::Mat edges;
	cv::Canny(gray, edges, 110, 220);

	std::vector<cv::Vec4i> lines;
    cv::HoughLinesP( edges, lines, 1, CV_PI/2, 0, 30, 1 );
    float meanY = 0; int nbLines = 0;
    float meanY2 = 0;
    for( size_t i = 0; i < lines.size(); i++ )
    {
    	if(lines[i][0] == lines[i][2]) continue;

		if(lines[i][1] > (.7 * image.rows)) continue;
		if(lines[i][1] < (.3 * image.rows)) continue;

        cv::line( image, cv::Point(lines[i][0], lines[i][1]),
            cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0,0,255), 3, 8 );

        x.push_back(lines[i][0]);
		x.push_back(lines[i][2]);
		y.push_back(lines[i][1]);
		y.push_back(lines[i][3]);

		meanY += lines[i][1]; ++nbLines;
		meanY += lines[i][3]; ++nbLines;

		meanY2 += lines[i][1] * lines[i][1];
		meanY2 += lines[i][3] * lines[i][3];

    }
    meanY /= nbLines;
    meanY2 /= nbLines;

    float stdev = std::sqrt(meanY2 - meanY*meanY);
    float meanValue = meanY;
    
    meanY = 0; nbLines = 0;
    for( size_t i = 0; i < lines.size(); i++ )
    {
    	if(lines[i][0] == lines[i][2]) continue;

		if(lines[i][1] > (.7 * image.rows)) continue;
		if(lines[i][1] < (.3 * image.rows)) continue;

        if(std::abs(lines[i][1] - meanValue) > 1.3 * stdev) continue;

		meanY += lines[i][1]; ++nbLines;
		meanY += lines[i][3]; ++nbLines;

    }
    meanY /= nbLines;



    if(nbLines == 0) {
    	meanY = image.rows/2;
    } else {
    	std::vector<float> model = polyfit<float>(x, y, 1);

	    if(std::abs(model[0]- image.rows/2) < std::abs(meanY - image.rows/2))
	    	meanY = model[0];
    }


	// cv::line(image, cv::Point_<int>(0, meanY), cv::Point_<int>(image.cols, meanY), cv::Scalar( 0, 255, 0 ), 2);



return meanY;


}


float faceLine(cv::Mat &image) {
	cv::Mat gray;
	cv::cvtColor(image, gray, CV_BGR2GRAY);

	cv::CascadeClassifier face_cascade;
	cv::CascadeClassifier faceProfil_cascade;
	bool faceCascadeEnabled = false;
	bool faceProfilCascadeEnabled = false;

	// ------------------------------------------------------------------------------------------------
	// init haar cascades framework

	if(face_cascade.load("haarcascade_frontalface_alt.xml")) {
		faceCascadeEnabled = true;
	} else {
		std::cerr<< "[I] cannot open: haarcascade_frontalface_alt.xml" << std::endl;
	}

	if(faceProfil_cascade.load("haarcascade_profileface.xml")) {
		faceProfilCascadeEnabled = true;
	} else {
		std::cerr<< "[I] cannot open: haarcascade_profileface.xml" << std::endl;
	}


	// ------------------------------------------------------------------------------------------------
	// find features

	std::list<cv::Rect> allFeatures;
	std::vector<cv::Rect> faceFeatures;
	if(faceCascadeEnabled)
		face_cascade.detectMultiScale( gray, faceFeatures, 2, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(15, 15) );

	for(size_t i = 0 ; i < faceFeatures.size() ; ++i) {
		cv::rectangle(image, faceFeatures[i], cv::Scalar( 0, 0, 255 ));
		allFeatures.push_back(faceFeatures[i]);
	}

	faceFeatures.clear();
	if(faceProfilCascadeEnabled)
		faceProfil_cascade.detectMultiScale( gray, faceFeatures, 2, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(15, 15) );

	for(size_t i = 0 ; i < faceFeatures.size() ; ++i) {
		cv::rectangle(image, faceFeatures[i], cv::Scalar( 255, 255, 0 ));
		allFeatures.push_back(faceFeatures[i]);
	}

	if(allFeatures.size() < 2) return -1;


	float mnX = image.cols;
	float mxX = 0;
	for( std::list<cv::Rect>::iterator it = allFeatures.begin() ; it != allFeatures.end() ; ++it ) {
		float x = it->x + it->width /2;

		if(x > mxX) mxX = x;
		if(x < mnX) mnX = x;
	}

	if((mxX-mnX)/image.cols < 0.2) return -1;	// if the detected faces are collocated in one area, then skip it.

	// ------------------------------------------------------------------------------------------------
	// find equatorial line



	float meanY = 0; int nbLines = 0;
    float meanY2 = 0;
    for( std::list<cv::Rect>::iterator it = allFeatures.begin() ; it != allFeatures.end() ; ++it ) {
    	meanY += it->y + it->height /2;
    	meanY2 += (it->y + it->height /2)*(it->y + it->height /2);
    	++nbLines;
    }

    if(nbLines == 0) return -1;

    meanY /= nbLines;
    meanY2 /= nbLines;


    float stdev = std::sqrt(meanY2 - meanY*meanY);
    float meanValue = meanY;
    
    meanY = 0; nbLines = 0;

    for( std::list<cv::Rect>::iterator it = allFeatures.begin() ; it != allFeatures.end() ; ++it ) {
    	float y = it->y + it->height /2;

    	if(std::abs(y-meanValue) > 1.3 * stdev) continue;

    	meanY += y;
    	++nbLines;
    }

    if(nbLines == 0) return meanValue;


	meanY /= nbLines;

	if(meanY < (gray.rows * 1 / 3) && allFeatures.size() < 2) { // we need at least two faces to justify an equatorial line lower higher the first third of the image
		return -1; 
	}


	return meanY;


}

float percentageSaliency(const cv::Mat& image, float slicePosition, float sliceHeightDegree = 10.f) {
	
	double sumImage = cv::sum( image )[0];
	float sumSlice = 0;

	float sliceHeight = sliceHeightDegree * (image.rows / 180);

	for(int i = static_cast<int>(std::max(slicePosition - sliceHeight, 0.f)) ; i < std::min(static_cast<int>(slicePosition + sliceHeight), image.rows) ; ++i) {
		for(int j = 0 ; j < image.cols ; ++j) {
			sumSlice += image.at<float>(i,j);
		}
	}

	return sumSlice / sumImage;

}


float salientCenter(const cv::Mat& image, int step = 5) {
	
	std::vector<float> histogram(180/step);
	int offset = 0; //static_cast<int>(30.f*(step/180.f));

	float sumHist = 0;
	for(int i = offset ; i < 180-offset ; i+=step) {
		float top    = image.rows*static_cast<float>(i)/180;
		float bottom = image.rows*static_cast<float>(i+step)/180;

		float sum = 0;
		for(int ii = top ; ii < bottom ; ++ii) {
			for(int jj = 0 ; jj < image.cols ; ++jj) {
				sum += image.at<float>(ii,jj);
			}
		}

		sum /= (bottom-top) * image.cols;
		histogram[i/step] = sum;
		sumHist += sum;
	}

	float loc = 0;
	for(size_t i = offset ; i < histogram.size() - offset ; ++i) {
		loc += i*step*histogram[i]/sumHist;
	}

	return image.rows * static_cast<float>(loc)/180.f;

}


void applyGaussianEquatorialPrior(cv::Mat& image, float gaussianM = 0.f, float gaussianSD = 500.f, bool central = false) {
	
	for(int i = 0 ; i < image.rows ; ++i) {
		float lat = (.5f - static_cast<float>(i)/image.rows) * 180.f;
		float prior = 0.f;

		if(central)
			prior =  (.01 + .40 * exp(-((lat-gaussianM)*(lat-gaussianM))/(gaussianSD))) / .41f;
		else
			prior =  (.01 + .40 * exp(-((lat-gaussianM)*(lat-gaussianM))/(gaussianSD)) + .75 * exp(-((lat-100)*(lat-100))/(200)) + .75 * exp(-((lat+90)*(lat+90))/(200))) / .41f;

		prior = std::min(1.f, prior);

		for(int j = 0 ; j < image.cols ; ++j) {
			image.at<float>(i,j) = prior * image.at<float>(i,j);
		}
	}	

}


void applyEquatorialPrior(cv::Mat& image, cv::Mat& colorImage, bool central = false, bool print = false) {
	float scaling_factor = static_cast<float>(image.cols) / 1400.f;	// normalize the size of the images
	
	cv::resize(colorImage, colorImage, cv::Size(colorImage.size().width/scaling_factor, colorImage.size().height/scaling_factor));
	float hz = colorImage.rows / 2;
	float fc = faceLine(colorImage);


	float equatorialLine = hz;


	if(fc > 0) {
		if(fc > colorImage.rows / 3)	// prior: faces are not higher than the first third of the image
			equatorialLine = fc;
	}

	float slCenter = salientCenter(image);

	slCenter = std::max(std::min((slCenter - image.rows/2)/image.rows, 0.1f), -0.1f);
	slCenter = slCenter * image.rows + image.rows/2;
	slCenter = (slCenter/scaling_factor + colorImage.rows / 2) / 2;
	

	if(fc > 0) {
		if(fc > colorImage.rows / 3)	// prior: faces are not higher than the first third of the image
			slCenter = fc;
	}


	equatorialLine = (.5f - slCenter / colorImage.rows) * 180.f;

	if(print) std::cout << equatorialLine << std::endl;

	applyGaussianEquatorialPrior(image, equatorialLine, 700.f, central);

}


void bottomThresholdSaliency(cv::Mat &saliency) {
	cv::Mat uCharSalmap;
	uCharSalmap = 255 * saliency;
	uCharSalmap.convertTo(uCharSalmap, CV_8UC1);

	int histSize[] = {255};
	int channels[] = {0};
	float range[] = { 0, 256 } ;
 	const float* histRange = { range };
	cv::Mat hist;

	cv::calcHist(&uCharSalmap, 1, channels, cv::Mat(), hist, 1, histSize, &histRange, true, true);

	float sum = 0;
	float mid = static_cast<float>(saliency.cols*saliency.rows) * .7;
	int theshold = 0;
	for(int i = 0 ; i < hist.rows ; ++i) {
		sum += hist.at<float>(i,0);

		if(sum > mid) {
			theshold = i;
			break;
		}
	}

	for(int i = 0 ; i < uCharSalmap.rows ; ++i) {
		for(int j = 0 ; j < uCharSalmap.cols ; ++j) {
			if(uCharSalmap.at<unsigned char>(i,j) < theshold) {
				uCharSalmap.at<unsigned char>(i,j) = theshold;
			}
		}
	}

	uCharSalmap.convertTo(saliency, CV_32FC1);
	uCharSalmap /= 255;

}



int main(int argc, char **argv) {


	// ------------------------------------------------------------------------------------------------------------------------------------------
	// parse parameters

	namespace po = boost::program_options;

	po::positional_options_description p;
	p.add("input-file", -1);

	po::options_description desc("Allowed options");
	desc.add_options()
			("help", "produce help message")
			("input-file,i", po::value< std::string >(), "Saliency input image.")
			("color-file,c", po::value< std::string >(), "Color image corresponding to the saliency map.")
			("line,l", po::value< float >(), "Add the ground truth equatorial line (in degree [-90,+90]).")
			("output-file,o", po::value< std::string >(), "Saliency map with the equatorial prior.")
			("central", "Consider only the central part of the Gaussian mixture")
			("no-adapt", "No adaptation")
			("print-horizon", "Output in the terminal the position of the horizon line")
	;


	po::variables_map vm;
	try {
		po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
		po::notify(vm);
	} catch(boost::exception &) {
		std::cerr << "Error incorect program options. See --help... \n";
		return 0;
	}

	if (vm.count("help")) {
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "\t\tApply the equatorial prior on saliency maps\n";
		std::cout << "--------------------------------------------------------------------------------\n";
		std::cout << "\n\n";
		std::cout << desc << "\n";
		return 1;
	}

	
	std::string inputImagePath;
	std::string inputColorImagePath;
	std::string outputPath;

	bool central = false;
	bool noAdapt = false;
	
	if (vm.count("input-file")) {
		inputImagePath = vm["input-file"].as< std::string >();
	} else {
		std::cerr << "It is required to provide the an input saliency map. See --help\n";
		return 0;
	}

	if (vm.count("color-file")) {
		inputColorImagePath = vm["color-file"].as< std::string >();
	} else {
		std::cerr << "It is required to provide the an input color image. See --help\n";
		return 0;
	}

	if (vm.count("output-file")) {
		outputPath = vm["output-file"].as< std::string >();
	} 

	if(vm.count("central")) {
		central = true;
	}

	if(vm.count("no-adapt")) {
		noAdapt = true;
	}

	bool printHz = false;
	if(vm.count("print-horizon")) {
		printHz = true;
	}


	// ---------------------------------------------------------------------------------------------------
	// Get statistics

	cv::Mat image = cv::imread(inputImagePath);
	if(image.empty()) {
		std::cerr << "Cannot open: " << inputImagePath << std::endl;
		return 0;
	}

	cv::Mat colorImage = cv::imread(inputColorImagePath);
	if(colorImage.empty()) {
		std::cerr << "Cannot open: " << inputColorImagePath << std::endl;
		return 0;
	}




	cv::cvtColor(image, image, CV_BGR2GRAY);

	image.convertTo(image, CV_32FC1);
	image /= 255;


	if(vm.count("second-file") && vm.count("weight")) {
		cv::Mat imageW = cv::imread(vm["second-file"].as< std::string >());
		if(imageW.empty()) {
			std::cerr << "cannot open: " << vm["second-file"].as< std::string >() << std::endl ;
		} else {
			cv::cvtColor(imageW, imageW, CV_BGR2GRAY);
			imageW.convertTo(imageW, CV_32FC1);
			imageW /= 255;

			float w = vm["weight"].as< float >();
			image = image * w  + (1 - w) * imageW;
		}
	}

	// image = cv::Mat(image.size(), CV_32FC1, cv::Scalar(1.0f));

	if(printHz) std::cout << inputColorImagePath << ", ";

	if(!noAdapt)
		applyEquatorialPrior(image, colorImage, central, printHz);
	else
		applyGaussianEquatorialPrior(image, 0, 700, central);

	// bottomThresholdSaliency(image);

	// if (vm.count("line")) {
	// 	float line = vm["line"].as< float >();


	// 	line =  image.rows * (90 - line) / 180.f ;

	// 	// std::cout << line << std::endl;
	// applyGaussianEquatorialPrior(image, 0, 500);


	double mn, mx;
	cv::minMaxLoc(image, &mn, &mx);

	image = (image - mn) / (mx - mn);

	// 	// line =  colorImage.rows * (90 - line) / 180.f ;

	// 	// if(line > 0 && line < colorImage.rows)
	// 	// 	cv::line(colorImage, cv::Point_<int>(0, line), cv::Point_<int>(colorImage.cols, line), cv::Scalar( 255, 0, 255), 2);
	// } 

	// cv::imshow("colorImage", colorImage);
	// cv::waitKey();

	if(!outputPath.empty()) {
		cv::imwrite(outputPath, 255*image);
		return 0;
	} else {
		cv::imshow("with equatorial prior", image);
		cv::waitKey();
	}


	return 0;
}

