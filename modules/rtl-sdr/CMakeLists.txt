cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(NativeSDR_rtl-sdr)

addModule()
#set(LIBUSB_FOUND TRUE)# "-DLIBUSB_FOUND=TRUE"
#getRemoteLibrary(rtlsdr GIT_REPO https://github.com/librtlsdr/librtlsdr.git GIT_TAG v0.8.0 BUILD_ARGS "-DTHREADS_FOUND=TRUE" "-DRTL_STATIC_BUILD=OFF" "-DLIBUSB_FOUND=TRUE" FIND rtlsdr)

findLibrary(rtlsdr COMPONENTS rtlsdr)#DEPENDENCIES libusb
useLibraries(rtlsdr::rtlsdr PRIVATE WIN_INSTALL ${MODULE_DIR})
##TODO: This module is only a POC