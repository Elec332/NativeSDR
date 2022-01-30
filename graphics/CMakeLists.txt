cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(graphics)

addProject(LIB SHARED)
installRuntime(DEV)

add_compile_definitions(IMGUI_USER_CONFIG=<graphics_export.h>)
add_compile_definitions(IMGUI_API=GRAPHICS_EXPORT)
add_compile_definitions(IMPLOT_API=GRAPHICS_EXPORT)
add_compile_definitions(IMGUI_IMPL_API=)

findLibrary(GLEW COMPONENTS glew)
findLibrary(GLFW3 NAME glfw)

useLibraries(NAMES GLEW::glew glfw PRIVATE WIN_INSTALL /)

useSourceLibrary(stb PRIVATE)
useSourceLibrary(imgui)
useSourceLibrary(imgui-stdlib)
useSourceLibrary(implot)
useSourceLibrary(imgui_node_editor)
useSourceLibrary(imgui_node_editor_blueprints)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC opengl32)
    target_link_libraries(${PROJECT_NAME} PRIVATE imm32)
endif()