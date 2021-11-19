//
// Created by Elec332 on 04/11/2021.
//

#include <dsp/iq_converter.h>
#include <volk/volk.h>
#include <iostream>
#include <dsp/fft.h>
#include "ui/sdr_ui.h"

bool showMenu = true;
float size = 0.2;

void sdr_ui::init() {
}

void sdr_ui::deinit() {
}

class UIBlock : public pipeline::block {

public:

    UIBlock() : pipeline::block("UI Sink", ImColor(0, 0, 255)) {
        addInput("Renderer", utils::uiType(), uiFunc, [](int flags) {
            std::cout << "Change " << flags << std::endl;
        });
    }

    void start() override {
    }

    void stop() override {
    }

    void drawMiddle() override {
    }

    virtual void draw(size_t rand) {
        if (uiFunc) {
            (*uiFunc)(rand);
        }
    }

private:

    utils::drawFunc* uiFunc = nullptr;

};

pipeline::block_ptr sdr_ui::createUIBlock() {
    return std::make_shared<UIBlock>();
}

void sdr_ui::draw(pipeline::schematic* nodes) {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));
    ImVec2 totalSize = ImGui::GetWindowSize();
    auto leftPaneWidth = totalSize.x * size;
    auto rightPaneWidth = totalSize.x * (1.0f - size);

    if (showMenu) {
        float splitterWidth = 4.0f;
        ImGui::Splitter(true, splitterWidth, &leftPaneWidth, &rightPaneWidth, 0, 50);
        ImGui::BeginChild("SubMenu", ImVec2(leftPaneWidth, totalSize.y));
        nodes->forEachBlock([&](const pipeline::block_data& w) {
            pipeline::block* block = w->getBlock();
            if (block->hasMenu() && ImGui::CollapsingHeader(w->getType().c_str())) {
                block->drawMenu();
            }
        });
        if (ImGui::CollapsingHeader("Debug")) {
            ImGui::Text("FPS: %.3f", ImGui::GetIO().Framerate);
        }
        ImGui::EndChild();
        ImGui::SameLine();
        size = std::min(leftPaneWidth / totalSize.x, 0.5f);
        rightPaneWidth -= splitterWidth;
    } else {
        rightPaneWidth = totalSize.x;
    }
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));
    ImGui::BeginChild("Main", ImVec2(rightPaneWidth - 8, totalSize.y));
//    ImVec2 smSize = ImGui::GetWindowSize();
//    std::cout << smSize.x << " Main " << smSize.y << std::endl;
    size_t rand = 0;
    nodes->forEachBlock("UI", [&](const pipeline::block_data& block) {
        ((UIBlock*) block->getBlock())->draw(rand);
        rand += 1024;
    });
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
}

