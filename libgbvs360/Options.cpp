#include "Options.h"

std::string Option::inputPath;
std::string Option::outputPath;
size_t		Option::threads = 1;


std::string Option::scanPath = "";
int Option::scanPathUserIdx = 0;

int Option::numberFixations  = 40;
int Option::experimentLength = 25;
int Option::experimentRepppetition = 40;
float Option::distScaling = 1.f;

// export raw features for training the pooling using R
bool		Option::exportRawFeatures = false;
std::string Option::prefix = "";


