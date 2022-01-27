cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(core)

addProject(LIB SHARED)
installRuntime(DEV)
setCentralOutput()

useLibrary(ModuleManager PUBLIC)
useLibrary(NativeSDR::graphics)

createSharedLibrary(fftw3f NAME FFTW3 COMPONENTS fftw3f USE FILE libfftw3f-3 INSTALL /)
createSharedLibrary(Volk COMPONENTS volk USE PUBLIC INSTALL /)

if(WIN32 AND NOT MSVC)
    createSharedLibrary(orc-0.4 USE FILE orc-0.4-0 INSTALL /)
endif()
createSharedLibrary(libusb-1.0 FIND_HEADER libusb-1.0/libusb.h HEADER_DIR libusb-1.0 PUBLIC INSTALL / GLOBAL)
useHeaderLibrary(json)

addTest()