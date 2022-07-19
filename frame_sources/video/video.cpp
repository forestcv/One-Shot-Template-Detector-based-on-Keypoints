#include "video.h"

#include <opencv2/videoio.hpp>

#include <iostream>
#include <thread>
#include <chrono>

using namespace frame_source;

Video::Video(const VideoSettings &settings) : settings(settings)
{
}

void Video::setup()
{
    capture = std::make_shared<cv::VideoCapture>(settings.path);
    if (!capture->isOpened())
    {
        std::cout << "ERROR: Video " << settings.path << "open error" << std::endl;
    }
    frameDelayMSec = 1000 / settings.fps;
}

void Video::start()
{
    execution.store(true, std::memory_order_acquire);
    std::thread executionThread(&Video::exec, this);
    executionThread.detach();

    frameSending.store(true, std::memory_order_acquire);
    std::thread frameSendingThread(&Video::sendFrame, this);
    frameSendingThread.detach();
}

void Video::finish()
{
    execution.store(false, std::memory_order_acquire);
}

void Video::exec()
{
    while (execution.load(std::memory_order_acquire))
    {
        auto completionTime = std::chrono::system_clock::now() +
                         std::chrono::milliseconds(frameDelayMSec);
        getNextFrame();
        std::this_thread::sleep_until(completionTime);
    }
}

bool Video::getNextFrame()
{
    if (capture->read(frame))
    {
        frameReadyCV.notify_one();
    }
    return false;
}

void Video::sendFrame()
{
    while (frameSending.load(std::memory_order_acquire))
    {
        std::unique_lock lock(frameMutex);
        frameReadyCV.wait(lock);
        for(auto handler : handlers)
        {
            handler->handleFrame(frame.data, frame.cols, frame.rows);
        }
    }
}