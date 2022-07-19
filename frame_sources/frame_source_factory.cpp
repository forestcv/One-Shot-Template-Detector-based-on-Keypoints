#include "frame_source_factory.h"
#include "video/video_factory.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace frame_source;

Factory::Factory(const std::string &pathToSettings)
    : pathToSettings(pathToSettings)
{
}

std::shared_ptr<FrameSource> Factory::create()
{
    boost::property_tree::ptree root;
    boost::property_tree::read_json(pathToSettings, root);

    int sourceID = root.get<int>("frame_source_id");

    switch (sourceID)
    {
    case static_cast<int>(FrameSources::VIDEO):
    {
        VideoFactory videoFactory{};
        return videoFactory.create(root);
        break;
    }
    default:
        break;
    }

    return nullptr;
}
