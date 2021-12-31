cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(NativeSDR_core)

addProject(LIB SHARED)
installRuntime()

useLibrary(ModuleManager PUBLIC)
useLibrary(NativeSDR_graphics)

createSharedLibrary(fftw3f USE FILE libfftw3f-3 INSTALL /)
createSharedLibrary(volk USE PUBLIC INSTALL /)
createSharedLibrary(libusb-1.0 PUBLIC INSTALL / GLOBAL)
useHeaderLibrary(json)

addTest()