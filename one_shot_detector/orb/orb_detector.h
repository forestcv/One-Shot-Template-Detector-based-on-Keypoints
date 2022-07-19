#ifndef ORB_DETECTOR_H
#define ORB_DETECTOR_H

#include <opencv2/core.hpp>

namespace one_shot_detector::orb
{
    struct OrbSettings
    {
        int nFeatures = 10000;
        float scaleFactor = 1.2f;
        int nLevels = 4;
        int edgeThreshold = 30;
        int firstLevel = 2;
        int wtaK = 4;
        int fastScore = 1;
        int patchSize = 30;
        int fastThreshold = 30;
    };

    struct DetectorSettigns
    {
        OrbSettings referenceSettings;
        OrbSettings frameSettings;
        int matcherThreshold = 8;
        int goodMatchesThreshold = 8;
    };
}

#endif