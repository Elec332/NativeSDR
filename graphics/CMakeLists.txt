cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(NativeSDR_graphics)

addProject(LIB SHARED)
installRuntime()

add_compile_definitions(IMGUI_USER_CONFIG=<nativesdr_graphics_export.h>)
add_compile_definitions(IMGUI_API=NATIVESDR_GRAPHICS_EXPORT)
add_compile_definitions(IMPLOT_API=NATIVESDR_GRAPHICS_EXPORT)

createSharedLibrary(glew USE FILE glew32 INSTALL /)
createSharedLibrary(GLFW USE LIB glfw3dll DLL glfw3 INSTALL /)
