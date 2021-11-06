//
// Created by Elec332 on 06/11/2021.
//

#ifndef NATIVESDR_BLOCK_CONNECTION_H
#define NATIVESDR_BLOCK_CONNECTION_H

#include <pipeline/block/block.h>

class schematic_link_handler;

class block_connection : public pipeline::block_connection_base_instance {

public:

    virtual void initOutput(schematic_link_handler* lh, ax::NodeEditor::PinId pin) = 0;

    virtual void setObject(const block_connection* other) = 0;

};

#include <impl/pipeline/schematic_links.h>

#endif //NATIVESDR_BLOCK_CONNECTION_H
