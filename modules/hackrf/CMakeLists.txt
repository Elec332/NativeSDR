cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(hackrf_module)

addModule()
findLibrary(libhackrf QUIET)
if(NOT TARGET libhackrf AND WIN32)
    #    findLibrary(pthreads ALT PThreads4W REQUIRED GLOBAL)
    get_target_property(LIBUSB_LIBRARIES libusb-1.0 INTERFACE_LINK_LIBRARIES)
    get_target_property(LIBUSB_INCLUDE_DIR libusb-1.0 INTERFACE_INCLUDE_DIRECTORIES)
    getRemoteLibrary(hackrf GIT_REPO https://github.com/greatscottgadgets/hackrf.git BUILD_ARGS
            "-DTHREADS_PTHREADS_WIN32_LIBRARY=${PThreads4W_LIBRARY_RELEASE}"
            "-DTHREADS_PTHREADS_INCLUDE_DIR=${PTHREAD_INCLUDE_DIR}"
            "-DLIBUSB_LIBRARIES=${LIBUSB_LIBRARIES}"
            "-DLIBUSB_INCLUDE_DIR=${LIBUSB_INCLUDE_DIR}"
            SOURCE_ROOT host/libhackrf
            )
    findLibrary(hackrf NAME libhackrf FIND_HEADER libhackrf/hackrf.h REQUIRED HINTS ${hackrf_INSTALL_DIR} ${hackrf_INSTALL_DIR}/bin)
endif()
useLibraries(libhackrf PRIVATE WIN_INSTALL modules)

