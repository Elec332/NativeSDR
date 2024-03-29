//
// Created by Elec332 on 05/11/2021.
//

#ifndef NATIVESDR_SCHEMATIC_H
#define NATIVESDR_SCHEMATIC_H

#include <filesystem>
#include <nativesdr/pipeline/block/node_manager.h>
#include <nativesdr/pipeline/block/block_data.h>
#include <nativesdr/core_export.h>

namespace pipeline {

    struct link_instance;

    typedef std::shared_ptr<pipeline::link_instance> link;

    class schematic {

    public:

        virtual void start() = 0;

        virtual void stop() = 0;

        virtual bool isStarted() = 0;

        virtual void save() = 0;

        virtual void save(const std::filesystem::path& path) = 0;

        virtual bool isBroken() = 0;

        virtual void ignoreBroken() = 0;

        virtual bool isLocked() = 0;

        virtual void forEachBlock(const std::function<void(const pipeline::block_data&)>& func) = 0;

        virtual void forEachBlock(const std::string& type, const std::function<void(const pipeline::block_data&)>& func) = 0;

        virtual void forEachLink(const std::function<void(const pipeline::link&)>& func) = 0;

        virtual void forEachFactory(const std::function<void(const std::string&)>& func) = 0;

        virtual bool addBlock(std::string name, float x, float y) = 0;

        virtual bool canConnect(ax::NodeEditor::PinId& a, ax::NodeEditor::PinId& b) = 0;

        virtual bool connect(ax::NodeEditor::PinId a, ax::NodeEditor::PinId b) = 0;

        virtual bool deleteBlock(ax::NodeEditor::NodeId id) = 0;

        virtual bool deleteLink(ax::NodeEditor::LinkId id) = 0;

        virtual pipeline::block_data getBlock(ax::NodeEditor::NodeId id) = 0;

#ifdef __IMGUI_NODE_EDITOR_H__

        [[nodiscard]] virtual ax::NodeEditor::EditorContext* getEditor() const = 0;

#endif

    };

    CORE_EXPORT pipeline::schematic* newSchematic(pipeline::node_manager* nodeManager, const std::filesystem::path& file);

    CORE_EXPORT void deleteSchematic(pipeline::schematic* schematic);

    struct link_instance {

        link_instance(ax::NodeEditor::LinkId id, ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId, ImColor color = ImColor(255, 255, 255)) :
                id(id), startPin(startPinId), endPin(endPinId), color(color.Value) {
        }

        const ax::NodeEditor::LinkId id;
        const ax::NodeEditor::PinId startPin;
        const ax::NodeEditor::PinId endPin;
        ImColor color;
    };

}

#endif //NATIVESDR_SCHEMATIC_H
