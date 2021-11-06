file(GLOB_RECURSE SRC src/cpp/*.cpp src/cpp/*.c src/cpp/*.hpp src/cpp/*.h)
add_library(${PROJECT_NAME} SHARED ${SRC} ${PROJECT_BINARY_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE src/headers)
target_include_directories(${PROJECT_NAME} PUBLIC src/public)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})

function(addLibraryRoot name)
    if(MSVC)
        target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/libs/${name}/include)
        target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/libs/${name}/lib)
    else()
        #todo: Linux ect
    endif()
endfunction()

function(linkLibrary name sub)
    addLibraryRoot(${name})
    if(MSVC)
        target_link_libraries(${PROJECT_NAME} PUBLIC ${sub})
    else()
        #todo: Linux ect
    endif()
endfunction()

function(installLibrary name file)
    addLibraryRoot(${name})
    if(MSVC)
        install(FILES ${PROJECT_SOURCE_DIR}/libs/${name}/bin/${file}.dll DESTINATION /)
    else()
        #todo: Linux ect
    endif()
endfunction()

function(addLibrary name sub)
    addLibraryRoot(${name})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${sub})
    installLibrary(${name} ${sub})
endfunction()

#todo: Force-overwrite assets. Install DIRECTORY doesnt do this, but install FILES doesnt do so either
if(EXISTS ${PROJECT_SOURCE_DIR}/assets/)
    file(GLOB files "assets/*.*")
    foreach(file ${files})
        install(FILES "${file}" DESTINATION assets)
    endforeach()
endif()