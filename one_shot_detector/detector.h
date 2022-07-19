#ifndef DETECTOR_H
#define DETECTOR_H

#include <opencv2/core.hpp>

namespace one_shot_detector
{
    /*!
    Draw reference image projection as quadrangle on image
    \param [in, out] image image for drawing (consist of reference and current frame)
    \param [in] referenceSize size of reference
    \param [in] corners points of image projection quadrangle
    */
    void drawImageAlignment(cv::Mat &image,
                            const cv::Size &referenceSize,
                            const std::vector<cv::Point2f> &corners);
    /*!
    Find reference projection on current frame by their matched keypoints
    // https://docs.opencv.org/3.4/d7/dff/tutorial_feature_homography.html
    \param [in] referenceKeyPts keypoints of reference image
    \param [in] frameKeyPts keypoints of current frame
    \param [in] goodMatches list of matched keypoints
    \param [in] referenceSize size of reference
    \param [in, out] corners points of image projection quadrangle
    */
    void findAlignment(const std::vector<cv::KeyPoint> &referenceKeyPts,
                       const std::vector<cv::KeyPoint> &frameKeyPts,
                       const std::vector<cv::DMatch> &goodMatches,
                       const cv::Size &referenceSize,
                       std::vector<cv::Point2f> &corners);

    /*!
    Find matches of keypoints with FGINN method
    \param [in] referenceDescr descriptors of reference's keypoints
    \param [in] frameDescr descriptors of current frame's keypoints
    \param [in] referenceKeyPts keypoints of reference image
    \param [in] frameKeyPts keypoints of current frame
    \param [in, out] goodMatches list of matched keypoints
    */
    void matchKeyPoints(const cv::Mat &referenceDescr,
                        const cv::Mat &frameDescr,
                        const std::vector<cv::KeyPoint> &referenceKeyPts,
                        const std::vector<cv::KeyPoint> &frameKeyPts,
                        std::vector<cv::DMatch> &goodMatches);

    /*!
    Find matches of keypoints
    https://docs.opencv.org/3.4/d5/d6f/tutorial_feature_flann_matcher.html
    \param [in] referenceDescr descriptors of reference's keypoints
    \param [in] frameDescr descriptors of current frame's keypoints
    \param [in] threshold distance of knn-method threshold
    \param [in, out] goodMatches list of matched keypoints
    */
    void matchKeyPoints(const cv::Mat &referenceDescriptor,
                        const cv::Mat &frameDescriptor,
                        double threshold,
                        std::vector<cv::DMatch> &goodMatches);

    struct ReferenceData
    {
        cv::Mat image;
        std::vector<cv::KeyPoint> keyPts;
        cv::Mat descriptor;
    };

    enum class DetectorType
    {
        ORB = 0,
        SIFT = 1
    };

    /*!
    Abstract one-shot detector
    */
    class OneShotDetector
    {
    public:
        /*!
        Initialize detector with reference image, calculate keypoints and
        their descriptors
        \param [in] reference reference image
        */
        virtual bool init(const cv::Mat &reference) = 0;
        /*!
        Find reference projection on current frame
        \param [in] frame current frame
        \param [in, out] target rectangle as reference projection on current frame
        */
        virtual bool detect(const cv::Mat &frame, cv::Rect &target) = 0;

    protected:
        /*!
        Prepare reference data for feature processing:
        convert reference to grayscale if nesessary, 
        calculate keypoints and their descriptors 
        \param [in] reference reference image
        */
        virtual void prepareReferenceData(const cv::Mat &reference) = 0;
        /*!
        Prepare current frame for keypoints calculation:
        convert to grayscale if nesessary and other
        \param [in] frame current frame
        \param [in, out] processedImage image for processing
        */
        virtual void prepareImage(const cv::Mat &frame, cv::Mat &processedImage) = 0;
        /*!
        Template method for abstract keypoints and descriptor calculation
        \param [in] img image
        \param [in] detectionMethod pointer to keypoint detection method
        \param [in] computeMethod pointer to descriptor calculation method
        \param [in, out] keyPts image keypoints
        \param [in, out] descriptor keypoints descriptors
        */
        template <typename InputImg, typename T1, typename T2>
        void getKeyPtsAndDescr(const InputImg &img,
                               const cv::Ptr<T1> &detectionMethod,
                               const cv::Ptr<T2> &computeMethod,
                               std::vector<cv::KeyPoint> &keyPts, InputImg &descriptor)
        {
            detectionMethod->detect(img, keyPts);
            computeMethod->compute(img, keyPts, descriptor);
        }
        /*!
        Check if descriptors mat isn't empty and convert it to nesessary data type
        \param [in, out] descriptor keypoints descriptors
        */
        virtual void convertDescriptors(cv::Mat &descriptors) = 0;
    };
}

#endif // DETECTOR_H