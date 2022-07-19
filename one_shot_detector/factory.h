#ifndef FACTORY_ONE_SHOT_DETECTOR_H
#define FACTORY_ONE_SHOT_DETECTOR_H

#include <memory>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "detector.h"
#include "orb/cpu/factory_orb_cpu.h"
#include "sift/cpu/factory_sift_cpu.h"

namespace one_shot_detector
{
    // factory for one-shot detector of different type prouction
    class OsdFactory
    {
    public:
        /*!
        Constructor of factory
        \param[in] pathToSettings Path to JSON file with configuration
        */
        OsdFactory(const std::string &pathToSettings) : pathToSettings(pathToSettings) {}
        /*!
        Read configuartion from JSON and ask concreate factory to create one-shot detector
        \return Shared pointer to one-shot detector of concreate type 
        */
        std::shared_ptr<OneShotDetector> create()
        {
            boost::property_tree::ptree root;
            boost::property_tree::read_json(pathToSettings, root);

            int detectorID = root.get<int>("detector_id");

            switch (detectorID)
            {
            case static_cast<int>(DetectorType::ORB):
            {
                orb::cpu::Factory orbFactory{};
                return orbFactory.create(root);
                break;
            }
            case static_cast<int>(DetectorType::SIFT):
            {
                sift::cpu::Factory siftFactory{};
                return siftFactory.create(root);
                break;
            }
            default:
                break;
            }

            return nullptr;
        }

    private:
        // path to json
        std::string pathToSettings;
    };
}

#endif // FACTORY_ONE_SHOT_DETECTOR_H