//
// Created by Elec332 on 21/10/2021.
//

#include "drawtest.h"

namespace ne = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

static ne::EditorContext* ctx = nullptr;

void init() {
    ctx = ne::CreateEditor();
}

void deinit() {
    ne::DestroyEditor(ctx);
}

void draw(ImGuiWindow* window) {
    ne::SetCurrentEditor(ctx);
    ImVec2 winSize = ImGui::GetWindowSize();
    ImVec2 mousePos = ImGui::GetMousePos();

    ImGui::Columns(2, "WindowColumns", false);
    ImGui::SetColumnWidth(0, winSize.x / 2);
    ImGui::SetColumnWidth(1, winSize.x / 2);


    ImGui::BeginChild("Left");
    ne::Begin("Window Test");
    ne::End();
    ImGui::EndChild();

    ImGui::NextColumn();

    double test[] = {1, 2, 3, 2, 3, 2, 3, 5.6, 7, 2, 4, 1};

    ImGui::BeginChild("lines");
    ImPlot::BeginPlot("", ImVec2(-1,0), ImPlotFlags_NoInputs);
    ImPlot::SetupAxesLimits(0, 20, 0, 20);
    ImPlot::PlotLine("", test, 12);
    ImPlot::EndPlot();
    ImGui::EndChild();
}
