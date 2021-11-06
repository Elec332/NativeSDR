file(GLOB_RECURSE SRC src/cpp/*.cpp src/cpp/*.c src/cpp/*.hpp src/cpp/*.h)
add_library(${PROJECT_NAME} SHARED ${SRC} ${PROJECT_BINARY_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE src/headers)
target_include_directories(${PROJECT_NAME} PUBLIC src/public)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})