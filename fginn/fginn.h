#ifndef FGINN_H
#define FGINN_H

#include <opencv2/core.hpp>

/*
 C++ implementation of Symmetric FGINN intersection
https://github.com/ducha-aiki/matching-strategies-comparison
*/

namespace fginn
{
    cv::Mat calcDistanceMatrix(const cv::Mat &anchor,
                               const cv::Mat &positive);

    std::vector<cv::DMatch> matchSymFginnIntersection(const cv::Mat &descr1,
                                                      const cv::Mat &descr2,
                                                      const std::vector<cv::KeyPoint> &kps1,
                                                      const std::vector<cv::KeyPoint> &kps2);

    std::vector<cv::Point> getFginnIndexes(cv::Mat dm,
                                           const cv::Mat &km);

    std::vector<cv::Point> findPairIndexes(const std::vector<cv::Point> &m1,
                                           const std::vector<cv::Point> &m2);
    cv::Mat keyPointsToMat(const std::vector<cv::KeyPoint> &kps);
}

#endif // FGINN_H