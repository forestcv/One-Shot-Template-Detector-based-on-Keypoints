#include "detector.h"
#include "../fginn/fginn.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

// https://docs.opencv.org/3.4/d7/dff/tutorial_feature_homography.html
void one_shot_detector::drawImageAlignment(cv::Mat &image,
                                           const cv::Size &referenceSize,
                                           const std::vector<cv::Point2f> &corners)
{
    cv::Point2f referenceOffset{static_cast<float>(referenceSize.width), 0};
    // each line must has offset from left as input image is combination of
    // reference and current frame
    cv::line(image, corners.at(0) + referenceOffset,
             corners.at(1) + referenceOffset, cv::Scalar(0, 255, 0), 4);
    cv::line(image, corners.at(0) + referenceOffset,
             corners.at(3) + referenceOffset, cv::Scalar(0, 255, 0), 4);
    cv::line(image, corners.at(1) + referenceOffset,
             corners.at(2) + referenceOffset, cv::Scalar(0, 255, 0), 4);
    cv::line(image, corners.at(2) + referenceOffset,
             corners.at(3) + referenceOffset, cv::Scalar(0, 255, 0), 4);
}

// https://docs.opencv.org/3.4/d7/dff/tutorial_feature_homography.html
void one_shot_detector::findAlignment(const std::vector<cv::KeyPoint> &referenceKeyPts,
                                      const std::vector<cv::KeyPoint> &frameKeyPts,
                                      const std::vector<cv::DMatch> &goodMatches,
                                      const cv::Size &referenceSize,
                                      std::vector<cv::Point2f> &corners)
{
    std::vector<cv::Point2f> frameMatchedKeyPts;
    frameMatchedKeyPts.reserve(goodMatches.size());
    std::vector<cv::Point2f> referenceMatchedKeyPts;
    referenceMatchedKeyPts.reserve(goodMatches.size());
    // write matched key points in correspondence vectors
    for (auto &match : goodMatches)
    {
        frameMatchedKeyPts.emplace_back(frameKeyPts.at(match.trainIdx).pt);
        referenceMatchedKeyPts.emplace_back(referenceKeyPts.at(match.queryIdx).pt);
    }
    cv::Mat homography = cv::findHomography(referenceMatchedKeyPts,
                                            frameMatchedKeyPts, cv::RANSAC);
    // check if homography was found successfully
    if (homography.cols != 0)
    {
        std::vector<cv::Point2f> referenceCorners{4};
        referenceCorners.at(0) = cv::Point2f{0, 0};
        referenceCorners.at(1) = cv::Point2f{referenceSize.width, 0};
        referenceCorners.at(2) = cv::Point2f{referenceSize.width,
                                             referenceSize.height};
        referenceCorners.at(3) = cv::Point2f{0, referenceSize.height};

        cv::perspectiveTransform(referenceCorners, corners, homography);
    }
    else // fill projection with zero points
    {
        for (int i = 0; i < 4; i++)
            corners.emplace_back(cv::Point2f(0, 0));
    }
}

// https://docs.opencv.org/3.4/d5/d6f/tutorial_feature_flann_matcher.html
void one_shot_detector::matchKeyPoints(const cv::Mat &referenceDescr,
                                       const cv::Mat &frameDescr,
                                       const std::vector<cv::KeyPoint> &referenceKeyPts,
                                       const std::vector<cv::KeyPoint> &frameKeyPts,
                                       std::vector<cv::DMatch> &goodMatches)
{
    goodMatches = fginn::matchSymFginnIntersection(referenceDescr,
                                                   frameDescr,
                                                   referenceKeyPts,
                                                   frameKeyPts);
}

void one_shot_detector::matchKeyPoints(const cv::Mat &referenceDescriptor,
                                       const cv::Mat &frameDescriptor,
                                       double threshold,
                                       std::vector<cv::DMatch> &goodMatches)
{
    // cv::Ptr<cv::DescriptorMatcher> matcher =
    //     cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);

    cv::Ptr<cv::DescriptorMatcher> matcher = cv::BFMatcher::create();
    std::vector<std::vector<cv::DMatch>> knn_matches;
    matcher->knnMatch(referenceDescriptor, frameDescriptor, knn_matches, 2);
    goodMatches.clear();
    for (size_t i = 0; i < knn_matches.size(); i++)
    {
        if (knn_matches.at(i).at(0).distance < threshold * knn_matches.at(i).at(1).distance)
        {
            goodMatches.emplace_back(knn_matches.at(i).at(0));
        }
    }
}