cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(NativeSDR_rtl-sdr)

addModule()
createSharedLibrary(rtlsdr COMPONENTS rtlsdr USE INSTALL modules)#DEPENDENCIES libusb