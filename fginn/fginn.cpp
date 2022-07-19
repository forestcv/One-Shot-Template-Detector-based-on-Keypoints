#include "fginn.h"

#include <iostream>
#include <cmath>

/*
 C++ implementation of Symmetric FGINN intersection
https://github.com/ducha-aiki/matching-strategies-comparison
*/

namespace fginn
{

        std::tuple<std::vector<float>, std::vector<int>> findMinByRows(const cv::Mat &dm);
        cv::Mat maskBelowThreshold(const cv::Mat &mat, float threshold);
}

/* Compare return value with original
def distance_matrix(anchor, positive):
    d1_sq = torch.sum(anchor * anchor, dim=1).unsqueeze(-1)
    d2_sq = torch.sum(positive * positive, dim=1).unsqueeze(-1)
    eps = 1e-6
    return torch.sqrt((temp1 + temp2))
                      - 2.0 * apProduct)+eps)
*/
// https://github.com/DagnyT/hardnet/blob/master/code/Losses.py#L5
cv::Mat fginn::calcDistanceMatrix(const cv::Mat &anchor,
                                  const cv::Mat &positive)
{
        cv::Mat dsqFirst = anchor.mul(anchor);
        cv::reduce(dsqFirst, dsqFirst, 1, cv::REDUCE_SUM, CV_32F);

        cv::Mat dsqSecond = positive.mul(positive);

        cv::reduce(dsqSecond, dsqSecond, 1, cv::REDUCE_SUM, CV_32F);

        cv::Mat temp1{dsqFirst.rows, positive.rows, CV_32F, cv::Scalar(0)};
        for (int i = 0; i < positive.rows; i++)
        {
                dsqFirst.copyTo(temp1(cv::Rect(i, 0, dsqFirst.cols, dsqFirst.rows)));
        }

        cv::Mat temp2{dsqSecond.rows, anchor.rows, CV_32F, cv::Scalar(0)};
        for (int i = 0; i < anchor.rows; i++)
        {
                dsqSecond.copyTo(temp2(cv::Rect(i, 0, dsqSecond.cols, dsqSecond.rows)));
        }
        cv::transpose(temp2, temp2);

        cv::Mat positive_t;
        cv::transpose(positive, positive_t);
        cv::Mat apProduct = anchor * positive_t;

        const float eps = 1e-6;

        cv::Mat temp3 = (temp1 + temp2 - 2 * apProduct) + eps;

        cv::sqrt(temp3, temp3);

        return temp3;
}

/*
convert keypoints list to matrix with size (num_of_keypoints x 2):
        0        1
0    kp[0].x  kp[0].y
1    kp[1].x  kp[1].y
2    kp[2].x  kp[2].y
*/
cv::Mat fginn::keyPointsToMat(const std::vector<cv::KeyPoint> &kps)
{
        cv::Mat res{static_cast<int>(kps.size()), 2, CV_32F, cv::Scalar(0.0f)};
        float *ptr = (float *)res.data;
        for (auto kp : kps)
        {
                *ptr = kp.pt.x;
                ptr++;
                *ptr = kp.pt.y;
                ptr++;
        }
        return res;
}


std::vector<cv::Point> fginn::getFginnIndexes(cv::Mat dm,
                                              const cv::Mat &km)
{
        auto [vals, idxs_in_2] = findMinByRows(dm);

        cv::Mat mask1 = maskBelowThreshold(km, 10.0f);

        cv::Mat mask2{idxs_in_2.size(), mask1.cols, CV_8U, cv::Scalar(0)};
        for (int i = 0; i < idxs_in_2.size(); i++)
        {
                mask1(cv::Rect(0, idxs_in_2.at(i), mask1.cols, 1)).copyTo(mask2(cv::Rect(0, i, mask2.cols, 1)));
        }

        uint8_t *mask2Ptr = mask2.data;
        for (float *ptr = (float *)dm.data;
             ptr < (float *)dm.data + dm.cols * dm.rows;
             ptr++,
                   mask2Ptr++)
        {
                if (*mask2Ptr == 1)
                        *ptr = 100000;
        }

        auto [vals_2nd, inxs_in_2_2nd] = findMinByRows(dm);

        std::vector<float> ratio(vals.size());
        for (int i = 0; i < vals.size(); i++)
        {
                ratio.at(i) = vals.at(i) / vals_2nd.at(i);
        }

        std::vector<bool> mask;
        mask.reserve(ratio.size());
        for (auto r : ratio)
        {
                if (r <= 0.8f)
                        mask.push_back(true);
                else
                        mask.push_back(false);
        }

        std::vector<int> idxs_in_1;
        idxs_in_1.reserve(idxs_in_2.size());
        for (int i = 0; i < idxs_in_2.size(); i++)
        {
                if (mask.at(i) == true)
                        idxs_in_1.push_back(i);
        }
        idxs_in_1.shrink_to_fit();

        std::vector<int> temp_idxs_in_2;
        temp_idxs_in_2.reserve(idxs_in_2.size());
        for (int i = 0; i < idxs_in_2.size(); i++)
        {
                if (mask.at(i) == true)
                        temp_idxs_in_2.push_back(idxs_in_2.at(i));
        }
        temp_idxs_in_2.shrink_to_fit();
        idxs_in_2 = temp_idxs_in_2;

        std::vector<cv::Point> res(idxs_in_1.size());
        for (int i = 0; i < idxs_in_1.size(); i++)
        {
                res.at(i) = cv::Point{idxs_in_1.at(i), idxs_in_2.at(i)};
        }

        return res;
}

std::vector<cv::DMatch> fginn::matchSymFginnIntersection(const cv::Mat &descr1,
                                                         const cv::Mat &descr2,
                                                         const std::vector<cv::KeyPoint> &kps1,
                                                         const std::vector<cv::KeyPoint> &kps2)
{
        cv::Mat xy1 = keyPointsToMat(kps1);
        cv::Mat xy2 = keyPointsToMat(kps2);

        cv::Mat dm = calcDistanceMatrix(descr1, descr2);
        cv::Mat km1 = calcDistanceMatrix(xy2, xy2);
        cv::Mat km2 = calcDistanceMatrix(xy1, xy1);

        std::vector<cv::Point> m1 = getFginnIndexes(dm, km1);
        cv::Mat dm_t;
        cv::transpose(dm, dm_t);
        std::vector<cv::Point> m2 = getFginnIndexes(dm_t, km2);

        std::vector<cv::Point> out_m = findPairIndexes(m1, m2);

        std::vector<cv::DMatch> goodMatches;
        for (auto &m : out_m)
                goodMatches.emplace_back(cv::DMatch{m.x, m.y, 1});

        return goodMatches;
}

std::tuple<std::vector<float>, std::vector<int>> fginn::findMinByRows(const cv::Mat &dm)
{
        std::vector<float> mins;
        mins.reserve(dm.rows);
        std::vector<int> inxs;
        inxs.reserve(dm.rows);

        double minVal = 0;
        double maxVal = 0;
        cv::Point minLoc{0, 0};
        cv::Point maxLoc{0, 0};

        for (int i = 0; i < dm.rows; i++)
        {
                cv::minMaxLoc(dm(cv::Rect(0, i, dm.cols, 1)), &minVal, &maxVal, &minLoc, &maxLoc);
                mins.emplace_back(minVal);
                inxs.emplace_back(minLoc.x);
        }

        return std::tuple<std::vector<float>, std::vector<int>>{mins, inxs};
}

cv::Mat fginn::maskBelowThreshold(const cv::Mat &mat, float threshold)
{
        cv::Mat mask{mat.rows, mat.cols, CV_8U, cv::Scalar(0)};
        uint8_t *maskPtr = mask.data;
        int cnt = 0;
        for (float *ptr = (float *)mat.data;
             ptr < (float *)mat.data + mat.cols * mat.rows;
             ptr++,
                   maskPtr++,
                   cnt++)
        {
                if (*ptr <= threshold || std::isnan(*ptr))
                        *maskPtr = 1;
        }
        return mask;
}

std::vector<cv::Point> fginn::findPairIndexes(const std::vector<cv::Point> &m1,
                                              const std::vector<cv::Point> &m2)
{
        std::vector<cv::Point> out_m;
        std::vector<cv::Point> row;
        row.reserve(m2.size());
        for (int i = 0; i < m1.size(); i++)
        {
                row.clear();
                for (auto &m : m2)
                {
                        if (m.y == m1.at(i).x)
                                row.push_back(m);
                }
                // if (!row.empty())
                // {
                //         if ((m1.at(i).x == row.at(0).y) && (m1.at(i).y == row.at(0).x))
                //                 out_m.emplace_back(m1.at(i));
                // }
                // for (auto &r : row)
                // {
                //         if ((m1.at(i).x == r.y) && abs(m1.at(i).y - r.x) < 1000)
                //                 out_m.emplace_back(m1.at(i));
                // }
                for (auto &r : row)
                {
                        if (abs(m1.at(i).x - r.y) < 500 && abs(m1.at(i).y - r.x) < 500)
                                out_m.emplace_back(m1.at(i));
                }
        }

        return out_m;
}