include(${CMAKE_INCLUDE})

target_link_libraries(${PROJECT_NAME} PRIVATE NativeSDR_core)
target_link_libraries(${PROJECT_NAME} PRIVATE NativeSDR_graphics)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION modules)
