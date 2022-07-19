cmake_minimum_required(VERSION 3.2)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(KeyPtBasedOneShotDetector VERSION ${PROJECT_VERSION})

add_executable(${PROJECT_NAME}
    main.cpp
    application.h
    application.cpp

    ### Detector ###
    one_shot_detector/detector.cpp
    one_shot_detector/detector.h
    one_shot_detector/factory.h
    ##### ORB #####
    one_shot_detector/orb/orb_detector.h
    one_shot_detector/orb/cpu/detector_orb_cpu.cpp
    one_shot_detector/orb/cpu/detector_orb_cpu.h
    one_shot_detector/orb/cpu/factory_orb_cpu.h
    ##### SIFT #####
    one_shot_detector/sift/sift_detector.h
    one_shot_detector/sift/cpu/detector_sift_cpu.cpp
    one_shot_detector/sift/cpu/detector_sift_cpu.h
    one_shot_detector/sift/cpu/factory_sift_cpu.h

    ### FGINN ###
    fginn/fginn.cpp
    fginn/fginn.h

    # ## Frame Sources ###
    frame_sources/frame_source.h
    frame_sources/frame_handler.h
    frame_sources/frame_source_factory.h
    frame_sources/frame_source_factory.cpp

    # #### Video #####
    frame_sources/video/video_factory.h
    frame_sources/video/video_factory.cpp
    frame_sources/video/video.h
    frame_sources/video/video.cpp
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON)

set(OpenCV_DIR /home/artem/OpenCV/install_4.5.5)
find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)
find_package(Boost 1.4 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} -lpthread)

option(WITH_GTEST "Whether to build Gtest test" ON)

if(WITH_GTEST)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(tests 
        tests/tests.cpp
        fginn/fginn.cpp
        fginn/fginn.h
    )

    set_target_properties(tests PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )

    target_link_libraries(tests
        gtest_main
        ${OpenCV_LIBS}
    )
endif()
