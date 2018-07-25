#include "VPD.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <lsd.h>
#include <iostream>
#include <cmath>
#include <boost/numeric/ublas/lu.hpp>


#include "VPCluster.h"
#include "VPSample.h"


std::vector< std::pair<float, float> > 	extractVP 		(image_double &image, const cv::Mat &img);
cv::Mat 								makeFeatureMap	(std::vector< std::pair<float, float> > &vp, const cv::Mat &img, double *reliability = NULL);



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

template<typename T>
std::vector<T> polyval( const std::vector<T>& oCoeff,  const std::vector<T>& oX ) {
	size_t nCount =  oX.size();
	size_t nDegree = oCoeff.size();
	std::vector<T>	oY( nCount );
 
	for ( size_t i = 0; i < nCount; i++ )
	{
		T nY = 0;
		T nXT = 1;
		T nX = oX[i];
		for ( size_t j = 0; j < nDegree; j++ )
		{
			// multiply current x by a coefficient
			nY += oCoeff[j] * nXT;
			// power up the X
			nXT *= nX;
		}
		oY[i] = nY;
	}
 
	return oY;
}





struct Segment {
	float x;
	float y;
	float xend;
	float yend;
	float angle;
};

float  horizonLine(cv::Mat &image) {
	if(image.empty()) return 0;


	std::vector<float> x, y;

	cv::Mat gray;
	cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

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


	cv::line(image, cv::Point_<int>(0, meanY), cv::Point_<int>(image.cols, meanY), cv::Scalar( 0, 255, 0 ), 2);



return meanY;


}


std::vector< std::pair<float, float> >  extractVP(const cv::Mat &image) {


	// ------------------------------------------------------------------------------------
	// convert image input

	if(image.empty()) return std::vector< std::pair<float, float> >();

	cv::Mat gray;
	cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
	cv::Mat grayF;
	gray.convertTo(grayF, CV_32FC1);


	image_double image_d = new_image_double(gray.cols, gray.rows);
	for(int i = 0 ; i < gray.cols*gray.rows ; ++i) {
		image_d->data[i] = reinterpret_cast<float*>(grayF.data)[i];
	}



	return extractVP(image_d, image);

}

std::vector< std::pair<float, float> > extractVP(image_double &image_d, const cv::Mat &) {


	// cv::Mat colorImg = image.clone();


	// ------------------------------------------------------------------------------------
	// Run LSD line segment detector
	

	ntuple_list ntuple = LineSegmentDetection( image_d, 0.7,
                                  0.6, 2.6,
                                  22.5, 0.0, 0.6,
                                  1024, 1.0,
                                  NULL );

	// ntuple_list ntuple = LineSegmentDetection( image_d, 0.7,
    //                               0.6, 2.6,
    //                               22.5, 0.0, 0.6,
    //                               1024, 1.0,
    //                               NULL );

	std::vector< std::vector<float> *> pts;

	std::vector<float> stroke_length(ntuple->size);
	for(size_t i = 0 ; i < ntuple->size ; ++i) {
		stroke_length[i] = static_cast<float>( (ntuple->values[ i * ntuple->dim + 0 ] - ntuple->values[ i * ntuple->dim + 2 ])*(ntuple->values[ i * ntuple->dim + 0 ] - ntuple->values[ i * ntuple->dim + 2 ]) + (ntuple->values[ i * ntuple->dim + 1 ] - ntuple->values[ i * ntuple->dim + 3 ])*(ntuple->values[ i * ntuple->dim + 1 ] - ntuple->values[ i * ntuple->dim + 3 ]));
	}

	std::sort(stroke_length.begin(), stroke_length.end());


	if(stroke_length.empty()) {
		free_image_double(image_d);
		free_ntuple_list(ntuple);
		return std::vector< std::pair<float, float> >();
	}

	// std::cout << stroke_length[static_cast<int>(stroke_length.size()*.7)] << std::endl;

	 // std::cout << ntuple->size << "line segments found:\n";
	for(size_t i = 0 ; i < ntuple->size ; ++i) {
		float len = static_cast<float>( (ntuple->values[ i * ntuple->dim + 0 ] - ntuple->values[ i * ntuple->dim + 2 ])*(ntuple->values[ i * ntuple->dim + 0 ] - ntuple->values[ i * ntuple->dim + 2 ]) + (ntuple->values[ i * ntuple->dim + 1 ] - ntuple->values[ i * ntuple->dim + 3 ])*(ntuple->values[ i * ntuple->dim + 1 ] - ntuple->values[ i * ntuple->dim + 3 ]) );
		if(len < stroke_length[static_cast<int>(stroke_length.size()*.7)]) continue;
		if(len < 827.748f) continue; 

		std::vector<float>* p = new std::vector<float>(4);
		pts.push_back(p);
		for(size_t j = 0 ; j < ntuple->dim ; ++j) {
			if(j < 4) (*p)[j] = static_cast<float>(ntuple->values[ i * ntuple->dim + j ]);
			// std::cout << ntuple->values[ i * ntuple->dim + j ] << " ";
		}
		 // std::cout << "\n";
	}

	free_image_double(image_d);
	free_ntuple_list(ntuple);


	// for(size_t i = 0 ; i < pts.size() ; ++i) {
	// 	cv::line(colorImg, cv::Point((*pts[i])[0], (*pts[i])[1]),  cv::Point((*pts[i])[2], (*pts[i])[3]), cv::Scalar(255,0,0),2);
	// }


	// ------------------------------------------------------------------------------------
	// JLinkage for cluster detection




	std::vector<unsigned int> Lables;
	std::vector<unsigned int> LableCount;

	if(pts.size() < 20) {
		for(size_t i = 0 ; i < pts.size() ; ++i) {
			delete pts[i];
		}

		return std::vector< std::pair<float, float> >();
	}

	std::vector<std::vector<float> *> *mModels = 
		VPSample::run(&pts, 5000, 2, 0, 3); // VPSample::run(&pts, 5000, 2, 0, 3);
	int classNum = VPCluster::run(Lables, LableCount, &pts, mModels, 2, 2);
	// std::cout<<"vpdetection found "<<classNum<<" classes!"<<std::endl;

	//2.1. release other resource
	for(unsigned int i=0; i < mModels->size(); ++i)
		delete (*mModels)[i];
	delete mModels;



	// ------------------------------------------------------------------------------------
	// Compute the vanishing point positions



	std::vector< std::vector<float> > lengths(Lables.size());
	std::vector<float> dydx(pts.size());
	std::vector<float> b(pts.size());
	std::vector<bool>  disabled(pts.size(), false);


	for(size_t i = 0 ; i < pts.size() ; ++i) {
		lengths[Lables[i]].push_back(std::sqrt(((*pts[i])[1]-(*pts[i])[3])*((*pts[i])[1]-(*pts[i])[3]) +  ((*pts[i])[0]-(*pts[i])[2])*((*pts[i])[0]-(*pts[i])[2])));
		if(std::abs(((*pts[i])[0] - (*pts[i])[2])) < 0.00001) {
			disabled[i] = true;
			continue;
		}


		dydx[i] = ((*pts[i])[1] - (*pts[i])[3]) / ((*pts[i])[0] - (*pts[i])[2]);
		b[i] =  (*pts[i])[1] - dydx[i] * (*pts[i])[0];

		if(atan(std::abs(dydx[i])) < (20*3.141592653/180)) {
			disabled[i] = true;
		}


	}

	for(int i = 0 ; i < classNum ; ++i) {
		std::sort(lengths[i].begin(), lengths[i].end());
	}

	std::vector<float> numPoints(classNum, 0);
	std::vector<float> vp_x(classNum, 0);
	std::vector<float> vp_y(classNum, 0);

	for(size_t ii = 0 ; ii < pts.size() ; ++ii) {
		for(size_t jj = ii+1 ; jj < pts.size() ; ++jj) {

			float l1 = std::sqrt(((*pts[ii])[1]-(*pts[ii])[3])*((*pts[ii])[1]-(*pts[ii])[3]) +  ((*pts[ii])[0]-(*pts[ii])[2])*((*pts[ii])[0]-(*pts[ii])[2]));
			float l2 = std::sqrt(((*pts[jj])[1]-(*pts[jj])[3])*((*pts[jj])[1]-(*pts[jj])[3]) +  ((*pts[jj])[0]-(*pts[jj])[2])*((*pts[jj])[0]-(*pts[jj])[2]));

			if(Lables[ii] != Lables[jj]) continue;
			if(disabled[ii] || disabled[jj]) continue;
			if(lengths[Lables[ii]].size() == 0) continue;
			if(lengths[Lables[jj]].size() == 0) continue;


			if(l1 < lengths[Lables[ii]][static_cast<int>(lengths[Lables[ii]].size()*.7)]) continue;
			if(l2 < lengths[Lables[jj]][static_cast<int>(lengths[Lables[jj]].size()*.7)]) continue;

			if(std::abs((dydx[jj] - dydx[ii])) < 0.000001) continue;

			float x = (b[ii] - b[jj]) / (dydx[jj] - dydx[ii]);
			float y = dydx[ii] * x + b[ii];

	
			// cv::line(colorImg, cv::Point((*pts[ii])[0], (*pts[ii])[1]),  cv::Point((*pts[ii])[2], (*pts[ii])[3]), cv::Scalar(0,255,0),2);
			// cv::line(colorImg, cv::Point((*pts[jj])[0], (*pts[jj])[1]),  cv::Point((*pts[jj])[2], (*pts[jj])[3]), cv::Scalar(0,0,255),2);

			vp_x[Lables[ii]] += x;
			vp_y[Lables[ii]] += y;
			++numPoints[Lables[ii]];
		}
	}

	std::vector< std::pair<float, float> > vp;
	int considered_idx = 0;
	for(int i = 0 ; i < classNum ; ++i) {
		if(numPoints[i] == 0) 
			continue;

		vp_x[i] /= numPoints[i];
		vp_y[i] /= numPoints[i];

		// if(numPoints[i] < 1)
		// 	continue;

		considered_idx = i;
		vp.push_back(std::make_pair(vp_x[i], vp_y[i]));

		// cv::circle(colorImg, cv::Point(vp_x[i], vp_y[i]), 3, cv::Scalar(0,0,255),2);
	}

	//cv::imshow("colorImg", colorImg);
	// cv::waitKey();

	// ------------------------------------------------------------------------------------
	// Done. free memory


	for(size_t i = 0 ; i < pts.size() ; ++i) {
		delete pts[i];
	}


	return vp;

}


cv::Mat makeFeatureMap	(std::vector< std::pair<float, float> > &vp, const cv::Mat &img, double *reliability) {
	cv::Mat featureMap(img.rows, img.cols, CV_64FC1, cv::Scalar(0.f));

	if(reliability != NULL)  *reliability = 0.f;

	std::vector<float> dists;
	for(size_t i = 0 ; i < vp.size() ; ++i) {
        float normx = vp[i].first/img.cols;
        float normy = vp[i].second/img.rows;

		if(normx > 1.f || normx < 0.f) continue;
		if(normy > 1.f || normy < 0.f) continue;

        // std::cout << normx << " " << normy << std::endl;

        float d = (normx-.5f)*(normx-.5f) + (normy-.5f)*(normy-.5f);

		dists.push_back(d);
	}

	std::sort(dists.begin(), dists.end());

    for(size_t i = 0 ; i < vp.size() ; ++i) {
        float normx = vp[i].first/img.cols;
        float normy = vp[i].second/img.rows;

		if(normx > 1.f || normx < 0.f) continue;
		if(normy > 1.f || normy < 0.f) continue;

        // std::cout << normx << " " << normy << std::endl;

        float d = (normx-.5f)*(normx-.5f) + (normy-.5f)*(normy-.5f);

		if(dists.size() > 7 && dists[dists.size()-7] > d) {
			continue;
		}

		if(std::abs(normy-.5f) > 0.25) continue;

		d =  (normy-.5f)*(normy-.5f);

        float sigma = 0.00005f / (d + 0.01f);
		if(reliability != NULL) {
			*reliability = std::max(static_cast<float>(*reliability), sigma);
		}
        
        for(int i = 0 ; i < img.rows ; ++i) {
            for(int j = 0 ; j < img.cols ; ++j) {
                float fi = static_cast<float>(i) / img.rows;
                float fj = static_cast<float>(j) / img.cols;
                featureMap.at<double>(i,j) = std::max(featureMap.at<double>(i,j), static_cast<double>(std::exp(-((fj-normx)*(fj-normx)*2 + (fi-normy)*(fi-normy))/sigma)));

            }
        }
    }

    return featureMap;
}


cv::Mat vanishingLineFeatureMapF64	(const cv::Mat &img, double *reliability) {


	image_double image_d = new_image_double(img.cols, img.rows);
	for(int i = 0 ; i < img.cols*img.rows ; ++i) {
		image_d->data[i] = reinterpret_cast<double*>(img.data)[i];
	}

	std::vector< std::pair<float, float> > vp = extractVP(image_d, img);

    return makeFeatureMap(vp, img, reliability);
}

cv::Mat vanishingLineFeatureMapF64C3(const cv::Mat &img, double *reliability) {

	cv::Mat imgu(img.rows, img.cols, CV_8UC3, cv::Scalar(0.f, 0.f, 0.f));
    for(int i = 0 ; i < img.rows ; ++i) {
        for(int j = 0 ; j < img.cols ; ++j) {
            cv::Point3_<unsigned char> &o = imgu.at< cv::Point3_<unsigned char> >(i,j);
            const cv::Point3_<double> &in = img.at< cv::Point3_<double> >(i,j);

            o.x = static_cast<unsigned char>(in.x*255);
            o.y = static_cast<unsigned char>(in.y*255);
            o.z = static_cast<unsigned char>(in.z*255);
        }
    }

	std::vector< std::pair<float, float> > vp = extractVP(imgu);

    return makeFeatureMap(vp, img, reliability);
}


cv::Mat vanishingLineFeatureMap (const cv::Mat &img, double *reliability) {

	if(img.empty()) return cv::Mat();

    std::vector< std::pair<float, float> > vp = extractVP(img);

    return makeFeatureMap(vp, img, reliability);
}








