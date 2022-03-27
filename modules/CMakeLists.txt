add_subdirectory(audio-sink)
add_subdirectory(file-source)
add_subdirectory(hackrf)
add_subdirectory(uhd_loader)
add_subdirectory(uhd)

#TODO: Find libs for all compilers
if(NOT WIN32 OR MSVC)
    add_subdirectory(rtl-sdr) #Not on MSYS2
endif()