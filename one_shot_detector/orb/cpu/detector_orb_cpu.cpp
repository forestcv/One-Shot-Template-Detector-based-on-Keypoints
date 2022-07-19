#include "detector_orb_cpu.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>

#include "../../../fginn/fginn.h"

using namespace one_shot_detector::orb::cpu;

Detector::Detector()
{
    referenceData = std::make_unique<ReferenceData>();
    OrbSettings frameOrbSettings{3000, 1.2f, 8, 30, 0, 4, 1, 30, 12};
    frameORB = cv::ORB::create(frameOrbSettings.nFeatures,
                               frameOrbSettings.scaleFactor,
                               frameOrbSettings.nLevels,
                               frameOrbSettings.edgeThreshold,
                               frameOrbSettings.firstLevel,
                               frameOrbSettings.wtaK,
                               cv::ORB::HARRIS_SCORE,
                               frameOrbSettings.patchSize,
                               frameOrbSettings.fastThreshold);
}

Detector::Detector(std::shared_ptr<DetectorSettigns> settings) : settings(settings)
{
    referenceData = std::make_unique<ReferenceData>();
    frameORB = cv::ORB::create(settings->frameSettings.nFeatures,
                               settings->frameSettings.scaleFactor,
                               settings->frameSettings.nLevels,
                               settings->frameSettings.edgeThreshold,
                               settings->frameSettings.firstLevel,
                               settings->frameSettings.wtaK,
                               cv::ORB::HARRIS_SCORE,
                               settings->frameSettings.patchSize,
                               settings->frameSettings.fastThreshold);
}

bool Detector::init(const cv::Mat &reference)
{
    prepareReferenceData(reference);
}

bool Detector::detect(const cv::Mat &frame, cv::Rect &target)
{
    cv::Mat processedImage;
    prepareImage(frame, processedImage);

    std::vector<cv::KeyPoint> frameKeyPts;
    frameKeyPts.reserve(5000);
    cv::Mat frameDescriptor;

    getKeyPtsAndDescr(processedImage,
                      frameORB, frameORB,
                      frameKeyPts, frameDescriptor);

    convertDescriptors(frameDescriptor);

    std::vector<cv::DMatch> goodMatches;
    matchKeyPoints(referenceData->descriptor,
                   frameDescriptor,
                   0.5,
                   goodMatches);
    // matchKeyPoints(referenceData->descriptor, frameDescriptor,
    //                referenceData->keyPts, frameKeyPts, goodMatches);

    cv::Mat img_matches;
    drawMatches(referenceData->image,
                referenceData->keyPts,
                processedImage,
                frameKeyPts,
                goodMatches,
                img_matches,
                cv::Scalar::all(-1),
                cv::Scalar::all(-1),
                std::vector<char>(),
                cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    bool result = false;
    if (goodMatches.size() >= 8)
    {
        std::vector<cv::Point2f> corners{};
        findAlignment(referenceData->keyPts,
                      frameKeyPts,
                      goodMatches,
                      referenceData->image.size(),
                      corners);
        drawImageAlignment(img_matches, referenceData->image.size(),
                           corners);
        result = true;
    }

    cv::resize(img_matches, img_matches, cv::Size(0, 0), 0.5, 0.5);
    cv::imshow("Result", img_matches);
    cv::waitKey(1);

    return result;
}

void Detector::prepareImage(const cv::Mat &frame, cv::Mat &processedImage)
{
    if (frame.channels() == 3)
        cv::cvtColor(frame, processedImage, cv::COLOR_BGR2GRAY);
    else
        processedImage = frame;
}

void Detector::prepareReferenceData(const cv::Mat &reference)
{
    cv::Ptr<cv::ORB> referenceORB = cv::ORB::create(referenceSettings.nFeatures,
                                                    referenceSettings.scaleFactor,
                                                    referenceSettings.nLevels,
                                                    referenceSettings.edgeThreshold,
                                                    referenceSettings.firstLevel,
                                                    referenceSettings.wtaK,
                                                    cv::ORB::HARRIS_SCORE,
                                                    referenceSettings.patchSize,
                                                    referenceSettings.fastThreshold);

    if (reference.channels() == 1)
        referenceData->image = reference.clone();
    else
        cv::cvtColor(reference, referenceData->image, cv::COLOR_BGR2BGRA);

    getKeyPtsAndDescr(referenceData->image,
                      referenceORB, referenceORB,
                      referenceData->keyPts, referenceData->descriptor);

    convertDescriptors(referenceData->descriptor);
}

void Detector::convertDescriptors(cv::Mat &descriptors)
{
    if (descriptors.empty() or descriptors.rows < 2)
    {
        descriptors = cv::Mat(32, 2, CV_32F, cv::Scalar(0));
    }
    else
        descriptors.convertTo(descriptors, CV_32F);
}