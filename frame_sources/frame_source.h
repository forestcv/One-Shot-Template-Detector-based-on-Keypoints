#ifndef FRAME_SOURCE_H
#define FRAME_SOURCE_H

#include <memory>
#include <atomic>
#include <vector>

#include "frame_handler.h"

namespace frame_source
{
    enum class FrameSources
    {
        VIDEO = 0,
        FOLDER = 1,
        WEBCAMERA = 2
    };

    /*!
    Frame source abstract class
    */
    class FrameSource
    {
    public:
        /*!
        Subscribe to frame update
        \param [in] pointer to frame handler instance
        */
        void registerFrameHandler(IFrameHandler *handler)
        {
            handlers.emplace_back(handler);
        }
        virtual void setup() = 0;
        virtual void start() = 0;
        virtual void finish() = 0;

    protected:
        std::vector<IFrameHandler *> handlers;
        std::atomic<bool> execution;
        std::atomic<bool> frameSending;


    protected:
        virtual void exec() = 0;
    };
}

#endif // FRAME_SOURCE_H
