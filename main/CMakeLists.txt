cmake_minimum_required(VERSION 3.20)
project(NativeSDR_main)

set(CMAKE_CXX_STANDARD 17)

add_executable(NativeSDR_main main.cpp)
target_link_libraries(NativeSDR_main PRIVATE NativeSDR_core)

install(TARGETS NativeSDR_main DESTINATION /)