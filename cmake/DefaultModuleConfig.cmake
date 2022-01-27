include(${CMAKE_CURRENT_LIST_DIR}/CMakeBase.cmake)

function(addModule)
    addProject(LIB SHARED)

    useLibrary(NativeSDR::core)
    useLibrary(NativeSDR::graphics)

    installRuntime(DESTINATION modules)
    add_custom_target(${PROJECT_NAME}_run COMMAND NativeSDR::main -md ${CMAKE_CURRENT_BINARY_DIR})
endfunction()
