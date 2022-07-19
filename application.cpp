#include "application.h"
#include "frame_sources/frame_source.h"
#include "frame_sources/frame_source_factory.h"
#include "one_shot_detector/factory.h"

#include <opencv2/imgproc.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <functional>
#include <iostream>

Application::Application(const std::string &pathToSettings)
{
    frame_source::Factory fsFactory(pathToSettings + "/frame_source.json");
    frameSource = fsFactory.create();
    frameSource->registerFrameHandler(this);
    one_shot_detector::OsdFactory osdFactory(pathToSettings + "/detector.json");
    detector = osdFactory.create();
    readSettings(pathToSettings + "/main_settings.json");
}

void Application::start()
{
    cv::Mat reference =
        cv::imread(settings.pathToReference, cv::IMREAD_GRAYSCALE);
    detector->init(reference);
    frameSource->setup();
    frameSource->start();
    execution.store(true, std::memory_order_acquire);
    std::thread executionThread(&Application::exec, this);
    executionThread.detach();
}

void Application::finish()
{
    execution.store(false, std::memory_order_acquire);
}

void Application::exec()
{
    while (execution.load(std::memory_order_acquire))
    {
        // wait for new frame
        std::unique_lock<std::mutex> lock(frameMutex);
        frameReadyCV.wait(lock);

        cv::Rect target(0, 0, 0, 0);
        detector->detect(frame, target);
        // cv::Mat result;
        // cv::resize(frame, result, cv::Size(0, 0), 0.5, 0.5);
        // cv::imshow("Frame", result);
        keyHandler(cv::waitKey(1));
    }
    exit(0);
}

void Application::handleFrame(uint8_t *frameBuffer, int w, int h)
{
    frame = cv::Mat{h, w, CV_8UC3, frameBuffer};
    frameReadyCV.notify_one();
}

void Application::keyHandler(char key)
{
    switch (key)
    {
    case 27:
        finish();
        break;
    default:
        break;
    }
}

bool Application::readSettings(const std::string &pathToSettings)
{
    boost::property_tree::ptree root;
    boost::property_tree::read_json(pathToSettings, root);

    settings.pathToReference = root.get<std::string>("path_to_reference");

    return true;    
}