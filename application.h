#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <mutex>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "frame_sources/frame_source.h"
#include "one_shot_detector/detector.h"

class Application : private IFrameHandler
{
public:
    Application(const std::string &settingsPath);
    void start();
    void finish();

private:
    std::atomic<bool> execution;
    std::shared_ptr<frame_source::FrameSource> frameSource = nullptr;
    std::condition_variable frameReadyCV;
    std::mutex frameMutex;
    cv::Mat frame;
    std::shared_ptr<one_shot_detector::OneShotDetector> detector = nullptr;

private:
    void exec();
    void handleFrame(uint8_t *frameBuffer,
                     int w,
                     int h) override;
    void keyHandler(char key);
    bool readSettings(const std::string &pathToSettings);

    struct ApplicationSettings
    {
        std::string pathToReference;
    } settings;
};

#endif // APPLICATION_H