cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(core)

addProject(LIB SHARED)
installRuntime(DEV)
setCentralOutput()

findLibrary(fftw3f NAME FFTW3 COMPONENTS fftw3f FILE libfftw3f-3)
findLibrary(Volk COMPONENTS volk)
findLibrary(libusb-1.0 FIND_HEADER libusb-1.0/libusb.h HEADER_DIR libusb-1.0)

useHeaderLibrary(json)
useLibraries(ModuleManager PUBLIC)
useLibraries(NativeSDR::graphics PRIVATE)
useLibraries(NAMES FFTW3::fftw3f libusb-1.0 PRIVATE WIN_INSTALL /)
useLibraries(Volk::volk PUBLIC WIN_INSTALL /)

if(WIN32 AND NOT MSVC)
    findLibrary(orc-0.4 FILE orc-0.4-0)
    useLibraries(orc-0.4 PRIVATE WIN_INSTALL /)
endif()

addTest()