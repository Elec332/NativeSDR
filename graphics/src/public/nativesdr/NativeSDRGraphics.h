//
// Created by Elec332 on 09/07/2021.
//

#ifndef NATIVESDR_NATIVESDRGRAPHICS_H
#define NATIVESDR_NATIVESDRGRAPHICS_H

#define IMGUI_DISABLE_INCLUDE_IMCONFIG_H
#define CUSTOM_IMGUIFILEDIALOG_CONFIG <nativesdr/graphics_export.h>

#include <nativesdr/graphics_export.h>
#include <nativesdr/graphics/imgui.h>
#include <nativesdr/graphics/imgui_stdlib.h>
#include <nativesdr/graphics/imgui_node_editor.h>
#include <nativesdr/graphics/builders.h>
#include <nativesdr/graphics/drawing.h>
#include <nativesdr/graphics/widgets.h>
#include <nativesdr/graphics/implot.h>
#include <nativesdr/imgui/imgui_math.h>
#include <functional>
#include <nativesdr/graphics/ImGuiFileDialog.h>
#include <nativesdr/NativeSDRGraphicsInfo.h>

#ifdef IM_INTERNAL

#include <nativesdr/graphics/imgui_internal.h>

#endif

class GRAPHICS_EXPORT NativeGraphics : public NativeGraphicsInfo {

public:

    int setupGraphics();

    template<class func, class... args>
    void drawScreen(func&& f, args&& ... a) {
        startRender();
        f(a...);
        endRender();
    }

    template<class func, class... args>
    void startMainWindow(func&& f, args&& ... a) {
        while (!shouldCloseWindow()) {
            drawScreen(f, a...);
        }
    }

    std::shared_ptr<SubContext> createChildContext();

    void closeWindow();

    void destroy();

    ImVec4* getClearColor();

    [[nodiscard]] std::string getAPI() const override;

    [[nodiscard]] int getMajorVersion() const override;

    [[nodiscard]] int getMinorVersion() const override;

    [[nodiscard]] const std::set<std::string> getExtensions() const override;

    [[nodiscard]] bool hasExtension(const std::string& name) const override;

    [[nodiscard]] const std::set<std::string> getShaderVersions() const override;

    [[nodiscard]] bool hasShaderVersion(const std::string& version) const override;

private:

    static bool shouldCloseWindow();

    static void startRender();

    static void endRender();

};

namespace ImGui {

    GRAPHICS_EXPORT void FocusCurrentWindow();

    GRAPHICS_EXPORT void UpdateTexture(ImTextureID texture, const void* data, int width, int height);

    GRAPHICS_EXPORT ImTextureID CreateTexture(const void* data, int width, int height);

    GRAPHICS_EXPORT ImTextureID LoadTexture(const char* path);

    GRAPHICS_EXPORT void DeleteTexture(ImTextureID& texture);

    GRAPHICS_EXPORT int GetTextureWidth(const ImTextureID& texture);

    GRAPHICS_EXPORT int GetTextureHeight(const ImTextureID& texture);

    GRAPHICS_EXPORT bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);

    GRAPHICS_EXPORT void FillBox(ImU32 col);

    inline void DebugGreenBox() {
        FillBox(IM_COL32(0, 255, 0, 128));
    }

    GRAPHICS_EXPORT const char* GetRendererName();

    GRAPHICS_EXPORT bool Combo(const char* label, int* current_item, const std::vector<std::string>& options);

#ifdef IM_INTERNAL

    static inline ImRect GetItemRect() {
        return {ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};
    }

#endif

    GRAPHICS_EXPORT bool ImageButtonWhite(const char* str_id, const ImVec2& size, const ImTextureID& texture, bool clickable);

    GRAPHICS_EXPORT ImFont* AddDefaultFont(float size);

    template<class T>
    class ImComboList {

    public:

        void registerItem(T t, std::string name) {
            names += name;
            names += '\0';
            list.emplace_back(std::move(t));
        }

        [[nodiscard]] const std::string& getAllNames() const {
            return names;
        }

        T* getSelectedItemReference() {
            return &list[index];
        }

        T getSelectedItem() {
            return list[index];
        }

        int index = 0;

    private:
        std::vector<T> list;
        std::string names;

    };

    template<class T>
    inline bool ComboList(const char* label, ImComboList<T>* data, int popup_max_height_in_items = -1) {
        return ImGui::Combo(label, &data->index, data->getAllNames().data(), popup_max_height_in_items);
    }

}

namespace ImGui {

    GRAPHICS_EXPORT ImVec2 DrawChartFrame(ImVec2& start, ImVec2& end, double yStart, double yStop, const std::function<std::string(double)>& yLabel, double xStart, double xStop, const std::function<std::string(double)>& xLabel);

    GRAPHICS_EXPORT void DrawChartLine(const ImVec2& start, const ImVec2& end, const float* points, int pointCount, const ImVec2& partsPerUnit, double yStart, ImU32 col, bool inverted = false);

    GRAPHICS_EXPORT void DrawChartLineFilled(const ImVec2& start, const ImVec2& end, const float* points, int pointCount, const ImVec2& partsPerUnit, double yStart, ImU32 col, ImU32 colInside, bool inverted = false);

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

typedef std::function<void(void* data, size_t size)> fileWriter;

GRAPHICS_EXPORT void writePNG(const fileWriter& wf, void* data, int width, int height, int channels, int size = 1);

#endif //NATIVESDR_NATIVESDRGRAPHICS_H
