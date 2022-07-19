#ifndef VIDEO_FACTORY_H
#define VIDEO_FACTORY_H

#include <memory>
#include <string>

#include <boost/property_tree/ptree.hpp>

#include "../frame_source.h"

using namespace frame_source;

namespace frame_source
{
    class VideoFactory
    {
        public:
            std::shared_ptr<FrameSource> create(boost::property_tree::ptree &settingsRoot);
    };
}

#endif
