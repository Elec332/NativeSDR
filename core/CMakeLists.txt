cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(NativeSDR_core)

addProject(LIB SHARED)
installRuntime()

useLibrary(ModuleManager PUBLIC)
useLibrary(NativeSDR_graphics)

createSharedLibrary(fftw USE FILE libfftw3f-3 INSTALL /)
createSharedLibrary(volk USE PUBLIC INSTALL /)
useHeaderLibrary(json)

#########################

addTest(NativeSDR_graphics)