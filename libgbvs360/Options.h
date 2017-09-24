#ifndef _Option_
#define _Option_

#include <string>


struct Option {
	static std::string inputPath;
	static std::string outputPath;
	static size_t threads;

	static std::string scanPath;
	static int scanPathUserIdx;
	
	static int numberFixations;
	static int experimentLength;
	static int experimentRepppetition;

	static float distScaling;


	// export raw features for training the pooling using R
	static bool exportRawFeatures;
	static std::string prefix;

};


#endif

