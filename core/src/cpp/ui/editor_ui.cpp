//
// Created by Elec332 on 04/11/2021.
//

#define IM_INTERNAL

#include <ui/editor_ui.h>
#include <iostream>

namespace ne = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

static ImTextureID background = nullptr;

void editor_ui::init() {
    background = ImGui::LoadTexture("assets/BlueprintBackground.png");
}

void editor_ui::deinit() {
    ImGui::DeleteTexture(background);
}

void editor_ui::draw(pipeline::schematic* nodes) {
    ne::SetCurrentEditor(nodes->getEditor());

    ne::Begin("Editor");
    util::BlueprintNodeBuilder builder(background, ImGui::GetTextureWidth(background),
                                       ImGui::GetTextureHeight(background));

    nodes->forEachBlock([&](const pipeline::block_data& block) {
        size_t id = block->getIdInt();
        builder.Begin(id);
        const pipeline::block* rb = block->getBlock();

        builder.Header(rb->getColor());
        ImGui::Spring(0);
        ImGui::TextUnformatted(rb->getName().c_str());
        ImGui::Spring(1);
        ImGui::Dummy(ImVec2(0, 28));
        ImGui::Spring(0);
        builder.EndHeader();

        for (auto& input: rb->getInputs()) {
            builder.Input(id + input->getId());
            input->getType()->drawIcon(false);
            ImGui::Spring(0);
            ImGui::TextUnformatted((input->getName() + " " + std::to_string(input->getId())).c_str());
            ImGui::Spring(0);
            builder.EndInput();
        }

        builder.Middle();
        block->getBlock()->drawMiddle();
        ImGui::Spring(1, 0);
        ImGui::TextUnformatted(("Hatseflats " + std::to_string(id)).c_str());
        ImGui::Spring(1, 0);

        for (auto& output: rb->getOutputs()) {
            builder.Output(id + output->getId());
            ImGui::Spring(0);
            ImGui::TextUnformatted((output->getName() + " " + std::to_string(output->getId())).c_str());
            ImGui::Spring(0);
            output->getType()->drawIcon(false);
            builder.EndOutput();
        }

        builder.End();
    });

    nodes->forEachLink([&](const pipeline::link& link) {
        ne::Link(link->id, link->startPin, link->endPin, link->color, 2.0f);
    });

    if (ne::BeginCreate(ImColor(255, 255, 255), 2.0f)) {
        auto showLabel = [](const char* label, ImColor color) {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
            auto size = ImGui::CalcTextSize(label);

            auto padding = ImGui::GetStyle().FramePadding;
            auto spacing = ImGui::GetStyle().ItemSpacing;

            ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

            auto rectMin = ImGui::GetCursorScreenPos() - padding;
            auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

            auto drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
            ImGui::TextUnformatted(label);
        };

        ne::PinId startPinId = 0, endPinId = 0;
        if (ne::QueryNewLink(&startPinId, &endPinId)) {
            if (startPinId && endPinId) {
                if (startPinId == endPinId) {
                    ne::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else if (!nodes->canConnect(startPinId, endPinId)) {
                    showLabel("Can't connect", ImColor(45, 32, 32, 180));
                    ne::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else {
                    showLabel("Create Link", ImColor(32, 45, 32, 180));
                    if (ne::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
                        nodes->connect(startPinId, endPinId);
                    }
                }
            }
        }
    }
    ne::EndCreate();

    if (ne::BeginDelete()) {
        ne::LinkId linkId = 0;
        while (ne::QueryDeletedLink(&linkId)) {
            if (ne::AcceptDeletedItem()) {
                nodes->deleteLink(linkId);
            }
        }

        ne::NodeId nodeId = 0;
        while (ne::QueryDeletedNode(&nodeId)) {
            if (ne::AcceptDeletedItem()) {
                nodes->deleteBlock(nodeId);
            }
        }
    }
    ne::EndDelete();

    ne::Suspend();
    auto pos = ImGui::GetMousePos();
    if (ne::ShowBackgroundContextMenu()) {
        ImGui::OpenPopup("Create New Node");
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("Create New Node")) {
        nodes->forEachFactory([&](const std::string& p) {
            if (ImGui::MenuItem(p.c_str())) {
                pos = ne::ScreenToCanvas(pos);
                nodes->addBlock(p, pos.x, pos.y);
            }
        });
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
    ne::Resume();

    ne::End();

}

