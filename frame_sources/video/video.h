#ifndef VIDEO_FRAME_SOURCE_H
#define VIDEO_FRAME_SOURCE_H

#include <string>
#include <condition_variable>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include "../frame_source.h"

using namespace frame_source;

namespace frame_source
{
    struct VideoSettings
    {
        std::string path;
        int fps = 30;
        bool repeat = false;
    };

    //    class Video : piblic FrameSource
    class Video : public FrameSource
    {
    public:
        Video(const VideoSettings &settings);
        void setup() override;
        void start() override;
        void finish() override;

    private:
        VideoSettings settings;
        std::shared_ptr<cv::VideoCapture> capture = nullptr;
        cv::Mat frame;
        int frameDelayMSec = 0;
        std::condition_variable frameReadyCV;
        std::mutex frameMutex;

    private:
        void exec() override;
        bool getNextFrame();
        void sendFrame();
    };
}

#endif // VIDEO_FRAME_SOURCE_H
