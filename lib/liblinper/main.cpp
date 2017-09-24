#include <iostream>

#include <boost/program_options.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "src/VPD.h"

void getFeature(const cv::Mat& img, cv::Mat &output, double *reliability) {
    output = vanishingLineFeatureMap(img, reliability);
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
            ("input-file,i", po::value< std::string >(), "Input image to analyze")
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
        std::cout << "\t\rFinding vanishing point in an image using vdpdetection\n";
        std::cout << "--------------------------------------------------------------------------------\n";
        std::cout << "\n\n";
        std::cout << desc << "\n";
        return 1;
    }



    // ------------------------------------------------------------------------------------------
    // Parsing input

    std::string inputPath;

     if (vm.count("input-file")) {
        inputPath = vm["input-file"].as< std::string >();
    } else {
        std::cerr << "You have to provide an image to process. See --help" << std::endl;
        return 0;
    }


    cv::Mat img = cv::imread(inputPath);
    if(img.empty()) {
    	std::cerr << "Cannot open: " << inputPath << std::endl;
    	return 0;
    }

    cv::resize(img, img, cv::Size(1200, static_cast<int>((1000.f/img.cols) * img.rows)));
    
    std::vector<double> reliability(4);
    std::vector<cv::Mat> outputs(4);
    boost::thread_group g;
    for(size_t i = 0 ; i < outputs.size() ; ++i) {
        g.create_thread(boost::bind(getFeature, boost::ref(img), boost::ref(outputs[i]), &reliability[i]));
    }
    g.join_all();

    //getFeature(img, outputs[0]);

    for(size_t i = 1 ; i < outputs.size() ; ++i) {
        outputs[0] += outputs[i];
        reliability[0] += reliability[i];
    }
    outputs[0] /= outputs.size();
    reliability[0] /= outputs.size();

    std::cout << inputPath << ", " << reliability[0] << std::endl;

    cv::imshow("colorImg", img);
    cv::imshow("featureMap", outputs[0]);
    cv::moveWindow("featureMap", 10, 10);
    cv::moveWindow("colorImg", 10, 20 + outputs[0].rows);

    cv::waitKey();


	return 0;
}
