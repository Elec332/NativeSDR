//
// Created by Elec332 on 05/11/2021.
//

#ifndef NATIVESDR_SCHEMATIC_LINKS_H
#define NATIVESDR_SCHEMATIC_LINKS_H

#include <impl/pipeline/wrapped_block.h>
#include <impl/pipeline/block_connection.h>

class schematic_link_handler {

public:

    virtual void save(nlohmann::json& json) const = 0;

    virtual void load(const nlohmann::json& json) = 0;

    virtual bool doConnect(ax::NodeEditor::PinId pinA, wrapped_block_instance* blockA, ax::NodeEditor::PinId pinB, wrapped_block_instance* blockB) = 0;

    virtual bool deleteLink(ax::NodeEditor::LinkId id) = 0;

    virtual void deleteBlockLinks(const wrapped_block& block) = 0;

    virtual bool hasConnection(const wrapped_block& block, const pipeline::block_connection_base& blockConnection) = 0;

    virtual void forEachLink(const std::function<void(const pipeline::link&)>& func) = 0;

    virtual void onLinkValueChanged(ax::NodeEditor::PinId pin, const block_connection* newRef, int flags) = 0;

};

std::shared_ptr<schematic_link_handler> newLinkHandler(pipeline::schematic* schematic);

#endif //NATIVESDR_SCHEMATIC_LINKS_H
