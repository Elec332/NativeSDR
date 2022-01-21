cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(NativeSDR_audio-sink)

include(${MODULES_INCLUDE})
createSharedLibrary(portaudio-2.0 USE FILE libportaudio INSTALL /)
