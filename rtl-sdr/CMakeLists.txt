cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(NativeSDR_rtl-sdr)

addModule()
findLibrary(rtlsdr COMPONENTS rtlsdr)#DEPENDENCIES libusb
useLibraries(rtlsdr::rtlsdr PRIVATE WIN_INSTALL ${MODULE_DIR})