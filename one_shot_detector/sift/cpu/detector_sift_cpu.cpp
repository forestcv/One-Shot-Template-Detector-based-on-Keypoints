#include "detector_sift_cpu.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>

#include "../../../fginn/fginn.h"

using namespace one_shot_detector::sift::cpu;

Detector::Detector()
{
    referenceData = std::make_unique<ReferenceData>();
    SiftSettings frameSettings;
    frameSIFT = cv::SIFT::create(frameSettings.nFeatures,
                                 frameSettings.nOctaveLayers,
                                 frameSettings.contrastThreshold,
                                 frameSettings.edgeThreshold,
                                 frameSettings.sigma);
}

Detector::Detector(std::shared_ptr<DetectorSettigns> settings) : settings(settings)
{
    referenceData = std::make_unique<ReferenceData>();
    frameSIFT = cv::SIFT::create(settings->frameSettings.nFeatures,
                                 settings->frameSettings.nOctaveLayers,
                                 settings->frameSettings.contrastThreshold,
                                 settings->frameSettings.edgeThreshold,
                                 settings->frameSettings.sigma);
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
    frameKeyPts.reserve(2500);
    cv::Mat frameDescriptor;

    getKeyPtsAndDescr(processedImage,
                      frameSIFT, frameSIFT,
                      frameKeyPts, frameDescriptor);

    convertDescriptors(frameDescriptor);

    std::vector<cv::DMatch> goodMatches;
    if (!frameKeyPts.empty() && !referenceData->keyPts.empty())
    {
        matchKeyPoints(referenceData->descriptor, frameDescriptor,
                       referenceData->keyPts, frameKeyPts, goodMatches);
    }

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

    if (goodMatches.size() >= settings->goodMatchesThreshold)
    {
        std::vector<cv::Point2f> corners{};
        findAlignment(referenceData->keyPts,
                      frameKeyPts,
                      goodMatches,
                      referenceData->image.size(),
                      corners);
        drawImageAlignment(img_matches, referenceData->image.size(),
                           corners);
    }

    cv::resize(img_matches, img_matches, cv::Size(0, 0), 0.5, 0.5);

    // static int cnt = 0;
    // cv::imwrite("../example/" + std::to_string(cnt) + ".jpg", img_matches);
    // cnt++;

    cv::imshow("Result", img_matches);
    cv::waitKey(1);
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
    cv::Ptr<cv::SIFT> referenceSIFT = cv::SIFT::create(settings->referenceSettings.nFeatures,
                                                       settings->referenceSettings.nOctaveLayers,
                                                       settings->referenceSettings.contrastThreshold,
                                                       settings->referenceSettings.edgeThreshold,
                                                       settings->referenceSettings.sigma);

    if (reference.channels() == 1)
        referenceData->image = reference.clone();
    else
        cv::cvtColor(reference, referenceData->image, cv::COLOR_BGR2BGRA);

    getKeyPtsAndDescr(referenceData->image,
                      referenceSIFT, referenceSIFT,
                      referenceData->keyPts, referenceData->descriptor);

    convertDescriptors(referenceData->descriptor);
}

void Detector::convertDescriptors(cv::Mat &descriptors)
{
    if (descriptors.empty() or descriptors.rows < 2)
    {
        descriptors = cv::Mat(2, 128, CV_32F, cv::Scalar(0));
    }
    // else
    //     descriptors.convertTo(descriptors, CV_32F);
}