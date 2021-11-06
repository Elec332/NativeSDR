cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
project(NativeSDR_graphics)

include(${CMAKE_INCLUDE})
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /)

add_compile_definitions(IMGUI_USER_CONFIG=<nativesdr_graphics_export.h>)
add_compile_definitions(IMGUI_API=NATIVESDR_GRAPHICS_EXPORT)
add_compile_definitions(IMPLOT_API=NATIVESDR_GRAPHICS_EXPORT)

addLibrary(glew glew32)
installLibrary(GLFW glfw3)

if(MSVC)
    target_link_libraries(${PROJECT_NAME} PUBLIC glfw3dll)
else()
    #todo: Linux ect
endif()
