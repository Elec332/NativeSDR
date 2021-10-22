//
// Created by Elec332 on 09/07/2021.
//

#ifndef NATIVESDR_NATIVESDRGRAPHICS_H
#define NATIVESDR_NATIVESDRGRAPHICS_H

#include <nativesdr_graphics_export.h>
#include <imgui/imgui.h>
#include <imgui/imgui_node_editor.h>
#include <imgui/utilities/builders.h>
#include <imgui/utilities/widgets.h>

#ifdef IM_INTERNAL
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui_internal.h>
#endif

class NATIVESDR_GRAPHICS_EXPORT NativeGraphics {

public:

    static int setupGraphics();

    template <class func, class... args>
    static void drawScreen(func&& f, args&&... a) {
        startRender();
        f(a...);
        endRender();
    }

    template <class func, class... args>
    static void startMainWindow(func&& f, args&&... a) {
        while (!shouldCloseWindow()) {
            drawScreen(f, a...);
        }
    }

    static void destroy();

    static ImVec4* getClearColor();

private:

    static bool shouldCloseWindow();

    static void startRender();

    static void endRender();

};

namespace ImGui {

    NATIVESDR_GRAPHICS_EXPORT void FocusCurrentWindow();

    NATIVESDR_GRAPHICS_EXPORT ImTextureID CreateTexture(const void* data, int width, int height);

    NATIVESDR_GRAPHICS_EXPORT ImTextureID LoadTexture(const char* path);

    NATIVESDR_GRAPHICS_EXPORT void DeleteTexture(ImTextureID& texture);

    NATIVESDR_GRAPHICS_EXPORT int GetTextureWidth(const ImTextureID& texture);

    NATIVESDR_GRAPHICS_EXPORT int GetTextureHeight(const ImTextureID& texture);

    NATIVESDR_GRAPHICS_EXPORT bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);

#ifdef IM_INTERNAL
    static inline ImRect GetItemRect(){
        return {ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};
    }
#endif

    NATIVESDR_GRAPHICS_EXPORT bool ImageButtonWhite(const char* str_id, const ImVec2& size, const ImTextureID& texture, bool clickable);

}

#ifdef IM_INTERNAL
static inline ImRect expandImRect(const ImRect& rect, float x, float y){
    auto ret = rect;
    ret.Min.x -= x;
    ret.Min.y -= y;
    ret.Max.x += x;
    ret.Max.y += y;
    return ret;
}
#endif

#endif //NATIVESDR_NATIVESDRGRAPHICS_H
