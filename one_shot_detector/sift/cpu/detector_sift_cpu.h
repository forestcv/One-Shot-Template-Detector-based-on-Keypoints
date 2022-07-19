#ifndef DETECTOR_SIFT_CPU_H
#define DETECTOR_SIFT_CPU_H

#include <memory>

#include <opencv2/xfeatures2d.hpp>

#include "../../detector.h"
#include "../sift_detector.h"

namespace one_shot_detector::sift::cpu
{
    class Detector : public OneShotDetector
    {
    public:
        Detector();
        Detector(std::shared_ptr<DetectorSettigns> settings);
        bool init(const cv::Mat &reference) override;
        bool detect(const cv::Mat &frame, cv::Rect &target) override;

    private:
        void prepareReferenceData(const cv::Mat &reference) override;
        void prepareImage(const cv::Mat &frame, cv::Mat &processedImage) override;
        void convertDescriptors(cv::Mat &descriptors) override;

        std::unique_ptr<ReferenceData> referenceData = nullptr;
        cv::Ptr<cv::SIFT> frameSIFT;
        std::shared_ptr<DetectorSettigns> settings;
    };
}

#endif