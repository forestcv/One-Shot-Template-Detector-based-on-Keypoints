#ifndef SIFT_DETECTOR_H
#define SIFT_DETECTOR_H

#include <opencv2/core.hpp>

namespace one_shot_detector::sift
{
    struct SiftSettings
    {
        int nFeatures = 10000;
        int nOctaveLayers = 3;
        double contrastThreshold = 0.04;
        double edgeThreshold = 10;
        double 	sigma = 1.6;
    };

    struct DetectorSettigns
    {
        SiftSettings referenceSettings;
        SiftSettings frameSettings;
        int goodMatchesThreshold = 8;
    };
}

#endif // SIFT_DETECTOR_H