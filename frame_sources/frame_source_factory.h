#ifndef FRAME_SOURCE_FACTORY_H
#define FRAME_SOURCE_FACTORY_H

#include <memory>
#include <string>

#include "frame_source.h"

using namespace frame_source;

namespace frame_source
{
    /*!
    Abstract frame source factory
    */
    class Factory
    {
    public:
        /*!
        Constructor of abstract frame source factory
        \param [in] pathToSettings path to json configuration file
        */
        Factory(const std::string &pathToSettings);
        /*!
        Read configuartion from JSON and ask concreate factory to create frame source
        \return Shared pointer to frame source of concreate type
        */
        std::shared_ptr<FrameSource> create();

    private:
        // path to json
        std::string pathToSettings;
    };
}

#endif // FRAME_SOURCE_FACTORY_H
