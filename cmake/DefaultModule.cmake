include(${ROOT_INCLUDE})
addProject(LIB SHARED)

useLibrary(NativeSDR_core)
useLibrary(NativeSDR_graphics)

installRuntime(modules)
