#include <gtest/gtest.h>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>

#include <opencv2/core.hpp>

#include "../fginn/fginn.h"

std::vector<std::string> split(const std::string &str, char d)
{
    std::vector<std::string> r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while (stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

cv::Mat getDataFromFile(const std::string &filename,
                        int cols, int rows)
{
    std::string line;
    std::ifstream myfile(filename);
    cv::Mat data{rows, cols, CV_32F, cv::Scalar(0.0f)};
    float *dmPtr = (float *)data.data;
    if (myfile.is_open())
    {
        while (std::getline(myfile, line))
        {
            std::vector<std::string> linedata = split(line, ' ');
            for (auto &d : linedata)
            {
                *dmPtr = stod(d);

                dmPtr++;
            }
        }
        myfile.close();
    }
    return data;
}

std::vector<cv::KeyPoint> getKeyPtsFromFile(const std::string &filename)
{
    std::string line;
    std::ifstream myfile(filename);
    std::vector<cv::KeyPoint> kps;
    kps.reserve(10000);
    cv::KeyPoint pt;
    if (myfile.is_open())
    {
        while (std::getline(myfile, line))
        {
            std::vector<std::string> linedata = split(line, ' ');
            pt.pt.x = stod(linedata.at(0));
            pt.pt.y = stod(linedata.at(1));
            kps.emplace_back(pt);
        }
        myfile.close();
    }
    kps.shrink_to_fit();
    return kps;
}

TEST(test_fginn, test_calcDistanceMatrix)
{
    cv::Mat anchor{4, 3, CV_32F, cv::Scalar(0)};

    float cnt = 0;
    for (float *ptr = (float *)anchor.data;
         ptr < (float *)anchor.data + anchor.cols * anchor.rows;
         ptr++)
    {
        *ptr = cnt++;
    }

    cv::Mat positive{5, 3, CV_32F, cv::Scalar(0)};

    for (float *ptr = (float *)positive.data;
         ptr < (float *)positive.data + positive.cols * positive.rows;
         ptr++)
    {
        *ptr = cnt++;
    }

    std::vector<float> expected{
        20.784609, 25.980762, 31.176914, 36.373066, 41.569218,
        15.588457, 20.784609, 25.980762, 31.176914, 36.373066,
        10.392304, 15.588457, 20.784609, 25.980762, 31.176914,
        5.1961527, 10.392304, 15.588457, 20.784609, 25.980762};

    cv::Mat temp = fginn::calcDistanceMatrix(anchor, positive);

    std::vector<float> result;
    result.reserve(temp.rows * temp.cols);
    for (float *ptr = (float *)temp.data;
         ptr < (float *)temp.data + temp.cols * temp.rows;
         ptr++)
    {
        result.emplace_back(*ptr);
    }

    for (int i = 0; i < result.size(); i++)
        EXPECT_NEAR(expected.at(i), result.at(i), 1e-4);
}

TEST(test_fginn, test_findPairIndexes)
{
    std::vector<cv::Point> m1;
    m1.push_back(cv::Point{21, 1468});
    m1.push_back(cv::Point{22, 1467});
    m1.push_back(cv::Point{24, 2706});
    m1.push_back(cv::Point{26, 1473});
    m1.push_back(cv::Point{33, 945});
    m1.push_back(cv::Point{56, 1377});
    m1.push_back(cv::Point{63, 2992});
    m1.push_back(cv::Point{66, 177});
    m1.push_back(cv::Point{68, 1072});
    m1.push_back(cv::Point{79, 2471});
    m1.push_back(cv::Point{83, 1360});
    m1.push_back(cv::Point{84, 1359});
    m1.push_back(cv::Point{89, 2975});
    m1.push_back(cv::Point{90, 1367});
    m1.push_back(cv::Point{91, 1354});
    m1.push_back(cv::Point{95, 1372});
    m1.push_back(cv::Point{98, 1361});
    m1.push_back(cv::Point{99, 2375});
    m1.push_back(cv::Point{105, 1335});
    m1.push_back(cv::Point{108, 1299});
    m1.push_back(cv::Point{118, 1325});
    m1.push_back(cv::Point{130, 2988});
    m1.push_back(cv::Point{132, 2748});
    m1.push_back(cv::Point{139, 1329});
    m1.push_back(cv::Point{153, 1245});
    m1.push_back(cv::Point{155, 1362});

    std::vector<cv::Point> m2;
    m2.push_back(cv::Point{2678, 104});
    m2.push_back(cv::Point{2697, 67});
    m2.push_back(cv::Point{2706, 24});
    m2.push_back(cv::Point{2751, 185});
    m2.push_back(cv::Point{2775, 168});
    m2.push_back(cv::Point{2780, 60});
    m2.push_back(cv::Point{2803, 168});
    m2.push_back(cv::Point{2829, 16});
    m2.push_back(cv::Point{2859, 58});
    m2.push_back(cv::Point{2864, 168});
    m2.push_back(cv::Point{2867, 150});
    m2.push_back(cv::Point{2892, 162});
    m2.push_back(cv::Point{2913, 115});
    m2.push_back(cv::Point{2929, 34});
    m2.push_back(cv::Point{2950, 48});
    m2.push_back(cv::Point{2988, 130});
    m2.push_back(cv::Point{2990, 169});
    m2.push_back(cv::Point{2998, 29});

    auto out_m = fginn::findPairIndexes(m1, m2);

    std::vector<cv::Point> expected{cv::Point(24, 2706),
                                    cv::Point(130, 2988)};

    EXPECT_EQ(out_m, expected);
}

TEST(test_fginn, getFginnIndexes)
{
    cv::Mat dm = getDataFromFile("../tests/data/dm.txt",
                                 3000, 200);
    cv::transpose(dm, dm);
    cv::Mat km = getDataFromFile("../tests/data/km2.txt",
                                 200, 200);

    auto res = fginn::getFginnIndexes(dm, km);

    // std::cout << res.size() << std::endl;
    // for (auto &r : res)
    //     std::cout << r << std::endl;
}

TEST(test_fginn, calcDistanceMatrix)
{
    cv::Mat descr1 = getDataFromFile("../tests/data/desc1.txt",
                                     128, 200);
    cv::Mat descr2 = getDataFromFile("../tests/data/desc2.txt",
                                     128, 3000);
    auto kps1 = getKeyPtsFromFile("../tests/data/kps1.txt");
    auto kps2 = getKeyPtsFromFile("../tests/data/kps2.txt");

    cv::Mat xy1 = fginn::keyPointsToMat(kps1);
    cv::Mat xy2 = fginn::keyPointsToMat(kps2);

    cv::Mat dm_res = fginn::calcDistanceMatrix(descr1, descr2);
    cv::Mat km1_res = fginn::calcDistanceMatrix(xy2, xy2);
    cv::Mat km2_res = fginn::calcDistanceMatrix(xy1, xy1);

    cv::Mat dm_expected = getDataFromFile("../tests/data/dm.txt",
                                          3000, 200);

    cv::Mat km1_expected = getDataFromFile("../tests/data/km1.txt",
                                           3000, 3000);
    cv::Mat km2_expected = getDataFromFile("../tests/data/km2.txt",
                                           200, 200);

    float *res_ptr = (float *)dm_res.data;
    float *expected_ptr = (float *)dm_expected.data;
    for (int i = 0; i < dm_res.cols * dm_res.rows; i++)
    {
        EXPECT_NEAR(*res_ptr, *expected_ptr, 1e-4);
        res_ptr++;
        expected_ptr++;
    }

    // res_ptr = (float *)km1_res.data;
    // expected_ptr = (float *)km1_expected.data;
    // for (int i = 0; i < km1_res.cols * km1_res.rows; i++)
    // {
    //     if (!std::isnan(*res_ptr) && !std::isnan(*expected_ptr))
    //         EXPECT_NEAR(*res_ptr, *expected_ptr, 1e-1);
    //     res_ptr++;
    //     expected_ptr++;
    // }

    res_ptr = (float *)km2_res.data;
    expected_ptr = (float *)km2_expected.data;
    for (int i = 0; i < km2_res.cols * km2_res.rows; i++)
    {
        if (!std::isnan(*res_ptr) && !std::isnan(*expected_ptr))
            EXPECT_NEAR(*res_ptr, *expected_ptr, 1e-1);
        res_ptr++;
        expected_ptr++;
    }
}

// TEST(test_fginn, matchSymFginnIntersection)
// {
//     cv::Mat descr1 = getDataFromFile("/home/tsikunov/cpp_projects/OneShotDetector/tests/data/desc1.txt",
//                                      128, 200);
//     cv::Mat descr2 = getDataFromFile("/home/tsikunov/cpp_projects/OneShotDetector/tests/data/desc2.txt",
//                                      128, 3000);
//     auto kps1 = getKeyPtsFromFile("/home/tsikunov/cpp_projects/OneShotDetector/tests/data/kps1.txt");
//     auto kps2 = getKeyPtsFromFile("/home/tsikunov/cpp_projects/OneShotDetector/tests/data/kps2.txt");

//     std::vector<cv::DMatch> goodMatches = fginn::matchSymFginnIntersection(descr1, descr2, kps1, kps2);

//     for (auto &match : goodMatches)
//         std::cout << match.imgIdx << "\t" << match.queryIdx << std::endl;
// }

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}