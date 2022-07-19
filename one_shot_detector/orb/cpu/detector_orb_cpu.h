#ifndef DETECTOR_ORB_CPU_H
#define DETECTOR_ORB_CPU_H

#include <memory>

#include <opencv2/xfeatures2d.hpp>

#include "../../detector.h"
#include "../orb_detector.h"

namespace one_shot_detector::orb::cpu
{
    class Detector : public OneShotDetector
    {
    public:
        Detector();
        Detector(std::shared_ptr<DetectorSettigns> settings);
        bool init(const cv::Mat &reference) override;
        bool detect(const cv::Mat &frame, cv::Rect &target) override;

    private:
        void prepareReferenceData(const cv::Mat &reference);
        void prepareImage(const cv::Mat &frame, cv::Mat &processedImage);
        void convertDescriptors(cv::Mat &descriptors) override;

        std::unique_ptr<ReferenceData> referenceData = nullptr;
        OrbSettings referenceSettings;
        cv::Ptr<cv::ORB> frameORB;
        const float matcherThreshold = 0.7;
        std::shared_ptr<DetectorSettigns> settings;
    };
}

#endif