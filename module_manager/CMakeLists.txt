cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
project(ModuleManager)

file(GLOB_RECURSE SRC "src/cpp/*.cpp")

add_library(${PROJECT_NAME} STATIC ${SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${PROJECT_NAME} PUBLIC "src/public")