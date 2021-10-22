cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
project(NativeSDR_graphics)

include(${CMAKE_INCLUDE})
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /)

add_compile_definitions(IMGUI_USER_CONFIG=<nativesdr_graphics_export.h>)
add_compile_definitions(IMGUI_API=NATIVESDR_GRAPHICS_EXPORT)

if(MSVC)
    target_include_directories(${PROJECT_NAME} PUBLIC
            ${PROJECT_SOURCE_DIR}/libs/glew/include
            ${PROJECT_SOURCE_DIR}/libs/GLFW/include
    )

    target_link_directories(${PROJECT_NAME} PUBLIC
            ${PROJECT_SOURCE_DIR}/libs/glew/lib
            ${PROJECT_SOURCE_DIR}/libs/GLFW/lib
    )

    target_link_libraries(${PROJECT_NAME} PUBLIC
            glfw3dll
            glew32
    )

    install(FILES ${PROJECT_SOURCE_DIR}/libs/glew/bin/glew32.dll DESTINATION /)
    install(FILES ${PROJECT_SOURCE_DIR}/libs/GLFW/bin/glfw3.dll DESTINATION /)
else()
    #todo: Linux ect
endif()
