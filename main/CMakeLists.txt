cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(NativeSDR_main)

addProject(EXEC)
installRuntime()

useLibrary(NativeSDR_core)