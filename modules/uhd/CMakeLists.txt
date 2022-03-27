cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(uhd_module)

addModule()
useLibraries(uhd_loader PRIVATE)

findLibrary(UHD COMPONENTS uhd FIND_HEADER uhd.h FILE uhd HINTS $ENV{UHD_PKG_PATH} REQUIRED)
if(NOT WIN32)
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    findLibrary(Boost COMPONENTS chrono date_time filesystem program_options regex unit_test_framework serialization thread system)
endif()
useLibraries(UHD::uhd PRIVATE)