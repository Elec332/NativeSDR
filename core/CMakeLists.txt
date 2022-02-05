cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(core)

addProject(LIB SHARED)
installRuntime(DEV)
setCentralOutput()

findLibrary(fftw3f NAME FFTW3 COMPONENTS fftw3f FILE libfftw3f-3 REQUIRED)
findLibrary(libusb-1.0 FIND_HEADER libusb-1.0/libusb.h HEADER_DIR libusb-1.0 REQUIRED)

findLibrary(Volk COMPONENTS volk QUIET)
if(NOT TARGET Volk::volk)
    findPython()
    ensurePythonModule("mako >= 0.4.2" mako "mako.__version__ >= '0.4.2'")
    getRemoteLibrary(volk GIT_REPO https://github.com/gnuradio/volk.git GIT_TAG main BUILD_ARGS "-DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}" FIND Volk)
endif()

getRemoteLibrary(json GIT_REPO https://github.com/nlohmann/json.git GIT_TAG v3.10.5 NO_BUILD)
includeDirectory(${json_SOURCE_DIR}/single_include PUBLIC)

useLibraries(ModuleManager PUBLIC STATIC_HEADERS)
useLibraries(NativeSDR::graphics PRIVATE)
useLibraries(NAMES FFTW3::fftw3f libusb-1.0 PRIVATE WIN_INSTALL /)
useLibraries(Volk::volk PUBLIC WIN_INSTALL / HEADER_INCLUDE_DIRS volk)

if(WIN32 AND NOT MSVC)
    findLibrary(orc-0.4 FILE orc-0.4-0)
    useLibraries(orc-0.4 PRIVATE WIN_INSTALL /)
endif()

addTest()