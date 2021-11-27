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
#include <imgui/implot.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/utilities/imgui_math.h>
#include <functional>

#ifdef IM_INTERNAL

#include <imgui/imgui_internal.h>
#include <imgui/implot_internal.h>

#endif

class NATIVESDR_GRAPHICS_EXPORT NativeGraphics {

public:

    static int setupGraphics();

    template<class func, class... args>
    static void drawScreen(func&& f, args&& ... a) {
        startRender();
        f(a...);
        endRender();
    }

    template<class func, class... args>
    static void startMainWindow(func&& f, args&& ... a) {
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

    NATIVESDR_GRAPHICS_EXPORT void UpdateTexture(ImTextureID texture, const void* data, int width, int height);

    NATIVESDR_GRAPHICS_EXPORT ImTextureID CreateTexture(const void* data, int width, int height);

    NATIVESDR_GRAPHICS_EXPORT ImTextureID LoadTexture(const char* path);

    NATIVESDR_GRAPHICS_EXPORT void DeleteTexture(ImTextureID& texture);

    NATIVESDR_GRAPHICS_EXPORT int GetTextureWidth(const ImTextureID& texture);

    NATIVESDR_GRAPHICS_EXPORT int GetTextureHeight(const ImTextureID& texture);

    NATIVESDR_GRAPHICS_EXPORT bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);

    NATIVESDR_GRAPHICS_EXPORT void FillBox(ImU32 col);

    inline void DebugGreenBox() {
        FillBox(IM_COL32(0, 255, 0, 128));
    }

#ifdef IM_INTERNAL

    static inline ImRect GetItemRect() {
        return {ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};
    }

#endif

    NATIVESDR_GRAPHICS_EXPORT bool ImageButtonWhite(const char* str_id, const ImVec2& size, const ImTextureID& texture, bool clickable);

    NATIVESDR_GRAPHICS_EXPORT ImFont* AddDefaultFont(float size);

}

namespace ImGui {

    NATIVESDR_GRAPHICS_EXPORT ImVec2 DrawChartFrame(ImVec2& start, ImVec2& end, double yStart, double yStop, const std::function<std::string(double)>& yLabel, double xStart, double xStop, const std::function<std::string(double)>& xLabel);

    NATIVESDR_GRAPHICS_EXPORT void DrawChartLine(const ImVec2& start, const ImVec2& end, const float* points, int pointCount, const ImVec2& partsPerUnit, double yStart, ImU32 col, bool inverted = false);

    NATIVESDR_GRAPHICS_EXPORT void DrawChartLineFilled(const ImVec2& start, const ImVec2& end, const float* points, int pointCount, const ImVec2& partsPerUnit, double yStart, ImU32 col, ImU32 colInside, bool inverted = false);

}

namespace ImPlot {

    void DrawImPlotChart(size_t id, const float* points, int pointCount, double yMin, double yMax, std::function<void(double, char*, int)>& xTags);

}

#ifdef IM_INTERNAL

static inline ImRect expandImRect(const ImRect& rect, float x, float y) {
    auto ret = rect;
    ret.Min.x -= x;
    ret.Min.y -= y;
    ret.Max.x += x;
    ret.Max.y += y;
    return ret;
}

#endif

static inline bool isInArea(ImVec2 mouse, ImVec2 min, ImVec2 max) {
    return mouse.x >= min.x && mouse.x < max.x && mouse.y >= min.y && mouse.y < max.y;
}

#endif //NATIVESDR_NATIVESDRGRAPHICS_H
