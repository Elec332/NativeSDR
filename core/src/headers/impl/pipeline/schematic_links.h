//
// Created by Elec332 on 05/11/2021.
//

#ifndef NATIVESDR_SCHEMATIC_LINKS_H
#define NATIVESDR_SCHEMATIC_LINKS_H

class schematic_link_handler {

public:

    virtual bool
    doConnect(ax::NodeEditor::PinId pinA, wrapped_block& blockA, ax::NodeEditor::PinId pinB, wrapped_block& blockB) = 0;

    virtual bool deleteLink(ax::NodeEditor::LinkId id) = 0;

    virtual void deleteBlockLinks(const wrapped_block& block) = 0;

    virtual bool hasConnection(const wrapped_block& block, const pipeline::block_connection_base& blockConnection) = 0;

    virtual void forEachLink(const std::function<void(const pipeline::link&)>& func) = 0;

};

std::shared_ptr<schematic_link_handler> newLinkHandler(pipeline::schematic* schematic);

#endif //NATIVESDR_SCHEMATIC_LINKS_H
