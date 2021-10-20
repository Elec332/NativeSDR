cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
project(NativeSDR_core)

include(${CMAKE_INCLUDE})
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /)

target_link_libraries(NativeSDR_core PRIVATE ModuleManager)
target_include_directories(${PROJECT_NAME} PUBLIC ../module_manager/src/public)
#target_link_libraries(NativeSDR_core PRIVATE NativeSDR_graphics)