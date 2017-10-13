// **************************************************************************************************
//
// This program was implemented by Pierre Lebreton
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// **************************************************************************************************




#include "GBVS.h"
#include <iostream>
#include <list>
#include <cmath>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <limits>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#ifdef WITH_FFTW
	int 			 GBVS::instanceCounter = 0;
	fftwpp::fft2d	*GBVS::forward = NULL;
	fftwpp::fft2d	*GBVS::backward = NULL;
#endif

#define WITH_LINPER 1
#define WITH_MEAN_SHIFT 1

#ifdef WITH_LINPER
	#include <VPD.h>
#endif

#ifdef WITH_MEAN_SHIFT
	#include <Segm.h>
#endif


GBVS::GBVS() {
	salmapmaxsize 	= 32;
	blurfrac		= 0.020f;

	channels		= "DIO";
	colorWeight		= 1.0f;
	intensityWeight	= 1.0f;
	orientationWeight	= 1.0f;
	contrastWeight	= 1.0f;
	flickerWeight	= 1.0f;
	motionWeight	= 1.0f;
	dklcolorWeight	= 1.0f;
	linperWeight	= .1f;
	faceFeatureWeight = 1.5f;
	blurFeatureWeight = 1.0f;

	gaborangles.resize(4);
	gaborangles[0] = 0;
	gaborangles[1] = 45;
	gaborangles[2] = 90;
	gaborangles[3] = 135;

	motionAngles.resize(4);
	motionAngles[0] = 0;
	motionAngles[1] = 45;
	motionAngles[2] = 90;
	motionAngles[3] = 135;

	contrastwidth	= 0.1f;
	flickerNewFrameWt = 1;

	unCenterBias = 0;

	levels.resize(3);
	levels[0] = 2;
	levels[1] = 3;
	levels[2] = 4;

	sigma_frac_act = 0.15f;
	sigma_frac_norm = 0.06f;
	num_norm_iters = 1;
	tol = .00000001f;
	cyclic_type = 2;
	normalizationType = 1;
	normalizeTopChannelMaps = 0;


	useCSF = true;
	viewingDistance = 2.5f;
	nbPixelPerDegree = 60.f;


	initDone = false;


	initCascadeClassifier = false;
	faceCascadeEnabled = false;
	eyeCascadeEnabled = false;


	equatorialPrior = false;
	nbThreads = 2;



#ifdef WITH_FFTW
	++instanceCounter;
#endif

}

GBVS::~GBVS() {

#ifdef WITH_FFTW
	--instanceCounter;

	if(instanceCounter == 0) {
		if(forward  != NULL)  delete forward;
		if(backward != NULL)  delete backward;
	}
#endif

}


void GBVS::clear() {
	channelMaps.clear();
	allmaps.clear();
	features.clear();
}


// ==============================================================================================================================
// GBVS model


void GBVS::compute(const cv::Mat& img, cv::Mat &out, bool normalize) {


	

	// ---------------------------------------------------------------------------------
	// STEP 1 : compute raw feature maps from image
	computeFeatures(img);




	// ---------------------------------------------------------------------------------
	// STEP 2 : run the GBVS framework to perform the activation and feature aggregation 

	gbvsActivation(img, out, normalize);



	if(equatorialPrior) {
		applyEquatorialPrior(out, img);
	}


}


void GBVS::computeFeatures(const cv::Mat &img_u) {

	assert(img_u.channels() == 3 && img_u.type() == CV_8UC3);
	

	if(img_u.cols < 128 || img_u.rows < 128) {
		std::cerr << "GBVS Error: gbvs() meant to be used with images >= 128x128\n";
		return;
	}

	if(!initDone)
		initGBVS(img_u);



	// convert input image to floating format
	cv::Mat img = getFloatingImageC3(img_u);



	// cleanup eventual previous execution...
	features.clear();
	allmaps.clear();
	channelMaps.clear();



	// get all features maps
	getFeatureMaps(img);

}



void GBVS::computeActivation(const cv::Mat& input, cv::Mat &out, bool normalize) {

	if(input.cols < 128 || input.rows < 128) {
		std::cerr << "GBVS Error: gbvs() meant to be used with images >= 128x128\n";
		return;
	}

	if(!initDone)
		initGBVS(input);


	features.clear();
	allmaps.clear();
	channelMaps.clear();

	cv::Mat featureMap;
	input.convertTo(featureMap, CV_64FC1);



	// -------------------------------------------------------------------
	// STEP 1 : create a multi-resolution version of the provided feature


	std::vector<cv::Mat> imgL;
	imgL.push_back(subsample(featureMap));
	

	for(int i = 1 ; i < maxcomputelevel ; ++i) {
		imgL.push_back(subsample(imgL.back()));
		if(imgL.back().cols < 3 || imgL.back().rows < 3) {
			std::cerr << "[I] reached minimum size at level " << i << "cutting off additional levels\n";
			std::vector<int> ll;
			for(int j = 0 ; j < i+1 ; ++j) {
				ll.push_back(levels.at(i));
			}

			levels = ll;
			maxcomputelevel = i;
			break;
		}
	}


	Feature f;
	f.description = "Inserted feature";
	f.weight = intensityWeight;
	f.channel = 0;

	for (size_t lev = 0 ; lev < levels.size() ; ++lev) {
		FeatureMap fm1;
		fm1.type  = 0;
		fm1.level = static_cast<int>(lev); 
		fm1.channel = 0;

		cv::resize(imgL[levels[lev]-1], fm1.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);
		
		f.maps.push_back(fm1);
	}

	features.push_back(f);



	// ---------------------------------------------------------------------------------
	// STEP 2 : run the GBVS framework to perform the activation and feature aggregation 


	gbvsActivation(featureMap, out, normalize);


}


void GBVS::gbvsActivation(const cv::Mat &featureMap, cv::Mat &out, bool normalize) {

	// STEP 1 : compute activation maps from feature maps
	computeActivation();

	// STEP 2: normalize activation maps
	normalizeActivation();

	// STEP 3 : average across maps within each feature channel
	averageByFeatureChannel();

	// STEP 4 : sum across feature channels
	sumChannels(normalize);

	// STEP 5: blur for better results
	blurMasterMap(normalize);

	// Resize output image
	cv::Mat result;
	cv::resize(master_map, result, cv::Size(featureMap.cols, featureMap.rows), 0, 0, cv::INTER_LANCZOS4);
	
	
	if(normalize) {
		double mn, mx;
		cv::minMaxLoc(result, &mn, &mx);
		result = (result-mn)/(mx-mn);
		
	} 

	result.convertTo(out, CV_32FC1);
}







// ==============================================================================================================================
// Initialization function



void GBVS::initGBVS(const cv::Mat& img_u) {

	if(initDone) return;
	
	maxcomputelevel = 0;
	for(size_t i = 0 ; i < levels.size() ; ++i)
		maxcomputelevel = std::max(maxcomputelevel, levels[i]);


	float width  = static_cast<float>(img_u.cols);
	float height  = static_cast<float>(img_u.rows);
	float scale = static_cast<float>(salmapmaxsize) / std::fmax(width,height);

	salmapmaxsize_v.clear();
	salmapmaxsize_v.push_back(static_cast<int>(round(height * scale)));
	salmapmaxsize_v.push_back(static_cast<int>(round(width  * scale)));


	graphsalinit(salmapmaxsize_v);
	makeGaborFilters();

	initDone = true;

}


// this function creates the weight matrix for making edge weights
// and saves some other constants (like node-in-lattice index) to a 'frame'
// used when the graphs are made from saliency/feature maps.
void GBVS::graphsalinit(const std::vector<int> &map_size) {

	std::vector<cv::Mat> apyr;
	std::vector< std::pair<int, int> > dims;
	int N = 0;
	std::vector<cv::Mat> nam;


	getDims(map_size , multilevels, apyr, dims);
	namenodes(dims, N, nam);
	grframe.lx = makeLocationMap(dims, nam, N);
	cv::Mat cx = connectMatrix(dims, grframe.lx, 2, 2 , cyclic_type);
	cv::Mat dx = distanceMatrix(dims, grframe.lx, cyclic_type);

	grframe.d = dx.mul(cx);
	grframe.dims = dims;
	grframe.multilevels = multilevels;

}





// ==============================================================================================================================
// Graph-related functions





void GBVS::getDims(const std::vector<int> &dims, const std::vector<int> &deltas, std::vector<cv::Mat> &apyr, std::vector< std::pair<int, int> > &o_dims) const {
	formMapPyramid(cv::Mat(dims[0], dims[1], CV_64FC1, cvScalar(1.f)), deltas, apyr, o_dims);
}





void GBVS::formMapPyramid(const cv::Mat& a, const std::vector<int> &deltas, std::vector<cv::Mat> &apyr, std::vector< std::pair<int, int> > &o_dims) const {
	double my_eps = 1e-12;

	size_t num_deltas = deltas.size();
	int max_delta = 0;
	for(size_t i = 0 ; i < deltas.size() ; ++i) {
		max_delta = static_cast<int>(fmax(max_delta, deltas[i]));
	}
	size_t num_pyr = 1 + num_deltas;

	std::vector<int> dim;
	dim.push_back(a.cols);
	dim.push_back(a.rows);
	dim.push_back(static_cast<int>(num_pyr));

	o_dims.resize(num_pyr);
	apyr.resize(num_pyr);

	std::list<cv::Mat> maps;
	maps.push_back(a);

	for(int i = 0 ; i < max_delta ; ++i) {
		maps.push_back(subsample(maps.back()));
	}


	for(std::list<cv::Mat>::iterator it = maps.begin() ; it != maps.end() ; ++it) {
		cv::Mat &lmap = *it;

		double mx, mn;
		cv::minMaxLoc(lmap, &mn, &mx);
		if(mx == 0) {
			lmap = lmap + my_eps;
		} 
		else {		// TODO : mat2gray() ? 
			lmap = (lmap - mn) / (mx - mn);
		}
	}


	apyr[0] = maps.front();
	o_dims[0].first  = apyr[0].rows;
	o_dims[0].second = apyr[0].cols;

	for(size_t i = 0 ; i < deltas.size() ; ++i) {
		int k = 1;
		std::list<cv::Mat>::iterator it;
		for(it = maps.begin() ; it != maps.end() ; ++it) {
			if(k == deltas[i])
				break;
			++k;
		}

		if(it == maps.end()) {
			throw std::logic_error(std::string("GBVS::getDims: Could not find the right band... This should not happens.\n"));
		}

		cv::Mat &lmap = *it;
		cv::Mat &lmap_apyr = apyr[1+i];

		lmap_apyr = lmap;

		o_dims[1+i].first  = lmap.rows;
		o_dims[1+i].second = lmap.cols;
	}

}




void GBVS::namenodes(const std::vector< std::pair<int, int> > &dims, int &N, std::vector<cv::Mat> &nam) const {
	size_t nmaps = dims.size();
	N = 0;

	for(size_t i = 0 ; i < nmaps ; ++i) {
		N += dims[0].second * dims[0].first;
	}

	nam.resize(nmaps);
	for(size_t i = 0 ; i < nmaps ; ++i) {
		nam[i] = cv::Mat(dims[0].second, dims[0].first, CV_64FC1, cvScalar(1.f));
	}

	double curN = 0;
	for(size_t i = 0 ; i < nmaps ; ++i) {
		cv::Mat &lnam = nam[i];

		for(int k = 0 ; k < dims[i].first ; ++k) {
			for(int j = 0 ; j < dims[i].second ; ++j) {
				lnam.at<double>(j,k) = curN;
				curN++;
			}
		}
	}
}




cv::Mat GBVS::makeLocationMap(const std::vector< std::pair<int, int> > &dims, const std::vector<cv::Mat> &nam, int N) const {
	size_t nmaps = dims.size();

	cv::Mat lx(N, 8195, CV_64FC1, cvScalar(0.f)); // 8195 == 2^13+3

	std::vector< std::vector< std::vector<int> > > px_r(nmaps);
	std::vector< std::vector< std::vector<int> > > px_c(nmaps);

	for(size_t i = 0 ; i < nmaps ; ++i) {
		px_r[i] = partitionindex(dims[0].second, dims[i].second);
		px_c[i] = partitionindex(dims[0].first, dims[i].first);
	}


	size_t maxL = 0;
	for(size_t i = 0 ; i < nmaps ; ++i) {
		for(int j = 0 ; j < dims[i].second ; ++j) {			// second is the number of rows
			for(int k = 0 ; k < dims[i].first ; ++k) {		// first is the number of columns
				double nm = nam[i].at<double>(j,k);

				std::vector<int> xcoords;
				std::vector<int> ycoords;

				for(size_t n = 0 ; n < px_r[i][1].size() ; ++n) {
					if(px_r[i][1][n] == j) {
						xcoords.push_back(px_r[i][0][n]);						
					}
				}


				for(size_t n = 0 ; n < px_c[i][1].size() ; ++n) {
					if(px_c[i][1][n] == k) {
						ycoords.push_back(px_c[i][0][n]);
					}
				}

				std::list<double> lst;
				size_t Nl = 0;
				for(size_t ii = 0 ; ii < xcoords.size() ; ++ii) {
					for(size_t jj = 0 ; jj < ycoords.size() ; ++jj) {
						lst.push_back(nam[0].at<double>(xcoords[jj], ycoords[ii]));
						Nl++;
					}
				}

				maxL = std::max(maxL, Nl);

				lx.at<double>(static_cast<int>(nm), 0) = static_cast<double>(nm);
				lx.at<double>(static_cast<int>(nm), 1) = static_cast<double>(Nl);
				int ii = 0;
				for(std::list<double>::iterator it = lst.begin() ; it != lst.end() ; ++it) {
					lx.at<double>(static_cast<int>(nm), 2+ii) = *it;
					++ii;
				}
				
			}
		}
	}

	cv::Mat lx2(N, static_cast<int>(maxL+2), CV_64FC1, cvScalar(0.f));
	for(int i = 0 ; i < lx2.rows ; ++i) {
		for(int j = 0 ; j < lx2.cols ; ++j) {
			lx2.at<double>(i,j) = lx.at<double>(i,j);
		}
	}

	return lx2;

}





std::vector< std::vector<int> > GBVS::partitionindex(int N, int M) const {
	int binSize = N / M;
	int leftOver = N - binSize * M;

	std::vector<int> pad(M);
	pad[0] = static_cast<int>(floor(static_cast<double>(leftOver)/2.f));
	pad[M-1] = static_cast<int>(ceil(static_cast<double>(leftOver)/2.f));

	std::vector< std::vector<int> > ix;
	ix.resize(2);
	ix[0].resize(N);
	ix[1].resize(N);

	int curindex = 0;
	for(int i = 0 ; i < M ; ++i) {
		for(int j = 0 ; j < binSize+pad[i] ; ++j) {
			ix[0][curindex] = curindex;
			ix[1][curindex] = i;
			curindex++;
		}
	}

	return ix;
}






cv::Mat GBVS::connectMatrix(const std::vector< std::pair<int, int> >& dims , 
							const cv::Mat&  lx , 
							int inter_type , 
							int intra_type , 
							int cyclic_type ) const {

// inter_type :
//   1 => only neighbor
//   2 => everywhere inter-scale on consecutive scales
// intra_type :
//   1 => only neighbor
//   2 => connect to everynode on same scale
// cyclic_type
//   1 => cyclic boundary rules
//   2 => non-cyclic boundaries

	// ------------------------------------------------------------
	//some useful numbers to access nodes at each resolution level
	int N = 0;
	for(size_t i = 0 ; i < dims.size() ; ++i)
		N += dims[i].first*dims[i].second;

	cv::Mat cx(N, N, CV_64FC1, cvScalar(0.f));

	std::vector<int> offsets(dims.size(),0);
	for(size_t i = 1 ; i < offsets.size() ; ++i) {
		offsets[i] = dims[i].first*dims[i].second + offsets[i-1];
	}


	// ------------------------------------------------------------
	// connect nodes on a single resolution/level

	for(size_t i = 0 ; i < dims.size() ; ++i) {
		int mapSize = dims[i].first*dims[i].second;

		if(intra_type == 1) {
			cv::Mat dmatrix = simpledistance(dims[i], cyclic_type);
			for(int ii = offsets[i] ; ii < offsets[i] + mapSize ; ++ii) {
				for(int jj = offsets[i] ; jj < offsets[i] + mapSize ; ++jj) {
					cx.at<double>(ii,jj) = static_cast<double>(dmatrix.at<double>(ii-offsets[i], jj-offsets[i]) <= 1);
				}
			}

		} else {
			for(int ii = offsets[i] ; ii < offsets[i] + mapSize ; ++ii) {
				for(int jj = offsets[i] ; jj < offsets[i] + mapSize ; ++jj) {
					cx.at<double>(ii,jj) = 1;
				}
			}
		}

	}

	// ------------------------------------------------------------
	// for inter-scale nodes , connect according to some rule
	// inter_type is 1  ==> connect only nodes corresponding to same location
	// inter_type is 2  ==> connect nodes corresponding to different locations



	for(size_t ci = 0 ; ci < dims.size() ; ++ci) {
		for(size_t cj = ci ; cj < dims.size() ; ++cj) {

			for(int ii = 0 ; ii < dims[ci].first * dims[ci].second ; ++ii) {
				for(int jj = 0 ; jj < dims[cj].first * dims[cj].second ; ++jj) {

					if(inter_type == 1) {
						std::vector<double> la, lb;
						for(int k = 2; k < (2+lx.at<double>(offsets[ci]+ii,1)) ; ++k)
							la.push_back(lx.at<double>(offsets[ci]+ii, k));

						for(int k = 2; k < (2+lx.at<double>(offsets[cj]+jj,1)) ; ++k)
							lb.push_back(lx.at<double>(offsets[cj]+jj, k));

						bool intersect = false;
						for(size_t k = 0 ; k < la.size() && !intersect ; ++k) {
							for(size_t k2 = 0 ; k2 < lb.size() && !intersect ; ++k2) {
								if(la[k] == lb[k2]) {
									intersect = true;
								}
							}
						}

						if(intersect) {
							cx.at<double>(offsets[ci]+ii, offsets[cj]+jj) = 1;
							cx.at<double>(offsets[cj]+jj, offsets[ci]+ii) = 1;
						}

					} else {
						if(inter_type == 2) {
							cx.at<double>(offsets[ci]+ii, offsets[cj]+jj) = 1;
							cx.at<double>(offsets[cj]+jj, offsets[ci]+ii) = 1;
						}
					}
				}
			}
		}
	}

	double *ptx = reinterpret_cast<double*>(cx.data);
	for(int i = 0 ; i < cx.cols*cx.rows ; ++i) {
		//std::cout << ptx[i] << " ";
		if(ptx[i] == 0)
			ptx[i] = std::numeric_limits<double>::max();
	}


	return cx;
}





cv::Mat GBVS::distanceMatrix(const std::vector< std::pair<int, int> >& dims, 
							 const cv::Mat&  lx, 
							 int cyclic_type) const {

	int N = 0;
	for(size_t i = 0 ; i < dims.size() ; ++i)
		N += dims[i].first*dims[i].second;

	cv::Mat dx(N, N, CV_64FC1, cvScalar(0.f));

	std::vector<int> offsets(dims.size(),0);
	for(size_t i = 1 ; i < offsets.size() ; ++i) {
		offsets[i] = dims[i].first*dims[i].second + offsets[i-1];
	}


	std::vector<cv::Mat> sd(dims.size());
	for(size_t i = 0 ; i < dims.size() ; ++i) {
		sd[i] = simpledistance(dims[i], cyclic_type);
	}


	for(size_t i = 0 ; i < dims.size() ; ++i) {
		int mapSize = dims[i].first*dims[i].second;
		for(int ii = 0 ; ii < mapSize ; ++ii) {
			for(int jj = 0 ; jj < mapSize ; ++jj) {
				dx.at<double>(offsets[i]+ii, offsets[i]+jj) = sd[i].at<double>(ii,jj) * (dims[0].first*dims[0].second/mapSize);
			}
		}
	}

	for(size_t ci = 0 ; ci < dims.size() ; ++ci) {
		for(size_t cj = ci ; cj < dims.size() ; ++cj) {

			for(int ii = 0 ; ii < dims[ci].first * dims[ci].second ; ++ii) {
				for(int jj = 0 ; jj < dims[cj].first * dims[cj].second ; ++jj) {
					
					// using location matrix, determine locations of the two nodes
					std::vector<double> la, lb;
					for(int k = 2; k < (2+lx.at<double>(offsets[ci]+ii,1)) ; ++k)
						la.push_back(lx.at<double>(offsets[ci]+ii, k));

					for(int k = 2; k < (2+lx.at<double>(offsets[cj]+jj,1)) ; ++k)
						lb.push_back(lx.at<double>(offsets[cj]+jj, k));

					double mean_dist = 0;

					for(size_t iii = 0 ; iii < la.size() ; ++iii) {
						for(size_t jjj = 0 ; jjj < lb.size() ; ++jjj) {
							double dd = sd[0].at<double>(static_cast<int>(la[iii]), static_cast<int>(lb[jjj]));
							mean_dist += dd;
						}
					}

					mean_dist /= (la.size()*lb.size());

					dx.at<double>(offsets[ci]+ii, offsets[cj]+jj) = mean_dist;
					dx.at<double>(offsets[cj]+jj, offsets[ci]+ii) = mean_dist;

				}

			}
		}
	}

	return dx;

}




cv::Mat GBVS::simpledistance(const std::pair<int, int>& dim, int cyclic_type) const {
	int N = dim.first*dim.second;
	cv::Mat d(N, N, CV_64FC1, cvScalar(0.f));

	#define idx(mi,mj) ((mj)*dim.first+(mi))

	for(int i = 0 ; i < dim.second ; ++i) {
		for(int j = 0 ; j < dim.first ; ++j) {
			for(int ii = 0 ; ii < dim.second ; ++ii) {
				for(int jj = 0 ; jj < dim.first ; ++jj) {

					if(d.at<double>(idx(j,i), idx(jj,ii)) == 0) {
						double di = 0.f;
						double dj = 0.f;

						if ( cyclic_type==1 ) {
							di = std::min( std::abs(i-ii) , std::abs( std::abs(i-ii) - dim.second ) );
							dj = std::min( std::abs(j-jj) , std::abs( std::abs(j-jj) - dim.first ) );
						} else {
							di = i-ii;
	                        dj = j-jj;
						}

						d.at<double>(idx(j,i), idx(jj,ii)) = di*di+dj*dj;
						d.at<double>(idx(jj,ii), idx(j,i)) = di*di+dj*dj;
					}
				}
			}
		}
	}

	#undef idx

	return d;

}



void GBVS::arrangeLinear(const std::vector<cv::Mat> &apyr, const std::vector< std::pair<int, int> > &dims, std::vector<double> &o_datas) const {
	int sumDim = 0;
	for(size_t i = 0 ; i < dims.size() ; ++i)
		sumDim += dims[i].first*dims[i].second;

	o_datas.resize(sumDim);
	size_t curindex = 0;
	for(size_t pyl = 0 ; pyl < apyr.size() ; ++pyl) {
		for(int jj = 0 ; jj < apyr[pyl].cols ; ++jj) {
			for(int ii = 0 ; ii < apyr[pyl].rows ; ++ii) {
				o_datas[curindex] = apyr[pyl].at<double>(ii,jj);
				++curindex;
			}
		}
	}
}



void GBVS::assignWeights(const std::vector<double>& AL, const cv::Mat& dw, cv::Mat &mm, int algtype) const {

	for(int c = 0 ; c < static_cast<int>(AL.size()) ; ++c) {

		for(int r = 0 ; r < static_cast<int>(AL.size()) ; ++r) {
			
			if(algtype == 1) {
				mm.at<double>(r,c) = dw.at<double>(r,c) * AL[r];
			} else if(algtype == 2) {
				mm.at<double>(r,c) = dw.at<double>(r,c) * std::abs( AL[r] - AL[c] );
			} else if(algtype == 3) {
				mm.at<double>(r,c) = dw.at<double>(r,c) * std::abs( std::log( AL[r]/AL[c] ) );
			} else if(algtype == 4) {
				mm.at<double>(r,c) = dw.at<double>(r,c) * 1.f / (std::abs( AL[r] - AL[c] )+1e-12);
			}
		}
	}
}




// each column sums to 1
void GBVS::columnNormalize(cv::Mat &mm) const {

	for(int j = 0 ; j < mm.cols ; ++j) {
		double sum = 0.f;
		for(int i = 0 ; i < mm.rows ; ++i) {
			sum += mm.at<double>(i,j);
		}

		if(std::abs(sum) < 0.00000000001) {
			sum = 0.00000000001;
		}

		for(int i = 0 ; i < mm.rows ; ++i) {
			mm.at<double>(i,j) /= sum;
		}
	}
}




// computes the principal eigenvector of a [nm nm] markov matrix
void GBVS::principalEigenvectorRaw(const cv::Mat& markovA, float tol, std::vector<double>& AL, int &iteri) const {
	// if(sparseness(markovA) < 0.4f) {
	// 	// MATLAB works on sparse matrix...
	// }

	int D = markovA.rows;
	double df = 1.0f;

	cv::Mat v(D, 1, CV_64FC1, cvScalar(1.f/D));
	cv::Mat oldv = v.clone();
	cv::Mat oldoldv = v.clone();

	iteri = 0;

	while(df > tol && iteri < 10000 ) { // 

		oldoldv = oldv.clone();		
		oldv = v.clone();

		
		v = markovA * v;
		df = 0.f;

		double sum = 0;
		for(int i = 0 ; i < v.cols ; ++i) {
			double diff = oldv.at<double>(i,0) - v.at<double>(i,0);
			df  += diff*diff;
			sum += v.at<double>(i,0);
		}

		// TODO: This should be a single square root. But the norm 2 differ with matlab? differences due to usage of sparse matrix by MATLAB? 
		// Here several additional iteration are enforced to converge a little bit more... 
		df = std::sqrt(df); 

		++iteri;

		if( sum >= 0 )
			continue;
		else {
			v = oldoldv; 
			break;
		}
	}

	double sum = cv::sum( v )[0];
	for(int i = 0 ; i < D ; ++i) {
		AL[i] = reinterpret_cast<double*>(v.data)[i] / sum;
	}

}



float GBVS::sparseness(const cv::Mat& markovA) const {
	float notNull = 0;
	for(int i = 0 ; i < markovA.rows ; ++i) {
		for(int j = 0 ; j < markovA.cols ; ++j) {
			if(markovA.at<double>(i,j) != 0) ++notNull;
		}
	}

	return notNull / (markovA.cols*markovA.rows);
}



void GBVS::sumOverScales(std::vector<double> &v, const cv::Mat &lx, int N, std::vector<double> &vo) const {
	vo.resize(N);

	for(int i = 0 ; i < static_cast<int>(v.size()) ; ++i) {
		double k = lx.at<double>(i, 1);
		double vtmp = v[i] / k;
		for(int j = 0 ; j < k ; ++j) {
			vo[ static_cast<int>( lx.at<double>(i, 2+j) ) ] += vtmp;
		}
	}
}




// ------------------------------------------------------------------------------------------------
// core function 




cv::Mat GBVS::graphsalapply(const cv::Mat &A, const Frame& frame, float sigma_frac, int num_iters, int algtype, float tol) const {
	if(algtype == 4) {
		cv::Mat result;
		cv::pow(A, 1.5, result);
		return result;
	}

	const cv::Mat &lx = frame.lx;

	// form a multiresolution pyramid of feature maps according to multilevels
	std::vector<cv::Mat> apyr;
	std::vector< std::pair<int, int> > dims;
	formMapPyramid(A, frame.multilevels, apyr, dims);


	//get a weight matrix between nodes based on distance matrix
	double sig = sigma_frac * static_cast<double>(A.cols + A.rows)/2.f;
	cv::Mat dw(frame.d.rows, frame.d.cols, CV_64FC1, cvScalar(0.f)); 
	for(int i = 0 ; i < frame.d.rows ; ++i) {
		for(int j = 0 ; j < frame.d.cols ; ++j) {
			dw.at<double>(i,j) = std::exp( -1 * frame.d.at<double>(i,j) / (2 * sig*sig));
		}
	}

	// assign a linear index to each node
	std::vector<double> AL;
	arrangeLinear(apyr , dims, AL);


	// create the state transition matrix between nodes
	cv::Mat mm(lx.rows, lx.rows, CV_64FC1, cvScalar(0.f)); 

	for(int iter = 0 ; iter < num_iters ; ++iter) {

		// assign edge weights based on distances between nodes and algtype
		assignWeights( AL , dw , mm , algtype );

		// make it a markov matrix (so each column sums to 1)
		columnNormalize(mm);

		int iteri = 0;
		principalEigenvectorRaw(mm, tol, AL, iteri); 

		iter += iteri;
	}

	// collapse multiresolution representation back onto one scale
	std::vector<double> vo;
	sumOverScales(AL, lx, A.cols*A.rows, vo);

	
	// arrange the nodes back into a rectangular map
	cv::Mat result(A.rows, A.cols, CV_64FC1, cvScalar(0.f));
	int curindex = 0;
	for(int jj = 0 ; jj < result.cols ; ++jj) {
		for(int ii = 0 ; ii < result.rows ; ++ii) {
			result.at<double>(ii,jj) = vo[curindex];
			++curindex;
		}
	}

	return result;
}








// ==============================================================================================================================
// Features related functions





void GBVS::makeGaborFilters() {
	for(size_t i = 0 ; i < gaborangles.size() ; ++i) {
		GaborFilter g;
		g.g0  = makeGaborFilterGBVS(gaborangles[i], 0);
		g.g90 = makeGaborFilterGBVS(gaborangles[i], 90);

		gaborFilters.push_back(g);
	}
}


cv::Mat GBVS::makeGaborFilterGBVS(float angle, float phase, float stddev, float elongation, float filterSize, float filterPeriod, bool makeDisk) const {
	double minor_stddev = stddev * elongation;
	double max_stddev   = std::fmax(minor_stddev, stddev);
	static const double pi = 3.14159265358979116f;

	if(filterSize < 0) {
		filterSize = static_cast<float>(std::ceil(max_stddev * std::sqrt(10.f)));
	} else {
		filterSize = std::floor(filterSize / 2);
	}

	



	double psi   = pi / 180.f * phase;
	double rtDeg = pi / 180.f * angle;
	double omega = 2 * pi / filterPeriod;
	double co = std::cos(rtDeg);
	double si = -std::sin(rtDeg);
	double major_sigq = 2 * stddev*stddev;
	double minor_sigq = 2 * minor_stddev*minor_stddev;

	cv::Mat result(static_cast<int>(2*filterSize+1), static_cast<int>(2*filterSize+1), CV_64FC1);

	for(int i = 0 ; i < result.rows ; ++i) {
		for(int j = 0 ; j < result.cols ; ++j) {
			double major = (i-filterSize)*co + (j-filterSize)*si;
			double major2 = major*major;

			double minor = (i-filterSize)*si - (j-filterSize)*co;
			double minor2 = minor*minor;

			result.at<double>(i,j) = std::cos(omega * major + psi) * std::exp(-major2 / major_sigq - minor2 / minor_sigq);
		}
	}

	if(makeDisk) {
		for(int i = 0 ; i < result.rows ; ++i) {
			for(int j = 0 ; j < result.cols ; ++j) {
				double major = (i-filterSize)*co + (j-filterSize)*si;
				double major2 = major*major;

				double minor = (i-filterSize)*si + (j-filterSize)*co;
				double minor2 = minor*minor;

				if((major2+minor2) > ((filterSize/2)*(filterSize/2))) {
					result.at<double>(i,j) = 0;
				}
			}
		}
	}

	double sum = 0.f;
	for(int i = 0 ; i < result.rows*result.cols ; ++i) {
		sum += reinterpret_cast<double*>(result.data)[i];
	}

	result -= sum / (result.rows*result.cols);

	sum = 0.f;
	for(int i = 0 ; i < result.rows*result.cols ; ++i) {
		sum += reinterpret_cast<double*>(result.data)[i]*reinterpret_cast<double*>(result.data)[i];
	}

	result /= std::sqrt(sum);

	return result;
}





cv::Mat GBVS::subsample(const cv::Mat& img) const {
	int w, h, wr, hr;
	w = img.cols;
	h = img.rows;
	wr = w / 2; if ( wr == 0 ) wr = 1;
	hr = h / 2; if ( hr == 0 ) hr = 1;

	cv::Mat decx (wr, h, CV_64FC1, 1);	// /!\ Do not forget to transpose the coordinate of the matrix.... 
	cv::Mat decxy(wr, hr, CV_64FC1, 1);

	if ( (w > 10) && (h > 10) ) {  
		// we need to transpose the input data as OpenCV and MATLAB do not use the same order for data
		// Indeed, MATLAB pointer gives data by column, OpenCV gives it by row
		// And I copy/pasted the original MEX code... 
		cv::Mat tr = transpose(img);

		lowPass6xDecX(reinterpret_cast<double*>(tr.data), reinterpret_cast<double*>(decx.data), w, h);
		lowPass6yDecY(reinterpret_cast<double*>(decx.data), reinterpret_cast<double*>(decxy.data), wr, h);

		return transpose(decxy);
	} else {
		return img;
	}

}




cv::Mat GBVS::getFloatingImageC3(const cv::Mat &input) const {
	cv::Mat floatColorImage(input.rows, input.cols, CV_64FC3, input.channels());


	for(int i = 0 ; i < input.rows ; ++i) {
		for(int j = 0 ; j < input.cols ; ++j) {
			const cv::Point3_<uchar> &bgr   = input.at< cv::Point3_<uchar> >(i, j);
			      cv::Point3_<double> &bgrF = floatColorImage.at< cv::Point3_<double> >(i, j);

			bgrF.x = static_cast<double>(bgr.x) / 255.f;
			bgrF.y = static_cast<double>(bgr.y) / 255.f;
			bgrF.z = static_cast<double>(bgr.z) / 255.f;

		}
	}

	return floatColorImage;
}




cv::Mat GBVS::getUCharImageC1(const cv::Mat &input) const {
	cv::Mat ucharImage(input.rows, input.cols, CV_8UC1, input.channels());


	for(int i = 0 ; i < input.rows*input.cols ; ++i) {
		reinterpret_cast<unsigned char*>(ucharImage.data)[i] = static_cast<unsigned char>(std::fmin(255, 255*reinterpret_cast<double*>(input.data)[i]));
	}

	return ucharImage;
}





// this computes feature maps for each cannnel in featureChannels/
void GBVS::getFeatureMaps(const cv::Mat& img) {

	// this computes feature maps for each cannnel in featureChannels/
	bool isColor = img.channels() == 3;

	// -----------------------------------------------------------------------------
	// STEP 1 : form image pyramid and prune levels if pyramid levels get too small.

	cv::Mat imgr;
	cv::Mat imgg;
	cv::Mat imgb;
	cv::Mat imgi;

	if(isColor) {
		imgr = cv::Mat(img.rows, img.cols, CV_64FC1, 1);
		imgg = cv::Mat(img.rows, img.cols, CV_64FC1, 1);
		imgb = cv::Mat(img.rows, img.cols, CV_64FC1, 1);
		imgi = cv::Mat(img.rows, img.cols, CV_64FC1, 1);

		for(int i = 0 ; i < img.rows ; ++i) {
			for(int j = 0 ; j < img.cols ; ++j) {
				const cv::Point3_<double> &bgr   = img.at< cv::Point3_<double> >(i, j);
				  
				reinterpret_cast<double*>(imgr.data)[i*img.cols+j] = bgr.z;
				reinterpret_cast<double*>(imgg.data)[i*img.cols+j] = bgr.y;
				reinterpret_cast<double*>(imgb.data)[i*img.cols+j] = bgr.x;
				reinterpret_cast<double*>(imgi.data)[i*img.cols+j] = std::fmax(bgr.x, std::fmax(bgr.y, bgr.z));
			}
		}
	} else {
		imgi = img.clone();
	}


	// -----------------------------------------------------------------------------
	// custom step. Do we use a CSF on the luminance? 
	// replace max(R,G,B) by L from Luv and apply a CSF


	if(useCSF)
		imgi = applyCSF(img);


	// -----------------------------------------------------------------------------


	std::vector<cv::Mat> imgL, imgR, imgG, imgB;
	imgL.push_back(subsample(imgi));
	imgR.push_back(subsample(imgr));
	imgG.push_back(subsample(imgg));
	imgB.push_back(subsample(imgb));

	for(int i = 1 ; i < maxcomputelevel ; ++i) {

		imgL.push_back(subsample(imgL.back()));


		if(isColor) {
			imgR.push_back(subsample(imgR.back()));
			imgG.push_back(subsample(imgG.back()));
			imgB.push_back(subsample(imgB.back()));
		} else {
			imgR.push_back(cv::Mat());
			imgG.push_back(cv::Mat());
			imgB.push_back(cv::Mat());
		}

		if(imgL.back().cols < 3 || imgL.back().rows < 3) {
			std::cerr << "[I] reached minimum size at level " << i << "cutting off additional levels\n";
			std::vector<int> ll;
			for(int j = 0 ; j < i+1 ; ++j) {
				ll.push_back(levels.at(i));
			}

			levels = ll;
			maxcomputelevel = i;
			break;
		}
	}

	// //TODO: full model needs to support motion!


	// -----------------------------------------------------------------------------
	// STEP 2 : compute feature maps

	int channelProcessed = 0;
	for(size_t i = 0 ; i < channels.length() ; ++i) {

		switch(channels[i]) {
			case 'D':
				if(!isColor) {
					std::cerr << "[I] cannot compute color feature on B&W image. Skip that feature." << std::endl;
					break;
				}

				{
					Feature f;
					f.description = "DKL Luminosity Channel";
					f.weight = dklcolorWeight;
					f.channel = channelProcessed;

					for (int lev = 0 ; lev < static_cast<int>(levels.size()) ; ++lev) {
						cv::Mat KL, KC1, KC2;

						rgb2dkl(imgR[levels[lev]-1], imgG[levels[lev]-1], imgB[levels[lev]-1], KL, KC1, KC2);

						FeatureMap fm1;
						fm1.type  = 0;
						fm1.level = lev; 
						fm1.channel = channelProcessed;
						cv::resize(KL, fm1.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);
						
						
						FeatureMap fm2;
						fm2.type  = 1;
						fm2.level = lev; 
						fm2.channel = channelProcessed;
						cv::resize(KC1, fm2.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);


						FeatureMap fm3;
						fm3.type  = 2;
						fm3.level = lev; 
						fm3.channel = channelProcessed;
						cv::resize(KC2, fm3.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);

						
						f.maps.push_back(fm1);
						f.maps.push_back(fm2);
						f.maps.push_back(fm3);
					}

					features.push_back(f);
				}

				++channelProcessed;

				break;

			case 'I':

				{
					Feature f;
					f.description = "Intensity";
					f.weight = intensityWeight;
					f.channel = channelProcessed;

					for (int lev = 0 ; lev < static_cast<int>(levels.size()) ; ++lev) {
						FeatureMap fm1;
						fm1.type  = 0;
						fm1.level = lev; 
						fm1.channel = channelProcessed;

						cv::resize(imgL[levels[lev]-1], fm1.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);
						
						f.maps.push_back(fm1);

						

					}

					features.push_back(f);
				}
				
				++channelProcessed;

				break;

			case 'O':

				{
					Feature f;
					f.description = "Gabor filtering";
					f.weight = orientationWeight;
					f.channel = channelProcessed;

					for (int lev = 0 ; lev < static_cast<int>(levels.size()) ; ++lev) {

							
						for(int o = 0 ; o < static_cast<int>(gaborFilters.size()) ; ++ o) {
							FeatureMap fm;
							fm.type  = o;
							fm.level = lev;
							fm.channel = channelProcessed;

							cv::Mat f0, f90;
							cv::filter2D(imgL[levels[lev]-1], f0, CV_64F,  gaborFilters[o].g0,  cv::Point(-1, -1), 0, cv::BORDER_DEFAULT );
							cv::filter2D(imgL[levels[lev]-1], f90, CV_64F, gaborFilters[o].g90, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT );

							cv::Mat map = cv::abs(f0) + cv::abs(f90);


							attenuateBordersGBVS(map, 13);


							cv::resize(map, fm.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);
							
							f.maps.push_back(fm);
						}
					}

					features.push_back(f);
				}

				++channelProcessed;

				break;


			case 'R':

				{
					Feature f;
					f.description = "Contrast feature";
					f.weight = contrastWeight;
					f.channel = channelProcessed;

					for (int lev = 0 ; lev < static_cast<int>(levels.size()) ; ++lev) {

						FeatureMap fm;
						fm.type  = 0;
						fm.level = lev;
						fm.channel = channelProcessed;

						cv::Mat contr = contrast(imgL[levels[lev]-1], static_cast<int>(std::round(imgL[levels[lev]-1].rows * contrastwidth)));

						cv::resize(contr, fm.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);
						f.maps.push_back(fm);

					}

					features.push_back(f);

				}

				++channelProcessed;

				break;


			case 'C':
				if(!isColor) {
					std::cerr << "[I] cannot compute color feature on B&W image. Skip that feature." << std::endl;
					break;
				}

				{
					Feature f;
					f.description = "Color features";
					f.weight = colorWeight;
					f.channel = channelProcessed;

					for (int lev = 0 ; lev < static_cast<int>(levels.size()) ; ++lev) {



						// ------------------------------------------------------------------------------
						// First feature... 
						FeatureMap fm;
						fm.type  = 0;
						fm.level = lev;
						fm.channel = channelProcessed;

						cv::Mat diff(imgL[levels[lev]-1].rows, imgL[levels[lev]-1].cols, CV_64FC1, cvScalar(0.f)); 

						for(int i = 0 ; i < diff.rows ; ++i) {
							for(int j = 0 ; j < diff.rows ; ++j) {
								diff.at<double>(i,j) = std::abs(imgB[levels[lev]-1].at<double>(i,j) - std::min(imgR[levels[lev]-1].at<double>(i,j), imgG[levels[lev]-1].at<double>(i,j)));
							}
						}


						diff = safeDivideGBVS(diff, imgL[levels[lev]-1]);
						cv::resize(diff, fm.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);
						f.maps.push_back(fm);




						// ------------------------------------------------------------------------------
						// Second feature... 
						FeatureMap fm2;
						fm2.type  = 1;
						fm2.level = lev;
						fm2.channel = channelProcessed;

						for(int i = 0 ; i < diff.rows ; ++i) {
							for(int j = 0 ; j < diff.rows ; ++j) {
								diff.at<double>(i,j) = std::abs(imgR[levels[lev]-1].at<double>(i,j) - imgG[levels[lev]-1].at<double>(i,j));
							}
						}


						diff = safeDivideGBVS(diff, imgL[levels[lev]-1]);
						cv::resize(diff, fm2.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);
						

						f.maps.push_back(fm2);

					}

					features.push_back(f);

				}


				++channelProcessed;

				break;


			case 'P':

#ifdef WITH_LINPER
				{
					Feature f;
					f.description = "Linear perspective";
					f.channel = channelProcessed;

					std::vector<double> reliability(4);
					std::vector<cv::Mat> outputs(4);
					boost::thread_group g;
					for(size_t i = 0 ; i < outputs.size() ; ++i) {
						g.create_thread(boost::bind(&GBVS::getPerspectiveFeatureJob, this, boost::ref(img), boost::ref(outputs[i]), &reliability[i]));
					}
					g.join_all();

					for(size_t i = 1 ; i < outputs.size() ; ++i) {
						outputs[0] += outputs[i];
						reliability[0] += reliability[i];
					}
					outputs[0] /= static_cast<double>(outputs.size());
					reliability[0] /= static_cast<double>(outputs.size());

					FeatureMap fm;
					fm.type  = 0;
					fm.level = 0;
					fm.channel = channelProcessed;

					if(reliability[0] < 0.0015) {	// if the feature map does not contains much information, drop it.
						fm.map = cv::Mat(cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), CV_64FC1, cv::Scalar(0.f));
					} else {
						cv::resize(outputs[0], fm.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);
					}

					f.maps.push_back(fm);
					if(reliability[0] >= 0.0038)
						f.weight = std::min(100.f, std::max(0.f, static_cast<float>(-2.824f + 3.105f * exp(662.373f * reliability[0])))) / 30.f;
					else
						f.weight = std::min(100.f, std::max(0.f, static_cast<float>(-3.276f + 2.936f * exp(689.485f * reliability[0])))) / 70.f;
						
					features.push_back(f);
					++channelProcessed;
				}
#else
					std::cerr << "[I] GBVS was not compiled with the Linear perspective module. Skipped." << std::endl;

#endif
			break;


			case 'F':

				{
					Feature f;
					f.description = "Face detector";
					f.weight = faceFeatureWeight;
					f.channel = channelProcessed;

					cv::Mat featureMap = faceFeaturesDectection(imgi);

					if(!featureMap.empty()) {
						FeatureMap fm;
						fm.type  = 0;
						fm.level = 0;
						fm.channel = channelProcessed;

						cv::resize(featureMap, fm.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);
						f.maps.push_back(fm);


						features.push_back(f);
						++channelProcessed;
					}

				}

			break;


			case 'B':

				{
					Feature f;
					f.description = "Blur map";
					f.weight = blurFeatureWeight;
					f.channel = channelProcessed;

					cv::Mat featureMap = defocusBlurMap(imgi);

					FeatureMap fm;
					fm.type  = 0;
					fm.level = 0;
					fm.channel = channelProcessed;

					cv::resize(featureMap, fm.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);
					f.maps.push_back(fm);


					features.push_back(f);
					++channelProcessed;
				}

			break;


			case 'S': 
			
				{
					Feature f = segmentationFeature(img);
					f.channel = channelProcessed;
					for(std::list<FeatureMap>::iterator it = f.maps.begin() ; it !=  f.maps.end() ; ++it) {
						it->channel = channelProcessed;
					}
					features.push_back(f);
					++channelProcessed;
				}



			default:
			
				break;
		}
	}
}

void GBVS::getPerspectiveFeatureJob(const cv::Mat& img, cv::Mat &output, double *reliability) const {
	output = vanishingLineFeatureMapF64C3(img, reliability);
}



cv::Mat GBVS::defocusBlurMap(const cv::Mat& image) const {
	cv::Mat gray;

	// -------------------------------------------------------------------------------------------
	// check user input


	if(image.channels() == 3 && image.type() == CV_8UC3) {
		cv::cvtColor(image, gray, CV_BGR2GRAY);
	} else {
		if(image.channels() == 1 && image.type() == CV_32FC1) {
			gray = cv::Mat(image.rows, image.cols, CV_8UC1, cv::Scalar(0));
			for(int i = 0 ; i < image.rows*image.cols ; ++i) {
				gray.data[i] = static_cast<unsigned char>(std::max(0.f, std::min(255.f, reinterpret_cast<float*>(image.data)[i] * 255.f)));
			}
		}
		if(image.channels() == 1 && image.type() == CV_64FC1) {
			gray = cv::Mat(image.rows, image.cols, CV_8UC1, cv::Scalar(0));
			for(int i = 0 ; i < image.rows*image.cols ; ++i) {
				gray.data[i] = static_cast<unsigned char>(std::max(0., std::min(255., reinterpret_cast<double*>(image.data)[i] * 255.)));
			}
		}

		if(gray.empty()) {
			throw std::logic_error(std::string("GBVS::defocusBlurMap(): Do not know what to do with this input image...\n"));
		}
	}


	// -------------------------------------------------------------------------------------------
	// apply a Gaussian blur to the original image

	cv::Mat blurredGray;
	cv::GaussianBlur(gray, blurredGray, cv::Size(7,7), 5, 5, cv::BORDER_REPLICATE);	


	// -------------------------------------------------------------------------------------------
	// measure the effect of the blur 

	cv::Mat blurMap (gray.rows, gray.cols, CV_64FC1, cv::Scalar(0));
	for(int i = 0 ; i < gray.rows ; ++i) {
		for(int j = 0 ; j < gray.cols ; ++j) {
			if(gray.at<unsigned char>(i,j) > 0)
				blurMap.at<double>(i,j) = static_cast<float>(blurredGray.at<unsigned char>(i,j)) / static_cast<float>(gray.at<unsigned char>(i,j));
		}
	}

	double mn, mx;
	cv::minMaxLoc(blurMap, &mn, &mx);

	return (blurMap-mn)/(mx-mn);
}



cv::Mat GBVS::faceFeaturesDectection(const cv::Mat& image) {

	cv::Mat gray;

	// -------------------------------------------------------------------------------------------
	// check user input


	if(image.channels() == 3 && image.type() == CV_8UC3) {
		cv::cvtColor(image, gray, CV_BGR2GRAY);
	} else {
		if(image.channels() == 1 && image.type() == CV_32FC1) {
			gray = cv::Mat(image.rows, image.cols, CV_8UC1, cv::Scalar(0));
			for(int i = 0 ; i < image.rows*image.cols ; ++i) {
				gray.data[i] = static_cast<unsigned char>(std::max(0.f, std::min(255.f, reinterpret_cast<float*>(image.data)[i] * 255.f)));
			}
		}
		if(image.channels() == 1 && image.type() == CV_64FC1) {
			gray = cv::Mat(image.rows, image.cols, CV_8UC1, cv::Scalar(0));
			for(int i = 0 ; i < image.rows*image.cols ; ++i) {
				gray.data[i] = static_cast<unsigned char>(std::max(0., std::min(255., reinterpret_cast<double*>(image.data)[i] * 255.)));
			}
		}

		if(gray.empty()) {
			throw std::logic_error(std::string("GBVS::faceFeaturesDectection(): Do not know what to do with this input image...\n"));
		}
	}


	// -------------------------------------------------------------------------------------------
	// run face feature detector

		
	if(!initCascadeClassifier) {
		initCascadeClassifier = true;

		if(face_cascade.load("haarcascade_frontalface_alt.xml")) {
			faceCascadeEnabled = true;
		} else {
			std::cerr<< "[I] cannot open: haarcascade_frontalface_alt.xml" << std::endl;
		}

		if(eye_cascade.load("haarcascade_eye_tree_eyeglasses.xml")) {
			eyeCascadeEnabled = true;
		} else {
			std::cerr<< "[I] cannot open: haarcascade_eye_tree_eyeglasses.xml" << std::endl;
		}
	}

	bool faceFound = false;

	cv::Mat featureMap (image.rows, image.cols, CV_64FC1, cv::Scalar(0));
	std::vector<cv::Rect> faceFeatures;
	if(faceCascadeEnabled)
		face_cascade.detectMultiScale( gray, faceFeatures, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30) );

	if(faceFeatures.size() > 0) faceFound = true;

	for (size_t k = 0; k < faceFeatures.size(); ++k) {
        // cv::rectangle(gray, faceFeatures[i], cv::Scalar(0,255,0));

        float sigma_x =  0.001f*faceFeatures[k].width;
        float sigma_y = 0.001f*faceFeatures[k].height;

        for(int i = 0 ; i < faceFeatures[k].height ; ++i) {
            for(int j = 0 ; j < faceFeatures[k].width ; ++j) {

                float fi = static_cast<float>(i - faceFeatures[k].height / 2) / faceFeatures[k].height;
                float fj = static_cast<float>(j - faceFeatures[k].width  / 2) / faceFeatures[k].width;
                featureMap.at<double>(i+faceFeatures[k].y,j+faceFeatures[k].x) = std::max(featureMap.at<double>(i+faceFeatures[k].y,j+faceFeatures[k].x), static_cast<double>(std::exp(-((fj*fj)/sigma_x + (fi*fi)/sigma_y))));
            }
        }
    }

    faceFeatures.clear();
	if(faceCascadeEnabled)
		eye_cascade.detectMultiScale( gray, faceFeatures, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30) );

	if(faceFeatures.size() > 0) faceFound = true;

	for (size_t k = 0; k < faceFeatures.size(); ++k) {
        float sigma_x =  0.05f*faceFeatures[k].width;
        float sigma_y = 0.05f*faceFeatures[k].height;

        for(int i = 0 ; i < faceFeatures[k].height ; ++i) {
            for(int j = 0 ; j < faceFeatures[k].width ; ++j) {

                float fi = static_cast<float>(i - faceFeatures[k].height / 2) / faceFeatures[k].height;
                float fj = static_cast<float>(j - faceFeatures[k].width  / 2) / faceFeatures[k].width;
                featureMap.at<double>(i+faceFeatures[k].y,j+faceFeatures[k].x) = std::max(featureMap.at<double>(i+faceFeatures[k].y,j+faceFeatures[k].x), static_cast<double>(std::exp(-((fj*fj)/sigma_x + (fi*fi)/sigma_y))));
            }
        }
    }

    if(faceFound) {
    	double mx, mn;
		cv::minMaxLoc(featureMap, &mn, &mx);

	    // cv::imshow("feature", (featureMap-mn)/(mx-mn));
	    // cv::waitKey();
	    
    } 

    return featureMap;

}

#ifdef WITH_MEAN_SHIFT

Feature GBVS::segmentationFeature (const cv::Mat& image) const {
	cv::Mat imgW;
	image.convertTo(imgW, CV_32FC3);

	cv::resize(imgW, imgW, cv::Size(1000, static_cast<int>(imgW.rows * 1000.f / imgW.cols)));

	cv::Mat labels;
	double maxCluster = imgW.cols*imgW.rows;
	double prevCluster = imgW.cols*imgW.rows+1;
	// std::cout << "enter seg" << std::endl;
	while(prevCluster != maxCluster) {
		prevCluster = maxCluster;
		msseg_f(imgW, 3, 3, 200, labels);
		
		cv::minMaxLoc(labels, NULL, &maxCluster);
		// std::cout << maxCluster << std::endl;
	}

	// cv::imshow("segmented image", imgW);
	

	cv::Mat imgr(imgW.rows, imgW.cols, CV_64FC1, cv::Scalar(0));
	cv::Mat imgg(imgW.rows, imgW.cols, CV_64FC1, cv::Scalar(0));
	cv::Mat imgb(imgW.rows, imgW.cols, CV_64FC1, cv::Scalar(0));
	cv::Mat imgi(imgW.rows, imgW.cols, CV_64FC1, cv::Scalar(0));

	for(int i = 0 ; i < imgW.rows ; ++i) {
		for(int j = 0 ; j < imgW.cols ; ++j) {
			const cv::Point3_<float> &bgr   = imgW.at< cv::Point3_<float> >(i, j);
				
			reinterpret_cast<double*>(imgr.data)[i*imgW.cols+j] = static_cast<double>(bgr.z);
			reinterpret_cast<double*>(imgg.data)[i*imgW.cols+j] = static_cast<double>(bgr.y);
			reinterpret_cast<double*>(imgb.data)[i*imgW.cols+j] = static_cast<double>(bgr.x);
			reinterpret_cast<double*>(imgi.data)[i*imgW.cols+j] = static_cast<double>(std::fmax(bgr.x, std::fmax(bgr.y, bgr.z)));
		}
	}


	Feature f;
	f.weight = 1.f;
	cv::Mat KL, KC1, KC2;

	// Apply the DKL color space to the segmented image, so we compute the color feature on an "object"-based level 
	rgb2dkl(imgr, imgg, imgb, KL, KC1, KC2);

	// cv::imshow("KL", KL);
	// cv::imshow("KC1", KC1);
	// cv::imshow("KC2", KC2);
	// cv::waitKey();

	FeatureMap fm1;
	fm1.type  = 0;
	fm1.level = 0; 
	fm1.channel = -1; // will be initialized later... 
	cv::resize(KL, fm1.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);
	
	
	FeatureMap fm2;
	fm2.type  = 1;
	fm2.level = 0; 
	fm2.channel = -1;
	cv::resize(KC1, fm2.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);


	FeatureMap fm3;
	fm3.type  = 2;
	fm3.level = 0; 
	fm3.channel = -1;
	cv::resize(KC2, fm3.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);


	FeatureMap fm4;
	fm4.type  = 3;
	fm4.level = 0; 
	fm4.channel = -1;
	cv::resize(imgi, fm4.map, cv::Size(salmapmaxsize_v[1], salmapmaxsize_v[0]), 0, 0, cv::INTER_AREA);

	f.maps.push_back(fm1);
	f.maps.push_back(fm2);
	f.maps.push_back(fm3);
	f.maps.push_back(fm4);

	return f;
}

#else

Feature GBVS::segmentationFeature (const cv::Mat& ) const {
	std::cout << "[I] Support for segmentation require the project to be compiled with mean-shift support. Return empty feature. " << std::endl;
	return Feature();
}

#endif


// ==============================================================================================================================
// Model related function








void GBVS::computeActivation() {


	mapWeights.resize(channels.size(), 1.f);

	// --------------- single threaded version ----------------

	// for(std::list<Feature>::iterator it = features.begin() ; it != features.end() ; ++it) {
	// 	mapWeights[it->channel] = it->weight;

	// 	int maxType = 0;
	// 	for(std::list<FeatureMap>::iterator mapIt = it->maps.begin() ; mapIt != it->maps.end() ; ++mapIt) {
	// 		maxType = std::max(maxType, mapIt->type);
	// 	}

	// 	for(int typei = 0 ; typei <= maxType ; ++typei) {
	// 		for(std::list<FeatureMap>::iterator mapIt = it->maps.begin() ; mapIt != it->maps.end() ; ++mapIt) {
	// 			if(mapIt->type != typei) continue;

	// 			// if the channel contains the face detection, we need to check if the map is not empty. If so, we will drop that channel as there is no information in it.
	// 			if(channels[it->channel] == 'F') {
	// 				double mx, mn;
	// 				cv::minMaxLoc(mapIt->map, &mn, &mx);
	// 				if((mx-mn)<0.000001) {
	// 					continue;
	// 				}
	// 			}


	// 			allmaps.push_back(FeatureMap());

	// 			allmaps.back().map = graphsalapply(mapIt->map, grframe, sigma_frac_act, 1, 2, static_cast<float>(tol));

	// 			allmaps.back().type = mapIt->type;
	// 			allmaps.back().level = mapIt->level;
	// 			allmaps.back().channel = mapIt->channel;

	// 		}
	// 	}
	// }


	// --------------- parallel version ----------------

	for(std::list<Feature>::iterator it = features.begin() ; it != features.end() ; ++it) {
		mapWeights[it->channel] = it->weight;

		int maxType = 0;
		for(std::list<FeatureMap>::iterator mapIt = it->maps.begin() ; mapIt != it->maps.end() ; ++mapIt) {
			maxType = std::max(maxType, mapIt->type);
		}

		// allocate memory
		for(int typei = 0 ; typei <= maxType ; ++typei) {
			for(std::list<FeatureMap>::iterator mapIt = it->maps.begin() ; mapIt != it->maps.end() ; ++mapIt) {
				if(mapIt->type != typei) continue;
					
				if(channels[it->channel] == 'F' || channels[it->channel] == 'P') {
					double mx, mn;
					cv::minMaxLoc(mapIt->map, &mn, &mx);
					if((mx-mn)<0.000001) {
						continue;
					}
				}

				allmaps.push_back(FeatureMap());
				allmaps.back().map = mapIt->map;
				allmaps.back().type = mapIt->type;
				allmaps.back().level = mapIt->level;
				allmaps.back().channel = mapIt->channel;
				allmaps.back().taskDone = false;
			}
		}
	}

	// run the activation in N threads.
	boost::thread_group g;
	for(int i = 0 ; i < nbThreads ; ++i) {
		g.create_thread(boost::bind(&GBVS::computeActivationJob, this));
	}

	g.join_all();

}

void GBVS::computeActivationJob() {

	bool taskFound = true;

	// while the thread can find something to do, do it.
	while(taskFound) {
		taskFound = false;
		FeatureMap *feature = NULL;

		gbvs_mutex.lock();
		for(std::list<FeatureMap>::iterator it = allmaps.begin() ; it != allmaps.end() && !taskFound ; ++it) {
			if(!it->taskDone) {
				taskFound = true;
				it->taskDone = true; // mark it as done. So other threads don't take this job
				feature = &(*it);
				break;
			}
		}

		gbvs_mutex.unlock();

		// if a task was found, do the job.
		if(taskFound) {
			feature->map = graphsalapply(feature->map, grframe, sigma_frac_act, 1, 2, static_cast<float>(tol));
		}
	}
}




void GBVS::normalizeActivation() {


	// --------------- single threaded version ----------------

	// for(std::list<FeatureMap>::iterator mapIt = allmaps.begin() ; mapIt != allmaps.end() ; ++mapIt) {
	// 	if(normalizationType == 1) {
	// 		mapIt->map = graphsalapply(mapIt->map, grframe, sigma_frac_act, num_norm_iters, 4, static_cast<float>(tol));
	// 	} else if (normalizationType == 2) {
	// 		mapIt->map = graphsalapply(mapIt->map, grframe, sigma_frac_act, num_norm_iters, 1, static_cast<float>(tol));
	// 	} else {
	// 		mapIt->map = maxNormalizeStdGBVS(mapIt->map);
	// 	}
	// }


	// --------------- parallel version ----------------

	// mark all feature maps as non-activated
	for(std::list<FeatureMap>::iterator mapIt = allmaps.begin() ; mapIt != allmaps.end() ; ++mapIt) {
		mapIt->taskDone = false;
	}


	// run the normalization in N threads.
	boost::thread_group g;
	for(int i = 0 ; i < nbThreads ; ++i) {
		g.create_thread(boost::bind(&GBVS::normalizeActivationJob, this));
	}

	g.join_all();

}


void GBVS::normalizeActivationJob() {
	bool taskFound = true;

	// while the thread can find something to do, do it.
	while(taskFound) {
		taskFound = false;
		FeatureMap *feature = NULL;


		gbvs_mutex.lock();
		for(std::list<FeatureMap>::iterator it = allmaps.begin() ; it != allmaps.end()  && !taskFound  ; ++it) {
			if(!it->taskDone) {
				taskFound = true;
				it->taskDone = true; // mark it as done. So other threads don't take this job
				feature = &(*it);
				break;
			}
		}
		gbvs_mutex.unlock();



		// if a task was found, do the job.
		if(taskFound) {
			if(normalizationType == 1) {
				feature->map = graphsalapply(feature->map, grframe, sigma_frac_act, num_norm_iters, 4, static_cast<float>(tol));
			} else if (normalizationType == 2) {
				feature->map = graphsalapply(feature->map, grframe, sigma_frac_act, num_norm_iters, 1, static_cast<float>(tol));
			} else {
				feature->map = maxNormalizeStdGBVS(feature->map);
			}			
		}
	}
}


void GBVS::averageByFeatureChannel() {
	std::vector<int> nfmap(channels.length(), 0);

	for(std::list<FeatureMap>::iterator mapIt = allmaps.begin() ; mapIt != allmaps.end() ; ++mapIt) {
		std::list<FeatureMap>::iterator channelIt;
		for(channelIt = channelMaps.begin() ; channelIt != channelMaps.end() ; ++channelIt) {
			if(channelIt->channel == mapIt->channel) {
				channelIt->map = channelIt->map + mapIt->map;
				++nfmap[channelIt->channel];
				break;
			}
		}

		if(channelIt == channelMaps.end()) {
			channelMaps.push_back(*mapIt);
			nfmap[mapIt->channel] = 1;
		}
	}

	for(std::list<FeatureMap>::iterator channelIt = channelMaps.begin() ; channelIt != channelMaps.end() ; ++channelIt) {

		if(normalizeTopChannelMaps == 1) {
			if(normalizationType == 1) {
				channelIt->map = graphsalapply(channelIt->map, grframe, sigma_frac_act, num_norm_iters, 4, static_cast<float>(tol));
			} else if (normalizationType == 2) {
				channelIt->map = graphsalapply(channelIt->map, grframe, sigma_frac_act, num_norm_iters, 1, static_cast<float>(tol));
			} else {
				channelIt->map = maxNormalizeStdGBVS(channelIt->map);
			}
		}

	}
}





void GBVS::sumChannels(bool normalize) {
	std::list<FeatureMap>::iterator channelIt = channelMaps.begin();

	if(channelIt != channelMaps.end()) {
		master_map = channelIt->map * mapWeights[channelIt->channel];
		++channelIt;
	}

	for( ; channelIt != channelMaps.end() ; ++channelIt) {

		if(channels[channelIt->channel] == 'F' || channels[channelIt->channel] == 'P' ) { // || channels[channelIt->channel] == 'P'
			double mn, mx;
			cv::minMaxLoc(master_map, &mn, &mx);

			double mnf, mxf;
			cv::minMaxLoc(channelIt->map, &mnf, &mxf);
			if(channels[channelIt->channel] == 'F') {
				master_map += (((channelIt->map - mnf) / (mxf - mnf)) * (mx - mn) + mn)  * mapWeights[channelIt->channel];
			} else {
				float max_val = (mx - mn)/.5f - mx;
				if(mxf * mapWeights[channelIt->channel] > max_val) {
					master_map += channelIt->map * (max_val / mxf);
				} else {
					master_map += channelIt->map * mapWeights[channelIt->channel];
				}
			}


		} else {
			master_map += channelIt->map * mapWeights[channelIt->channel];
		}
	}

	attenuateBordersGBVS(master_map, 4);

	if(normalize) {
		double mn = 0;
		double mx = 0;
		cv::minMaxLoc(master_map, &mn, &mx);

		master_map = (master_map-mn)/(mx-mn);
	}

}






cv::Mat GBVS::maxNormalizeStdGBVS(const cv::Mat &map) const {
	cv::Mat result(map.rows, map.cols, CV_64FC1, cvScalar(0.f)); 

	for(int i = 0 ; i < map.rows ; ++i) {
		for(int j = 0 ; j < map.cols ; ++i) {
			result.at<double>(i,j) = static_cast<int>(std::fmin(255.f, map.at<double>(i,j)*10));
		}
	}

	// get the mean value of the local maxima:
	double lm_sum = 0.f; 
	double lm_num = 0.f;
	double lm_avg = 0.f;

	double thresh = 1.f;

	for (int j = 1; j < map.rows - 1; ++j) {
		for (int i = 1; i < map.cols - 1; ++i) {
	    double val = map.at<double>(j,i);

	    if (val >= thresh &&
	        val >= map.at<double>(j,i-1) &&
	        val >= map.at<double>(j,i+1) &&
	        val >= map.at<double>(j+1,i) &&
	        val >= map.at<double>(j-1,i)) {

		        lm_sum += val;
		        lm_num++;
	    	}
		}
	}

	if(lm_sum > 0) 
		lm_avg = lm_sum / lm_num;

	if(lm_num > 1)
		result = result * (10 - lm_avg)*(10 - lm_avg);
	else if (lm_num == 1) {
		result = result * 10 * 10;
	}  

	return result;
}





void GBVS::blurMasterMap(bool normalize) {
	if(blurfrac > 0) {
		cv::Mat blurredMap;
		int mxSize = std::max(master_map.cols, master_map.rows);
		cv::GaussianBlur(master_map, blurredMap, cv::Size(3,3), mxSize*blurfrac, mxSize*blurfrac, cv::BORDER_REPLICATE);

		double mn, mx;
		if(normalize) {
			cv::minMaxLoc(blurredMap, &mn, &mx);
			master_map = (blurredMap-mn)/(mx-mn);
		} else {
			master_map = blurredMap;
		}
	}
}






// ==============================================================================================================================
// Equatorial prior






float GBVS::faceLine(const cv::Mat &image) const {
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
		std::cerr<< "[I] cannot open: haarcascade_eye_tree_eyeglasses.xml" << std::endl;
	}


	// ------------------------------------------------------------------------------------------------
	// find features

	std::list<cv::Rect> allFeatures;
	std::vector<cv::Rect> faceFeatures;
	if(faceCascadeEnabled)
		face_cascade.detectMultiScale( gray, faceFeatures, 2, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(15, 15) );

	for(size_t i = 0 ; i < faceFeatures.size() ; ++i) {
		allFeatures.push_back(faceFeatures[i]);
	}

	faceFeatures.clear();
	if(faceProfilCascadeEnabled)
		faceProfil_cascade.detectMultiScale( gray, faceFeatures, 2, 2, 0|CV_HAAR_SCALE_IMAGE, cv::Size(15, 15) );

	for(size_t i = 0 ; i < faceFeatures.size() ; ++i) {
		allFeatures.push_back(faceFeatures[i]);
	}

	if(allFeatures.size() < 2) return -1;


	float mnX = static_cast<float>(image.cols);
	float mxX = 0;
	for( std::list<cv::Rect>::iterator it = allFeatures.begin() ; it != allFeatures.end() ; ++it ) {
		float x = static_cast<float>(it->x + it->width /2);

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
		float y = static_cast<float>(it->y + it->height / 2);

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


float GBVS::salientCenter(const cv::Mat& image, int step) const {
	
	std::vector<float> histogram(180/step);
	int offset = 0; //static_cast<int>(30.f*(step/180.f));

	float sumHist = 0;
	for(int i = offset ; i < 180-offset ; i+=step) {
		float top    = image.rows*static_cast<float>(i)/180;
		float bottom = image.rows*static_cast<float>(i+step)/180;

		float sum = 0;
		for(int ii = static_cast<int>(top) ; ii < static_cast<int>(bottom) ; ++ii) {
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


void GBVS::applyGaussianEquatorialPrior(cv::Mat& image, float gaussianM, float gaussianSD) const {
	
	for(int i = 0 ; i < image.rows ; ++i) {
		float lat = (.5f - static_cast<float>(i)/image.rows) * 180.f;
		float prior =  (.01f + .40f * exp(-((lat-gaussianM)*(lat-gaussianM))/(gaussianSD)) + .75f * exp(-((lat-100)*(lat-100))/(200)) + .75f * exp(-((lat+90)*(lat+90))/(200))) / .41f;
		prior = std::min(1.f, prior);

		for(int j = 0 ; j < image.cols ; ++j) {
			image.at<float>(i,j) = prior * image.at<float>(i,j);
		}
	}	

}


void GBVS::applyEquatorialPrior(cv::Mat& image, const cv::Mat& colorImageInput) const {
	float scaling_factor = static_cast<float>(image.cols) / 1400.f;	// normalize the size of the images
	cv::Mat colorImage = colorImageInput.clone();

	cv::resize(colorImage, colorImage, cv::Size(static_cast<int>(colorImage.size().width/scaling_factor), static_cast<int>(colorImage.size().height/scaling_factor)));
	float fc = faceLine(colorImage);
	float slCenter = salientCenter(image);


	slCenter = std::max(std::min((slCenter - image.rows/2)/image.rows, 0.1f), -0.1f);
	slCenter = slCenter * image.rows + image.rows/2;
	slCenter = (slCenter/scaling_factor + colorImage.rows / 2) / 2;

	if(fc > 0) {
		if(fc > colorImage.rows / 3)	// prior: faces are not higher than the first third of the image
			slCenter = fc;
	}


	float equatorialLine = (.5f - slCenter / colorImage.rows) * 180.f;

	applyGaussianEquatorialPrior(image, equatorialLine);

}



// ==============================================================================================================================
// Model related function










void GBVS::attenuateBordersGBVS(cv::Mat &map, int borderSize) const {

	if (borderSize * 2 > map.rows) borderSize = (map.rows / 2); 
	if (borderSize * 2 > map.cols) borderSize = (map.cols / 2); 
	if (borderSize < 1) return; 

	for(int i = 0 ; i < map.rows ; ++i) {
		for(int j = 0 ; j < borderSize ; ++j) {
			map.at<double>(i,j) = map.at<double>(i,j) * (j+1) / (borderSize+1);
			map.at<double>(i, map.cols - 1 - j) = map.at<double>(i, map.cols - 1 - j) * (j+1) / (borderSize+1);
		}
	}

	for(int j = borderSize ; j < map.cols - borderSize ; ++j) {
		for(int i = 0 ; i < borderSize ; ++i) {
			map.at<double>(i,j) = map.at<double>(i,j) * (i+1) / (borderSize+1);
			map.at<double>(map.rows - 1 - i,j) = map.at<double>(map.rows - 1 - i,j) * (i+1) / (borderSize+1);
		}
	}
}









// MATLAB pointer gives data by column, OpenCV gives it by row.  
cv::Mat GBVS::transpose(const cv::Mat& input) const {
	cv::Mat transposedImg(input.cols, input.rows, CV_64FC1, input.channels());


	for(int i = 0 ; i < input.rows ; ++i) {
		for(int j = 0 ; j < input.cols ; ++j) {
			transposedImg.at< double >(j, i) = input.at< double >(i, j);
		}
	}

	return transposedImg;

}






cv::Mat GBVS::safeDivideGBVS(const cv::Mat &u, const cv::Mat &v) const {
	cv::Mat result = cv::Mat(u.rows, u.cols, CV_64FC1, 1);

	for(int i = 0 ; i < u.rows ; ++i) {
		for(int j = 0 ; j < u.cols ; ++j) {
			if(v.at<double>(i,j) == 0)
				result.at<double>(i,j) = u.at<double>(i,j);
			else
				result.at<double>(i,j) = u.at<double>(i,j) / v.at<double>(i,j);
		}
	}

	return result;

}



// The contrast function only compute the standard deviation of pixels in a bounding box defined by the second parameter... 
cv::Mat GBVS::contrast(const cv::Mat &img, int size) const {

	cv::Mat ctr(img.rows, img.cols, CV_64FC1, 1);

	int mo = size / 2;

	for(int i = 0 ; i < img.rows ; ++i) {
		for(int j = 0 ; j < img.cols ; ++j) {

			double sumPix = 0;
			double sumSqPix = 0;
			int ni = 0;

			for(int ii = i - mo ; ii < i + mo ; ++ii) {
				for(int jj = j - mo ; jj < j + mo ; ++jj) {
					if ( ii < 0 || ii >= img.rows || jj < 0 || jj >= img.cols ) continue;

					double v = img.at<double>(ii,jj);
					sumPix   += v;
					sumSqPix += v*v;

					++ni;
				}
			}

			if(ni > 0) {
				sumPix   /= ni;
				sumPix = (sumSqPix - ni * sumPix * sumPix) / ni;
			}

			ctr.at<double>(i,j) = sumPix;

		}
	}

	return ctr;

}




void GBVS::rgb2dkl(cv::Mat &imgR, cv::Mat &imgG, cv::Mat &imgB, cv::Mat &imgL, cv::Mat &imgC1, cv::Mat &imgC2) const {
	imgL  = cv::Mat(imgR.rows, imgR.cols, CV_64FC1, 1);
	imgC1 = cv::Mat(imgR.rows, imgR.cols, CV_64FC1, 1);
	imgC2 = cv::Mat(imgR.rows, imgR.cols, CV_64FC1, 1);

	double lut_rgb[256][3] = {
	    {0.024935, 0.0076954, 0.042291},
	    {0.024974, 0.0077395, 0.042346},
	    {0.025013, 0.0077836, 0.042401},
	    {0.025052, 0.0078277, 0.042456},
	    {0.025091, 0.0078717, 0.042511},
	    {0.02513, 0.0079158, 0.042566},
	    {0.025234, 0.007992, 0.042621},
	    {0.025338, 0.0080681, 0.042676},
	    {0.025442, 0.0081443, 0.042731},
	    {0.025545, 0.0082204, 0.042786},
	    {0.025649, 0.0082966, 0.042841},
	    {0.025747, 0.0084168, 0.042952},
	    {0.025844, 0.0085371, 0.043062},
	    {0.025942, 0.0086573, 0.043172},
	    {0.026039, 0.0087776, 0.043282},
	    {0.026136, 0.0088978, 0.043392},
	    {0.026234, 0.0090581, 0.043502},
	    {0.026331, 0.0092184, 0.043612},
	    {0.026429, 0.0093788, 0.043722},
	    {0.026526, 0.0095391, 0.043833},
	    {0.026623, 0.0096994, 0.043943},
	    {0.026818, 0.0099198, 0.044141},
	    {0.027013, 0.01014, 0.044339},
	    {0.027208, 0.010361, 0.044537},
	    {0.027403, 0.010581, 0.044736},
	    {0.027597, 0.010802, 0.044934},
	    {0.027857, 0.010994, 0.04522},
	    {0.028117, 0.011186, 0.045507},
	    {0.028377, 0.011379, 0.045793},
	    {0.028636, 0.011571, 0.046079},
	    {0.028896, 0.011764, 0.046366},
	    {0.029104, 0.012068, 0.046652},
	    {0.029312, 0.012373, 0.046938},
	    {0.029519, 0.012677, 0.047225},
	    {0.029727, 0.012982, 0.047511},
	    {0.029935, 0.013287, 0.047797},
	    {0.030273, 0.013663, 0.048326},
	    {0.03061, 0.01404, 0.048855},
	    {0.030948, 0.014417, 0.049383},
	    {0.031286, 0.014794, 0.049912},
	    {0.031623, 0.01517, 0.050441},
	    {0.032156, 0.015707, 0.051035},
	    {0.032688, 0.016244, 0.05163},
	    {0.033221, 0.016782, 0.052225},
	    {0.033753, 0.017319, 0.052819},
	    {0.034286, 0.017856, 0.053414},
	    {0.034961, 0.018693, 0.054493},
	    {0.035636, 0.019531, 0.055573},
	    {0.036312, 0.020369, 0.056652},
	    {0.036987, 0.021206, 0.057731},
	    {0.037662, 0.022044, 0.058811},
	    {0.038623, 0.023246, 0.060044},
	    {0.039584, 0.024449, 0.061278},
	    {0.040545, 0.025651, 0.062511},
	    {0.041506, 0.026854, 0.063744},
	    {0.042468, 0.028056, 0.064978},
	    {0.043857, 0.029659, 0.066806},
	    {0.045247, 0.031263, 0.068634},
	    {0.046636, 0.032866, 0.070463},
	    {0.048026, 0.034469, 0.072291},
	    {0.049416, 0.036072, 0.074119},
	    {0.051221, 0.038156, 0.076476},
	    {0.053026, 0.04024, 0.078833},
	    {0.054831, 0.042325, 0.081189},
	    {0.056636, 0.044409, 0.083546},
	    {0.058442, 0.046493, 0.085903},
	    {0.06039, 0.048737, 0.087996},
	    {0.062338, 0.050982, 0.090088},
	    {0.064286, 0.053226, 0.092181},
	    {0.066234, 0.055471, 0.094273},
	    {0.068182, 0.057715, 0.096366},
	    {0.070519, 0.06012, 0.098921},
	    {0.072857, 0.062525, 0.10148},
	    {0.075195, 0.06493, 0.10403},
	    {0.077532, 0.067335, 0.10659},
	    {0.07987, 0.069739, 0.10914},
	    {0.082208, 0.072345, 0.11176},
	    {0.084545, 0.07495, 0.11438},
	    {0.086883, 0.077555, 0.117},
	    {0.089221, 0.08016, 0.11963},
	    {0.091558, 0.082766, 0.12225},
	    {0.094026, 0.085611, 0.12533},
	    {0.096494, 0.088457, 0.12841},
	    {0.098961, 0.091303, 0.1315},
	    {0.10143, 0.094148, 0.13458},
	    {0.1039, 0.096994, 0.13767},
	    {0.10688, 0.10028, 0.14119},
	    {0.10987, 0.10357, 0.14471},
	    {0.11286, 0.10685, 0.14824},
	    {0.11584, 0.11014, 0.15176},
	    {0.11883, 0.11343, 0.15529},
	    {0.12208, 0.11695, 0.15903},
	    {0.12532, 0.12048, 0.16278},
	    {0.12857, 0.12401, 0.16652},
	    {0.13182, 0.12754, 0.17026},
	    {0.13506, 0.13106, 0.17401},
	    {0.1387, 0.13499, 0.17819},
	    {0.14234, 0.13892, 0.18238},
	    {0.14597, 0.14285, 0.18656},
	    {0.14961, 0.14677, 0.19075},
	    {0.15325, 0.1507, 0.19493},
	    {0.15727, 0.15519, 0.19956},
	    {0.1613, 0.15968, 0.20419},
	    {0.16532, 0.16417, 0.20881},
	    {0.16935, 0.16866, 0.21344},
	    {0.17338, 0.17315, 0.21806},
	    {0.17805, 0.17796, 0.22291},
	    {0.18273, 0.18277, 0.22775},
	    {0.1874, 0.18758, 0.2326},
	    {0.19208, 0.19238, 0.23744},
	    {0.19675, 0.19719, 0.24229},
	    {0.20156, 0.20224, 0.24758},
	    {0.20636, 0.20729, 0.25286},
	    {0.21117, 0.21234, 0.25815},
	    {0.21597, 0.21739, 0.26344},
	    {0.22078, 0.22244, 0.26872},
	    {0.2261, 0.22806, 0.27423},
	    {0.23143, 0.23367, 0.27974},
	    {0.23675, 0.23928, 0.28524},
	    {0.24208, 0.24489, 0.29075},
	    {0.2474, 0.2505, 0.29626},
	    {0.25299, 0.25651, 0.3022},
	    {0.25857, 0.26253, 0.30815},
	    {0.26416, 0.26854, 0.3141},
	    {0.26974, 0.27455, 0.32004},
	    {0.27532, 0.28056, 0.32599},
	    {0.28156, 0.28697, 0.33238},
	    {0.28779, 0.29339, 0.33877},
	    {0.29403, 0.2998, 0.34515},
	    {0.30026, 0.30621, 0.35154},
	    {0.30649, 0.31263, 0.35793},
	    {0.3126, 0.31904, 0.36388},
	    {0.3187, 0.32545, 0.36982},
	    {0.32481, 0.33186, 0.37577},
	    {0.33091, 0.33828, 0.38172},
	    {0.33701, 0.34469, 0.38767},
	    {0.34325, 0.3511, 0.39361},
	    {0.34948, 0.35752, 0.39956},
	    {0.35571, 0.36393, 0.40551},
	    {0.36195, 0.37034, 0.41145},
	    {0.36818, 0.37675, 0.4174},
	    {0.37429, 0.38317, 0.42313},
	    {0.38039, 0.38958, 0.42885},
	    {0.38649, 0.39599, 0.43458},
	    {0.3926, 0.4024, 0.44031},
	    {0.3987, 0.40882, 0.44604},
	    {0.40494, 0.41523, 0.45198},
	    {0.41117, 0.42164, 0.45793},
	    {0.4174, 0.42806, 0.46388},
	    {0.42364, 0.43447, 0.46982},
	    {0.42987, 0.44088, 0.47577},
	    {0.43623, 0.44689, 0.48128},
	    {0.4426, 0.45291, 0.48678},
	    {0.44896, 0.45892, 0.49229},
	    {0.45532, 0.46493, 0.4978},
	    {0.46169, 0.47094, 0.5033},
	    {0.46792, 0.47695, 0.50837},
	    {0.47416, 0.48297, 0.51344},
	    {0.48039, 0.48898, 0.5185},
	    {0.48662, 0.49499, 0.52357},
	    {0.49286, 0.501, 0.52863},
	    {0.49805, 0.50701, 0.53392},
	    {0.50325, 0.51303, 0.53921},
	    {0.50844, 0.51904, 0.54449},
	    {0.51364, 0.52505, 0.54978},
	    {0.51883, 0.53106, 0.55507},
	    {0.52442, 0.53667, 0.55969},
	    {0.53, 0.54228, 0.56432},
	    {0.53558, 0.5479, 0.56894},
	    {0.54117, 0.55351, 0.57357},
	    {0.54675, 0.55912, 0.57819},
	    {0.55182, 0.56433, 0.58304},
	    {0.55688, 0.56954, 0.58789},
	    {0.56195, 0.57475, 0.59273},
	    {0.56701, 0.57996, 0.59758},
	    {0.57208, 0.58517, 0.60242},
	    {0.57675, 0.58998, 0.60639},
	    {0.58143, 0.59479, 0.61035},
	    {0.5861, 0.5996, 0.61432},
	    {0.59078, 0.60441, 0.61828},
	    {0.59545, 0.60922, 0.62225},
	    {0.60065, 0.61403, 0.62709},
	    {0.60584, 0.61884, 0.63194},
	    {0.61104, 0.62365, 0.63678},
	    {0.61623, 0.62846, 0.64163},
	    {0.62143, 0.63327, 0.64648},
	    {0.62584, 0.63808, 0.65088},
	    {0.63026, 0.64289, 0.65529},
	    {0.63468, 0.6477, 0.65969},
	    {0.63909, 0.65251, 0.6641},
	    {0.64351, 0.65731, 0.6685},
	    {0.64857, 0.66132, 0.67269},
	    {0.65364, 0.66533, 0.67687},
	    {0.6587, 0.66934, 0.68106},
	    {0.66377, 0.67335, 0.68524},
	    {0.66883, 0.67735, 0.68943},
	    {0.67273, 0.68136, 0.69361},
	    {0.67662, 0.68537, 0.6978},
	    {0.68052, 0.68938, 0.70198},
	    {0.68442, 0.69339, 0.70617},
	    {0.68831, 0.69739, 0.71035},
	    {0.69221, 0.7022, 0.7141},
	    {0.6961, 0.70701, 0.71784},
	    {0.7, 0.71182, 0.72159},
	    {0.7039, 0.71663, 0.72533},
	    {0.70779, 0.72144, 0.72907},
	    {0.71169, 0.72505, 0.73348},
	    {0.71558, 0.72866, 0.73789},
	    {0.71948, 0.73226, 0.74229},
	    {0.72338, 0.73587, 0.7467},
	    {0.72727, 0.73948, 0.7511},
	    {0.73247, 0.74349, 0.75507},
	    {0.73766, 0.74749, 0.75903},
	    {0.74286, 0.7515, 0.763},
	    {0.74805, 0.75551, 0.76696},
	    {0.75325, 0.75952, 0.77093},
	    {0.75714, 0.76393, 0.77599},
	    {0.76104, 0.76834, 0.78106},
	    {0.76494, 0.77275, 0.78612},
	    {0.76883, 0.77715, 0.79119},
	    {0.77273, 0.78156, 0.79626},
	    {0.77792, 0.78677, 0.80132},
	    {0.78312, 0.79198, 0.80639},
	    {0.78831, 0.79719, 0.81145},
	    {0.79351, 0.8024, 0.81652},
	    {0.7987, 0.80762, 0.82159},
	    {0.80519, 0.81283, 0.82687},
	    {0.81169, 0.81804, 0.83216},
	    {0.81818, 0.82325, 0.83744},
	    {0.82468, 0.82846, 0.84273},
	    {0.83117, 0.83367, 0.84802},
	    {0.83636, 0.83888, 0.85286},
	    {0.84156, 0.84409, 0.85771},
	    {0.84675, 0.8493, 0.86256},
	    {0.85195, 0.85451, 0.8674},
	    {0.85714, 0.85972, 0.87225},
	    {0.86364, 0.86613, 0.87819},
	    {0.87013, 0.87255, 0.88414},
	    {0.87662, 0.87896, 0.89009},
	    {0.88312, 0.88537, 0.89604},
	    {0.88961, 0.89178, 0.90198},
	    {0.8961, 0.8986, 0.90947},
	    {0.9026, 0.90541, 0.91696},
	    {0.90909, 0.91222, 0.92445},
	    {0.91558, 0.91904, 0.93194},
	    {0.92208, 0.92585, 0.93943},
	    {0.92857, 0.93307, 0.94493},
	    {0.93506, 0.94028, 0.95044},
	    {0.94156, 0.94749, 0.95595},
	    {0.94805, 0.95471, 0.96145},
	    {0.95455, 0.96192, 0.96696},
	    {0.96364, 0.96954, 0.97357},
	    {0.97273, 0.97715, 0.98018},
	    {0.98182, 0.98477, 0.98678},
	    {0.99091, 0.99238, 0.99339},
	    {1, 1, 1 }};

    double lms0[] = { 34.918538957799996, 19.314796676499999, 0.585610818500000 };
    double m[]    = { 18.32535,  44.60077,   7.46216, 4.09544,  28.20135,   6.66066, 0.02114,   0.10325,   1.05258 };  
    double fac    = 1.0 / (lms0[0] + lms0[1]);
    double mm[] = { sqrt(3.0)*fac, sqrt(3.0)*fac, 0.0, sqrt(lms0[0]*lms0[0]+lms0[1]*lms0[1])/lms0[0]*fac, -sqrt(lms0[0]*lms0[0]+lms0[1]*lms0[1])/lms0[1]*fac, 0.0, -fac, -fac, (lms0[0] + lms0[1]) / lms0[2] * fac };

    for(int i = 0 ; i < imgL.rows ; ++i) {
		for(int j = 0 ; j < imgL.cols ; ++j) {
			double aa1 = lut_rgb[static_cast<int>(255*reinterpret_cast<double*>(imgR.data)[i*imgR.cols+j])][0];
			double aa2 = lut_rgb[static_cast<int>(255*reinterpret_cast<double*>(imgG.data)[i*imgR.cols+j])][1];
			double aa3 = lut_rgb[static_cast<int>(255*reinterpret_cast<double*>(imgB.data)[i*imgR.cols+j])][2];

			double lms1 = m[0] * aa1 + m[1] * aa2 + m[2] * aa3 - lms0[0];
			double lms2 = m[3] * aa1 + m[4] * aa2 + m[5] * aa3 - lms0[1];
			double lms3 = m[6] * aa1 + m[7] * aa2 + m[8] * aa3 - lms0[2];

			double dkl1 = mm[0] * lms1 + mm[1] * lms2 + mm[2] * lms3;
			double dkl2 = mm[3] * lms1 + mm[4] * lms2 + mm[5] * lms3;
			double dkl3 = mm[6] * lms1 + mm[7] * lms2 + mm[8] * lms3;

			reinterpret_cast<double*>(imgL.data)[i*imgR.cols+j] = dkl1 * 0.5774;
			reinterpret_cast<double*>(imgC1.data)[i*imgR.cols+j] = dkl2 * 2.7525;
			reinterpret_cast<double*>(imgC2.data)[i*imgR.cols+j] = dkl3 * 0.4526;
		}
	}

}





// ######################################################################
// kernel: 1 5 10 10 5 1
void GBVS::lowPass6yDecY(double* sptr, double* rptr, int w, int hs) const {
  int x, y;
  int hr = hs / 2;
  if (hr == 0) hr = 1;

  /* if (hs <= 1)
     result = src;
     else 
  */ 
  if (hs == 2)
    for (x = 0; x < w; ++x)
      {
        // use kernel [1 1]^T / 2
        *rptr++ = (sptr[0] + sptr[1]) / 2.0;
        sptr += 2;
      }
  else if (hs == 3)
    for (x = 0; x < w; ++x)
      {
        // use kernel [1 2 1]^T / 4
        *rptr++ = (sptr[0] + sptr[1] * 2.0 + sptr[2]) / 4.0;
        sptr += 3;
      }
  else // general case with hs >= 4
    for (x = 0; x < w; ++x)
      {
        // top most point - use kernel [10 10 5 1]^T / 26
        *rptr++ = ((sptr[0] + sptr[1]) * 10.0 + 
		   sptr[2] * 5.0 + sptr[3]) / 26.0;
        //++sptr;
        
        // general case
        for (y = 0; y < (hs - 5); y += 2)
          {
            // use kernel [1 5 10 10 5 1]^T / 32
            *rptr++ = ((sptr[1] + sptr[4])  *  5.0 +
                       (sptr[2] + sptr[3])  * 10.0 +
                       (sptr[0] + sptr[5])) / 32.0;
            sptr += 2;
          }
        
        // find out how to treat the bottom most point
        if (y == (hs - 5))
          {
            // use kernel [1 5 10 10 5]^T / 31
	    *rptr++ = ((sptr[1] + sptr[4])  *  5.0 +
		       (sptr[2] + sptr[3])  * 10.0 +
		       sptr[0])            / 31.0;
            sptr += 5;
          }
        else
          {
            // use kernel [1 5 10 10]^T / 26
            *rptr++ = ( sptr[0] + sptr[1]  *  5.0 +
			(sptr[2] + sptr[3]) * 10.0) / 26.0;
            sptr += 4;
          }
      }
}

// ######################################################################
// kernel: 1 5 10 10 5 1
void GBVS::lowPass6xDecX(double* sptr, double* rptr, int ws, int h) const {
  int x,y;
  const int h2 = h * 2, h3 = h * 3, h4 = h * 4, h5 = h * 5;
  int wr = ws / 2;
  if (wr == 0) wr = 1;

  /* if (ws <= 1)
     result = src;
     else */
  if (ws == 2)
    for (y = 0; y < h; ++y)
      {
        // use kernel [1 1] / 2
        *rptr++ = (sptr[0] + sptr[h]) / 2.0;
        ++sptr;
      }
  else if (ws == 3)
    for (y = 0; y < h; ++y)
      {
        // use kernel [1 2 1] / 4
        *rptr++ = (sptr[0] + sptr[h] * 2.0 + sptr[h2]) / 4.0;
        ++sptr;
      }
  else // general case for ws >= 4
    {
      // left most point - use kernel [10 10 5 1] / 26
      for (y = 0; y < h; ++y)
        {
          *rptr++ = ((sptr[0] + sptr[h]) * 10.0 + 
		     sptr[h2] * 5.0 + sptr[h3]) / 26.0;
          ++sptr;
        }
      sptr -= h;
      
      // general case
      for (x = 0; x < (ws - 5); x += 2)
        {
          for (y = 0; y < h; ++y)
            {
              // use kernel [1 5 10 10 5 1] / 32
              *rptr++ = ((sptr[h]  + sptr[h4])  *  5.0 +
                         (sptr[h2] + sptr[h3])  * 10.0 +
                         (sptr[0]  + sptr[h5])) / 32.0;
              ++sptr;
            }
          sptr += h;
        }
        
      // find out how to treat the right most point
      if (x == (ws - 5))
        for (y = 0; y < h; ++y)
          {
            // use kernel [1 5 10 10 5] / 31
            *rptr++ = ((sptr[h]  + sptr[h4])  *  5.0 +
                       (sptr[h2] + sptr[h3])  * 10.0 +
		       sptr[0]) / 31.0;
            ++sptr;
          }
      else
        for (y = 0; y < h; ++y)
          {
            // use kernel [1 5 10 10] / 26
            *rptr++ = ( sptr[0]  + sptr[h]   * 5.0 + 
			(sptr[h2] + sptr[h3]) * 10.0) / 26.0;
            ++sptr;
          }
    }
}








// ==============================================================================================================================
// Contrast sensitivity function. - Not part of original GBVS model -

#ifdef WITH_FFTW

cv::Mat GBVS::applyCSF(const cv::Mat& img1) {
	if(forward == NULL)
		forward = new fftwpp::fft2d(img1.cols, img1.rows, -1);

	if(backward == NULL)
		backward = new fftwpp::fft2d(img1.cols, img1.rows, 1);


	cv::Mat imgFloat;
	img1.convertTo(imgFloat, CV_32FC3);


	cv::Mat imgLab(img1.rows, img1.cols, CV_32FC3, img1.channels());
	cvtColor(imgFloat, imgLab, CV_BGR2Lab);


	cv::Mat csf = getCSF(imgLab, *forward, *backward);


	cv::Mat result(img1.rows, img1.cols, CV_64FC1, img1.channels());
	for(int i = 0 ; i < img1.rows ; ++i) {
		for(int j = 0 ; j < img1.cols ; ++j) {
			const cv::Point3_<float> &ac1c2 = imgLab.at< cv::Point3_<float> >(i, j);
			result.at<double>(i,j) = ac1c2.x * csf.at<float>(i,j);
		}
	}

	double mx, mn;
	cv::minMaxLoc(result, &mn, &mx);
	result = (result-mn) / (mx-mn);

	return result;
}



cv::Mat GBVS::getCSF(const cv::Mat& img1, fftwpp::fft2d &Forward, fftwpp::fft2d &Backward) const {

	/* get image properties */
	int width  	  = img1.cols;
	int height 	  = img1.rows;

	Complex *fft = fftwpp::ComplexAlign(width*height);
	
	// Complex *fftCR1, *fftCR2;
	// fftCR1 = fftwpp::ComplexAlign(width*height);
	// fftCR2 = fftwpp::ComplexAlign(width*height);

	for(int i = 0 ; i < height ; i++ ) {
		for(int j = 0 ; j < width ; ++j) {
			const cv::Point3_<float> &ac1c2 = img1.at< cv::Point3_<float> >(i, j);
			fft[i*width+j] = ac1c2.x;
			// fftCR1[i*width+j] = ac1c2.y;
			// fftCR2[i*width+j] = ac1c2.z;
		}
	}

	Forward.fft(fft);
	fftShift(fft, height, width);
	applyAchromaticCSF(fft, height, width);


	fftShift(fft, width, height);
	Backward.fftNormalized(fft);

	double mn, mx;
	cv::Mat globalCSF(img1.rows, img1.cols, CV_32FC1);
	for(int i = 0 ; i < width*height ; ++i)
		reinterpret_cast<float*>(globalCSF.data)[i] = static_cast<float>(real(fft[i])); 
		
	cv::minMaxLoc(globalCSF, &mn, &mx);
	globalCSF = (globalCSF - mn) / (mx - mn);


	fftwpp::deleteAlign(fft);


	return globalCSF;
}




void GBVS::applyAchromaticCSF(Complex * fft, int height, int width) const {

	float peakSensitivity		= 250.f;
	float excentricity			= 0.24f; // 0.24f in Scott Daly paper... (was 0...)
	float luminanceAdaptation	= 100.0f;
	float frequencyCstScaling	= 0.9f;
	float imageArea				= height*width/(nbPixelPerDegree*nbPixelPerDegree);

	float A = 0.801f*std::pow((1+0.7f)/luminanceAdaptation, -0.2f);
	float B = 0.3f*std::pow((1+100.f)/luminanceAdaptation, 0.15f);
	float ra = 0.856f*std::pow(viewingDistance, 0.14f);
	float re = 1/(1+0.24f*excentricity);
	


	float maxSensitivity = 0;
	for(int i = 0 ; i < height/2 ; ++i) {
		for(int j = 0 ; j < width/2 ; ++j) {
			float r2 = nbPixelPerDegree*nbPixelPerDegree*(static_cast<float>(i+1)*static_cast<float>(i+1)/(height*height)
														 +static_cast<float>(j+1)*static_cast<float>(j+1)/(width*width));
			float r = std::sqrt(r2);
			float theta = std::atan2(static_cast<float>(i+1)/height,static_cast<float>(j+1)/width);

			float Sensitivity1 = std::pow(std::pow(3.23f*std::pow(r2*imageArea, -0.3f), 5.f) + 1, -1/5.f) *
				A*frequencyCstScaling*r*exp(-B*frequencyCstScaling*r)*std::sqrt(1+0.06f*exp(B*frequencyCstScaling*r));
			
			float r2p = r / (ra*re*(0.11f*std::cos(4*theta)+0.89f));
			float Sensitivity2 = std::pow(std::pow(3.23f*std::pow(imageArea*r2p*r2p, -0.3f), 5.f) + 1, -1/5.f) *
				A*frequencyCstScaling*r2p*exp(-B*frequencyCstScaling*r2p)*sqrt(1+0.06f*exp(B*frequencyCstScaling*r2p));

			float csf = std::min(Sensitivity1, Sensitivity2);

			if(csf > maxSensitivity) maxSensitivity = csf;

			fft[(height/2+i)*width+(width/2)+j] *= csf;
			fft[(height/2-i)*width+(width/2)-j] *= csf;
			fft[(height/2-i)*width+(width/2)+j] *= csf;
			fft[(height/2+i)*width+(width/2)-j] *= csf;


		}
	}

	for(int i = 0 ; i < width*height ; ++i) {
		fft[i] *= peakSensitivity/maxSensitivity;
	}

}



void GBVS::applyChromaticCSF(Complex * fft, int height, int width, int color) const {

	float peakSensitivity		= 250.f;


	float a, b, c;
	switch(color) {
		case 1:
			peakSensitivity = 33.f;
			a = 5.52f;
			b = 1.72f;
			c = 0.27f;
			break;
		
		case 2:
			peakSensitivity = 5.f;
			a = 4.123f;
			b = 1.6454f;
			c = 0.24f;
			break;

		default:
			throw std::logic_error(std::string("GBVS::applyChromaticCSF: Invalid color component."));
			return;
	}
	

	for(int i = 0 ; i < height/2 ; ++i) {
		for(int j = 0 ; j < width/2 ; ++j) {
			float r2 = nbPixelPerDegree*nbPixelPerDegree*(static_cast<float>(i+1)*static_cast<float>(i+1)/(height*height)
														 +static_cast<float>(j+1)*static_cast<float>(j+1)/(width*width));
			float r = std::sqrt(r2);
			float theta = std::atan2(static_cast<float>(i+1)/height,static_cast<float>(j+1)/width);

			float csf = peakSensitivity / (1+std::pow(r/a, b))*(1-c*sin(2*theta));

			fft[(height/2+i)*width+(width/2)+j] *= csf;
			fft[(height/2-i)*width+(width/2)-j] *= csf;
			fft[(height/2-i)*width+(width/2)+j] *= csf;
			fft[(height/2+i)*width+(width/2)-j] *= csf;

		}
	}

}

void GBVS::fftShift(Complex * fft, int height, int width) const {
	for(int i = 0 ; i < height/2 ; ++i) {
		for(int j = 0 ; j < width/2 ; ++j) {
			std::swap(fft[i*width+j], fft[width*height/2+width/2+i*width+j]);
			std::swap(fft[i*width+width/2+j], fft[width*height/2+i*width+j]);
		}
	}
}

#else

cv::Mat GBVS::applyCSF(const cv::Mat& img1) {

	std::cerr << "[I] Support for FFT was not compiled. The CSF only return the Luminance component from CIELab \n";
	
	cv::Mat imgFloat;
	img1.convertTo(imgFloat, CV_32FC3);


	cv::Mat imgLab(img1.rows, img1.cols, CV_32FC3, img1.channels());
	cvtColor(imgFloat, imgLab, CV_BGR2Lab);


	cv::Mat result(img1.rows, img1.cols, CV_64FC1, img1.channels());
	for(int i = 0 ; i < img1.rows ; ++i) {
		for(int j = 0 ; j < img1.cols ; ++j) {
			const cv::Point3_<float> &ac1c2 = imgLab.at< cv::Point3_<float> >(i, j);
			result.at<double>(i,j) = ac1c2.x;
		}
	}

	double mx, mn;
	cv::minMaxLoc(result, &mn, &mx);
	result = (result-mn) / (mx-mn);

	return result;
}



#endif




