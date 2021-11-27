cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(NativeSDR_rtl-sdr)

include(${MODULES_INCLUDE})
createSharedLibrary(librtlsdr USE INSTALL modules DEPENDENCIES libusb)