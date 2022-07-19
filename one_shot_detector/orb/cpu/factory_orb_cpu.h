#ifndef FACTORY_ORB_CPU_H
#define FACTORY_ORB_CPU_H

#include <memory>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "detector_orb_cpu.h"

namespace one_shot_detector::orb::cpu
{
    /* !
    ORB-based one-shot detector concrete factory
    */
    class Factory
    {
    public:
        /* !
        Create ORB-based one-shot detector
        \param[in] property tree from json configuration file
        \return shared pointer to ORB-based one-shot detector
        */
        std::shared_ptr<Detector> create(const boost::property_tree::ptree &settingsRoot)
        {
            std::shared_ptr<DetectorSettigns> settings = std::make_shared<DetectorSettigns>();
            settings->referenceSettings.nFeatures = settingsRoot.get<int>("orb.reference.nFeatures");
            settings->referenceSettings.scaleFactor = settingsRoot.get<float>("orb.reference.scaleFactor");
            settings->referenceSettings.nLevels = settingsRoot.get<int>("orb.reference.nLevels");
            settings->referenceSettings.edgeThreshold = settingsRoot.get<int>("orb.reference.edgeThreshold");
            settings->referenceSettings.firstLevel = settingsRoot.get<int>("orb.reference.firstLevel");
            settings->referenceSettings.wtaK = settingsRoot.get<int>("orb.reference.wtaK");
            settings->referenceSettings.fastScore = settingsRoot.get<int>("orb.reference.fastScore");
            settings->referenceSettings.patchSize = settingsRoot.get<int>("orb.reference.patchSize");
            settings->referenceSettings.fastThreshold = settingsRoot.get<int>("orb.reference.fastThreshold");

            settings->frameSettings.nFeatures = settingsRoot.get<int>("orb.frame.nFeatures");
            settings->frameSettings.scaleFactor = settingsRoot.get<float>("orb.frame.scaleFactor");
            settings->frameSettings.nLevels = settingsRoot.get<int>("orb.frame.nLevels");
            settings->frameSettings.edgeThreshold = settingsRoot.get<int>("orb.frame.edgeThreshold");
            settings->frameSettings.firstLevel = settingsRoot.get<int>("orb.frame.firstLevel");
            settings->frameSettings.wtaK = settingsRoot.get<int>("orb.frame.wtaK");
            settings->frameSettings.fastScore = settingsRoot.get<int>("orb.frame.fastScore");
            settings->frameSettings.patchSize = settingsRoot.get<int>("orb.frame.patchSize");
            settings->frameSettings.fastThreshold = settingsRoot.get<int>("orb.frame.fastThreshold");

            settings->matcherThreshold = settingsRoot.get<int>("orb.matcherThreshold");
            settings->goodMatchesThreshold = settingsRoot.get<int>("orb.goodMatchesThreshold");

            return std::make_shared<Detector>(settings);
        }
    };
}

#endif