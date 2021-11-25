//
// Created by Elec332 on 09/07/2021.
//

#define IM_INTERNAL

#include "NativeSDRGraphics.h"

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

//Todo: Linux
#pragma comment(lib, "opengl32.lib")

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GLFWwindow* window_GLFW;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
ImFont* defaultFont;

int NativeGraphics::setupGraphics() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return 1;
    }

    // Decide GL+GLSL versions
#if defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.3 + GLSL 330
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    window_GLFW = glfwCreateWindow(1280, 720, "NativeSDR", nullptr, nullptr);
    if (window_GLFW == nullptr) {
        return 1;
    }
    glfwMakeContextCurrent(window_GLFW);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImPlot::SetImGuiContext(ImGui::GetCurrentContext());
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    defaultFont = io.Fonts->AddFontDefault();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window_GLFW, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return 0;
}

bool NativeGraphics::shouldCloseWindow() {
    return glfwWindowShouldClose(window_GLFW);
}

void NativeGraphics::startRender() {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::PushFont(defaultFont);
}

void NativeGraphics::endRender() {
    ImGui::PopFont();
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window_GLFW, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window_GLFW);
}

void NativeGraphics::destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window_GLFW);
    glfwTerminate();
}

ImVec4* NativeGraphics::getClearColor() {
    return &clear_color;
}

void ImGui::FocusCurrentWindow() {
    ImGui::FocusWindow(ImGui::GetCurrentWindow());
}

//

#define STB_IMAGE_IMPLEMENTATION
extern "C" {
#include "stb/stb_image.h"
}

struct TexInfo {
    GLuint texID = 0;
    int width = 0;
    int height = 0;
};

static std::vector<TexInfo> g_Textures;

static std::vector<TexInfo>::iterator findTexture(ImTextureID texture) {
    auto textureID = static_cast<GLuint>(reinterpret_cast<std::intptr_t>(texture));
    return std::find_if(g_Textures.begin(), g_Textures.end(), [textureID](TexInfo& texture) {
        return texture.texID == textureID;
    });
}

void UpdateTexture(TexInfo& texture, const void* data, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, texture.texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    texture.width = width;
    texture.height = height;
}

void ImGui::UpdateTexture(ImTextureID texture, const void* data, int width, int height) {
    auto textureIt = findTexture(texture);
    if (textureIt != g_Textures.end()) {
        UpdateTexture(&textureIt, data, width, height);
    }
}

ImTextureID ImGui::CreateTexture(const void* data, int width, int height) {
    g_Textures.resize(g_Textures.size() + 1);
    TexInfo& texture = g_Textures.back();

    // Upload texture to graphics system
    GLint last_texture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &texture.texID);
    UpdateTexture(texture, data, width, height);

    glBindTexture(GL_TEXTURE_2D, last_texture);
    return reinterpret_cast<ImTextureID>(static_cast<std::intptr_t>(texture.texID));
}

ImTextureID ImGui::LoadTexture(const char* path) {
    int width = 0, height = 0, component = 0;
    if (auto data = stbi_load(path, &width, &height, &component, 4)) {
        auto texture = CreateTexture(data, width, height);
        stbi_image_free(data);
        return texture;
    } else {
        return nullptr;
    }
}

void ImGui::DeleteTexture(ImTextureID& texture) {
    auto textureIt = findTexture(texture);
    if (textureIt == g_Textures.end()) {
        return;
    }
    glDeleteTextures(1, &textureIt->texID);
    g_Textures.erase(textureIt);
    texture = nullptr;
}

int ImGui::GetTextureWidth(const ImTextureID& texture) {
    auto textureIt = findTexture(texture);
    if (textureIt != g_Textures.end()) {
        return textureIt->width;
    }
    return 0;
}

int ImGui::GetTextureHeight(const ImTextureID& texture) {
    auto textureIt = findTexture(texture);
    if (textureIt != g_Textures.end()) {
        return textureIt->height;
    }
    return 0;
}

bool ImGui::Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size) {
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID("##Splitter");
    ImRect bb;
    bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
    bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
    return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}

bool ImGui::ImageButtonWhite(const char* str_id, const ImVec2& size, const ImTextureID& texture, bool clickable) {
    auto drawList = ImGui::GetWindowDrawList();
    bool ret = false;
    ImU32 color;
    if (clickable) {
        if (ImGui::InvisibleButton(str_id, size)) {
            ret = true;
        }
        if (ImGui::IsItemActive()) {
            color = IM_COL32(255, 255, 255, 96);
        } else if (ImGui::IsItemHovered()) {
            color = IM_COL32(255, 255, 255, 255);
        } else {
            color = IM_COL32(255, 255, 255, 160);
        }
    } else {
        ImGui::Dummy(size);
        color = IM_COL32(255, 255, 255, 32);
    }
    drawList->AddImage(texture, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), color);
    return ret;
}

ImFont* ImGui::AddDefaultFont(float size) {
    ImFontConfig config;
    config.SizePixels = size;
    config.OversampleH = config.OversampleV = 1;
    config.PixelSnapH = true;
    return ImGui::GetIO().Fonts->AddFontDefault(&config);
}

void ImGui::FillBox(ImU32 col) {
    ImVec2 start = ImGui::GetWindowPos();
    ImGui::GetWindowDrawList()->AddRectFilled(start, start + ImGui::GetWindowSize(), col);
}

static void formatTag(double value, char* buf, int len, void* data) {
    auto tagger = (std::function<void(double, char*, int)>*) data;
    (*tagger)(value, buf, len);
}

void ImPlot::DrawImPlotChart(size_t id, const float* points, int pointCount, double yMin, double yMax, std::function<void(double, char*, int)>& xTags) {
    std::string str = "##" + std::to_string(id);

    ImPlot::SetNextAxisToFit(ImAxis_X1);
    if (ImPlot::BeginPlot(str.c_str(), ImVec2(-1, 0), ImPlotFlags_NoInputs)) {
        ImPlot::SetupAxisLimits(ImAxis_Y1, -100, 0);
        if (xTags != nullptr) {
            ImPlot::SetupAxisFormat(ImAxis_X1, formatTag, &xTags);
        }
        ImPlot::SetupFinish();
        ImPlot::PlotLine(str.c_str(), points, pointCount);
        ImPlot::EndPlot();
    }
}
