cmake_minimum_required(VERSION 3.21)
project(NativeSDR)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 14)

set(CMAKE_BASE_FOLDER ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/build/install")
set(MODULES_INCLUDE ${CMAKE_BASE_FOLDER}/DefaultModule.cmake)
set(ROOT_INCLUDE ${CMAKE_BASE_FOLDER}/CMakeBase.cmake)

include(${ROOT_INCLUDE})
fetchGTest(VERSION release-1.11.0)
#setCentralOutput()
forceX64()

add_subdirectory("module_manager")
add_subdirectory("graphics")
add_subdirectory("core")
add_subdirectory("main")

add_subdirectory("testmodule2")
add_subdirectory("rtl-sdr")
