include(${CMAKE_CURRENT_LIST_DIR}/CMakeBase.cmake)
set(MODULE_DIR modules)

function(addModule)
    addProject(LIB SHARED)

    useLibraries(NAMES NativeSDR::core NativeSDR::graphics PRIVATE)

    installRuntime(DESTINATION ${MODULE_DIR})
    add_custom_target(${PROJECT_NAME}_run COMMAND NativeSDR::main -md ${CMAKE_CURRENT_BINARY_DIR})
endfunction()
