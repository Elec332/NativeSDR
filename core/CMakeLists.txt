cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(core)

addProject(LIB SHARED)
installRuntime(DEV)
setCentralOutput()

setRunPath(.:./lib)

#findLibrary(fftw3f NAME FFTW3 COMPONENTS fftw3f FILE libfftw3f-3 REQUIRED)
findLibrary(fftw3f ALT FFTW3 NAME FFTW3 COMPONENTS fftw3f FILE libfftw3f-3 REQUIRED)

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
useLibraries(libusb-1.0 PRIVATE)
getConfigurableProperty(LULOC libusb-1.0 IMPORTED_LOCATION)
useLibraries(FFTW3::fftw3f PRIVATE WIN_INSTALL / BIN_PREFIX "${LULOC}/../../../")
useLibraries(Volk::volk PUBLIC WIN_INSTALL / HEADER_INCLUDE_DIRS volk)

if(WIN32 AND NOT MSVC)
    findLibrary(orc-0.4 FILE orc-0.4-0)
    useLibraries(orc-0.4 PRIVATE WIN_INSTALL /)
endif()

if(WIN32)
    findLibrary(zlib FIND_HEADER zlib.h FILE zlib1 REQUIRED)
    useLibraries(zlib PRIVATE WIN_INSTALL /)
endif()

addTest()