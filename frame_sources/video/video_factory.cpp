#include "video_factory.h"
#include "video.h"

#include <boost/property_tree/json_parser.hpp>

using namespace frame_source;

std::shared_ptr<FrameSource> VideoFactory::create(boost::property_tree::ptree &settingsRoot)
{
    VideoSettings settings{};

    settings.path = settingsRoot.get<std::string>("video.path");
    settings.fps = settingsRoot.get<int>("video.fps");
    settings.repeat = settingsRoot.get<bool>("video.repeat");

    std::shared_ptr<FrameSource> video = std::make_shared<Video>(settings);

    return video;
}
