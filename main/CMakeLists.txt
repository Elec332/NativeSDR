cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(main)

addProject(EXECUTABLE)
installRuntime(DEV DESTINATION ${CMAKE_INSTALL_PREFIX})
setCentralOutput()

setRunPath(.:./lib)

useLibraries(NativeSDR::core PRIVATE)
