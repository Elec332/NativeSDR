cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
project(NativeSDR LANGUAGES CXX)

if(MSVC)
    set(CMAKE_GENERATOR_PLATFORM x64)
endif()

set(CMAKE_BASE_FOLDER ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/build/install")
set(CMAKE_INCLUDE ${CMAKE_BASE_FOLDER}/DefaultCMake.cmake)
set(MODULES_INCLUDE ${CMAKE_BASE_FOLDER}/DefaultModule.cmake)
set(GOOGLETEST_VERSION release-1.11.0)

add_subdirectory("main")
add_subdirectory("core")
add_subdirectory("module_manager")
add_subdirectory("graphics")

add_subdirectory("testmodule2")
