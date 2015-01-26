/******************************************************************************
 *  Copyright (c) 2014. All rights reserved.
 *
 *  Project: Ray Tracing
 *  Filename: main.cc
 *  Version: 1.0
 *  Author: Jinming Hu
 *  E-mail: hjm211324@gmail.com
 *  Date: Apr. 07, 2014
 *  Time: 14:53:03
 *  Description: set parameters, get color of each pixel, show on screen
 *****************************************************************************/
#ifdef _OPENMP
#include <omp.h>
#endif
#include <opencv2/opencv.hpp>
#include "common.h"
#include "scene.h"
#include "camera.h"
#include "parser.h"

void antiAliasing(cv::Mat_<cv::Vec3b>&, const COUNTTYPE);

int main(int argc, char** argv) {
    using namespace std;
    using namespace cv;
    using namespace RayTracing;
    using RayTracing::Point;
    
    assert(argc == 4);
    Scene scene;
    ObjParser objParser(argv[1], scene);
    CmrParser cmrParser(argv[2]);
    Camera* camera = cmrParser.getCamera();
    COUNTTYPE AARatio = cmrParser.aaRatio();

    // image buffer for parallel computing
    vector< vector< Color > > colorMat(camera -> resolutionWidth(), 
                                       vector< Color >(camera -> resolutionLength(), 
                                                       Color(0, 0, 0, 0)));
#ifdef _OPENMP
#   pragma omp parallel for num_threads(8) schedule(dynamic)
#endif
    // ray trace
    for (COUNTTYPE j = 0; j < camera -> resolutionWidth(); ++j) {
        for (COUNTTYPE i = 0; i < camera -> resolutionLength(); ++i) {
            vector<Ray> rays = camera -> getRays(i, j);
            Color mixColor(0, 0, 0, 0);
            for (auto ray: rays) {
                Color color(0, 0, 0, 0);
                scene.rayTrace(ray, color);
                mixColor += color;
            }
            colorMat[j][i] = mixColor;
        }
    }

    //copy to oepncv image
    Mat_<Vec3b> image(camera -> resolutionWidth(), camera -> resolutionLength());
    auto ite = image.begin();
    for (auto w: colorMat)
        for (auto l: w) {
            (*ite)[0] = l[2], (*ite)[1] = l[1], (*ite)[2] = l[0];
            ++ite;
        }

    // anti aliasing
    antiAliasing(image, AARatio);
    
    imwrite(argv[3], image);
    //namedWindow("Preview");
    //imshow("Preview", image);
    //waitKey(0);
    return 0;
}

void antiAliasing(cv::Mat_<cv::Vec3b>& image, const COUNTTYPE aaRatio) {
    if (aaRatio != 1)
        cv::resize(image, image, 
                   cv::Size(image.cols / aaRatio, image.rows / aaRatio),
                   0, 0, cv::INTER_AREA);
}

