include(${CMAKE_INCLUDE})

add_compile_definitions(IMGUI_USER_CONFIG=<nativesdr_graphics_export.h>)
add_compile_definitions(IMGUI_API=NATIVESDR_GRAPHICS_EXPORT)

target_link_libraries(${PROJECT_NAME} PRIVATE NativeSDR_core)
#target_link_libraries(${PROJECT_NAME} PRIVATE NativeSDR_graphics)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION modules)
