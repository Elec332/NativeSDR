cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
project(NativeSDR_core)

include(${CMAKE_INCLUDE})
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /)

target_link_libraries(NativeSDR_core PRIVATE ModuleManager)
target_include_directories(${PROJECT_NAME} PUBLIC ../module_manager/src/public) # Y u make me do dis :/
target_link_libraries(NativeSDR_core PRIVATE NativeSDR_graphics)

addLibrary(fftw libfftw3f-3)
addLibrary(volk volk)
addLibraryRoot(json)

#########################

option(ENABLE_UNIT_TESTS "Enable unit tests" OFF)
message(STATUS "Enable testing: ${ENABLE_UNIT_TESTS}")
if (ENABLE_UNIT_TESTS)
    set(TEST_P_NAME ${PROJECT_NAME}_test)

    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        ${GOOGLETEST_VERSION}
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(BUILD_GTEST ON CACHE BOOL "" FORCE)
    option(INSTALL_GMOCK OFF)
    option(INSTALL_GTEST OFF)
    FetchContent_MakeAvailable(googletest)

    file(GLOB_RECURSE TEST_SRC test/*.cpp test/*.c test/*.hpp test/*.h)

    add_executable(${TEST_P_NAME}
            ${TEST_SRC}
            )

    target_link_libraries(${TEST_P_NAME}
            PRIVATE
            ${PROJECT_NAME}
            gtest_main
            )

    enable_testing()
    add_test(
            NAME google_test
            COMMAND $<TARGET_FILE:${TEST_P_NAME}>
    )

endif()