cmake_minimum_required(VERSION 3.21)
project(NativeSDR)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 14)

set(CMAKE_BASE_FOLDER ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/build/install")

include(${CMAKE_BASE_FOLDER}/DefaultModuleConfig.cmake)
set(ENABLE_UNIT_TESTS FALSE)
fetchGTest(VERSION release-1.11.0)
forceX64()

makeRootCMakeConfig(FILES ${CMAKE_BASE_FOLDER}/DefaultModuleConfig.cmake ${CMAKE_BASE_FOLDER}/CMakeBase.cmake)

checkInstallFiles()
set(EXPORT_HEADER_BASE_DIR nativesdr)


findLibrary(libusb-1.0 FIND_HEADER libusb-1.0/libusb.h HEADER_DIR libusb-1.0 REQUIRED)
findLibrary(pthreads ALT PThreads4W REQUIRED GLOBAL)

useLibraries(libusb-1.0 PRIVATE WIN_INSTALL / HEADER_INCLUDE_DIRS /)
if(WIN32)
    useLibraries(PThreads4W::PThreads4W PRIVATE WIN_INSTALL /)
endif()

add_subdirectory(module_manager)
add_subdirectory(graphics)
add_subdirectory(core)
add_subdirectory(main)

add_subdirectory(modules)
