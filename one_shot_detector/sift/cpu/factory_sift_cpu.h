#ifndef FACTORY_SIFT_CPU_H
#define FACTORY_SIFT_CPU_H

#include <memory>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "detector_sift_cpu.h"

namespace one_shot_detector::sift::cpu
{
    /* !
    SIFT-based one-shot detector concrete factory
    */
    class Factory
    {
    public:
        /* !
        Create SIFT-based one-shot detector
        \param[in] property tree from json configuration file
        \return shared pointer to SIFT-based one-shot detector
        */
        std::shared_ptr<Detector> create(const boost::property_tree::ptree &settingsRoot)
        {
            std::shared_ptr<DetectorSettigns> settings = std::make_shared<DetectorSettigns>();
            settings->referenceSettings.nFeatures = settingsRoot.get<int>("sift.reference.nFeatures");
            settings->referenceSettings.nOctaveLayers = settingsRoot.get<int>("sift.reference.nOctaveLayers");
            settings->referenceSettings.contrastThreshold = settingsRoot.get<double>("sift.reference.contrastThreshold");
            settings->referenceSettings.edgeThreshold = settingsRoot.get<double>("sift.reference.edgeThreshold");
            settings->referenceSettings.sigma = settingsRoot.get<double>("sift.reference.sigma");

            settings->frameSettings.nFeatures = settingsRoot.get<int>("sift.frame.nFeatures");
            settings->frameSettings.nOctaveLayers = settingsRoot.get<int>("sift.frame.nOctaveLayers");
            settings->frameSettings.contrastThreshold = settingsRoot.get<double>("sift.frame.contrastThreshold");
            settings->frameSettings.edgeThreshold = settingsRoot.get<double>("sift.frame.edgeThreshold");
            settings->frameSettings.sigma = settingsRoot.get<double>("sift.frame.sigma");

            settings->goodMatchesThreshold = settingsRoot.get<int>("sift.goodMatchesThreshold");

            return std::make_shared<Detector>(settings);
        }
    };
}

#endif