file(GLOB_RECURSE SRC src/cpp/*.cpp src/cpp/*.c src/cpp/*.hpp src/cpp/*.h)
add_library(${PROJECT_NAME} SHARED ${SRC} ${PROJECT_BINARY_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE src/headers)
target_include_directories(${PROJECT_NAME} PUBLIC src/public)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})

#todo: Force-overwrite assets. Install DIRECTORY doesnt do this, but install FILES doesnt do so either
if(EXISTS ${PROJECT_SOURCE_DIR}/assets/)
    file(GLOB files "assets/*.*")
    foreach(file ${files})
        install(FILES "${file}" DESTINATION assets)
    endforeach()
endif()