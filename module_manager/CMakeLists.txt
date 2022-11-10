cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
project(ModuleManager)

addProject(LIB STATIC)

if(NOT WIN32)
    target_compile_options(${PROJECT_NAME} PUBLIC -fPIC)
else()
    add_compile_definitions(LL_EXPORT=__declspec\(dllexport\))
endif()
