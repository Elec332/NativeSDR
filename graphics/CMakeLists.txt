cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
project(graphics)

addProject(LIB SHARED)
installRuntime(DEV)

add_compile_definitions(IMGUI_USER_CONFIG=<nativesdr/graphics_export.h>)
add_compile_definitions(IMGUI_API=GRAPHICS_EXPORT)
add_compile_definitions(IMPLOT_API=GRAPHICS_EXPORT)
add_compile_definitions(IMGUI_IMPL_API=)
add_compile_definitions(IMGUI_DISABLE_INCLUDE_IMCONFIG_H=)
add_compile_definitions(USE_STD_FILESYSTEM=)
add_compile_definitions(CUSTOM_IMGUIFILEDIALOG_CONFIG=<nativesdr/graphics_export.h>)

findLibrary(GLEW MODULE)
#findLibrary(GLFW3 ALT glfw3 NAME glfw)
findLibrary(glfw3 NAME glfw)
useLibraries(NAMES glfw PRIVATE WIN_INSTALL /)
useLibraries(NAMES GLEW::glew PUBLIC WIN_INSTALL / HEADER_INCLUDE_FILES gl/glew.h)

getRemoteLibrary(imgui GIT_REPO https://github.com/thedmd/imgui.git GIT_TAG feature/layout-external NO_BUILD)
getRemoteLibrary(imgui_node_editor GIT_REPO https://github.com/thedmd/imgui-node-editor.git GIT_TAG develop NO_BUILD)
getRemoteLibrary(implot GIT_REPO https://github.com/epezent/implot.git GIT_TAG v0.13 NO_BUILD)
getRemoteLibrary(stb GIT_REPO https://github.com/nothings/stb.git NO_BUILD)
getRemoteLibrary(file_dialog GIT_REPO https://github.com/aiekick/ImGuiFileDialog.git NO_BUILD)

useRemoteSource(stb stb
        PUBLIC_HEADER_DIR nativesdr/stb
        PUBLIC_HEADER_FILES stb_image.h
        )
useRemoteSource(implot implot
        PUBLIC_HEADER_DIR nativesdr/graphics
        PUBLIC_HEADER_FILES implot.h
        SOURCE_FILES *.cpp implot_internal.h
        )
useRemoteSource(imgui imgui
        PUBLIC_HEADER_DIR nativesdr/graphics
        PUBLIC_HEADER_FILES imgui.h imgui_internal.h imstb_textedit.h imgui_stacklayout.h imgui_stacklayout_internal.h
        SOURCE_FILES *.cpp *.h backends/*opengl3*.* backends/*glfw.*
        )
useRemoteSource(imgui imgui_stdlib
        ROOT misc/cpp
        PUBLIC_HEADER_DIR nativesdr/graphics
        PUBLIC_HEADER_FILES *.h
        SOURCE_FILES *.cpp
        )
useRemoteSource(file_dialog file_dialog
        ROOT ImGuiFileDialog
        PUBLIC_HEADER_DIR nativesdr/graphics
        PUBLIC_HEADER_FILES *log.h
        SOURCE_FILES *.cpp
        REPLACE "prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_DisableBookmarkMode|true" "class FileDialog|class GRAPHICS_EXPORT FileDialog"
        )
useRemoteSource(imgui_node_editor imgui_nodes_blueprints
        ROOT examples/blueprints-example/utilities
        PUBLIC_HEADER_DIR nativesdr/graphics
        PUBLIC_HEADER_FILES *.h
        SOURCE_FILES *.cpp
        HEADER_REPLACE "void|IMGUI_API void" "BlueprintNodeBuilder(|IMGUI_API BlueprintNodeBuilder("
        )
useRemoteSource(imgui_node_editor imgui_node_editor
        PUBLIC_HEADER_DIR nativesdr/graphics
        PUBLIC_HEADER_FILES imgui_node_editor.h
        SOURCE_FILES *.cpp *.h *.inl
        HEADER_REPLACE "void|IMGUI_API void" "IMGUI_API void*|void*"
        "bool Begin|IMGUI_API bool Begin" "bool End|IMGUI_API bool End" "bool Query|IMGUI_API bool Query" "bool Accept|IMGUI_API bool Accept"
        "bool Delete|IMGUI_API bool Delete" "bool Link|IMGUI_API bool Link" "bool Show|IMGUI_API bool Show" "bool Is|IMGUI_API bool Is" "bool Has|IMGUI_API bool Has"
        "EditorContext* Get|IMGUI_API EditorContext* Get" "EditorContext* Create|IMGUI_API EditorContext* Create" "int Get|IMGUI_API int Get"
        "ImVec2|IMGUI_API ImVec2" "IMGUI_API ImVec2  |ImVec2  " "IMGUI_API ImVec2(|ImVec2(" "IMGUI_API ImVec2&|ImVec2&"
        )

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC opengl32)
    target_link_libraries(${PROJECT_NAME} PRIVATE imm32)
endif()