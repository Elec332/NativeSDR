//
// Created by Elec332 on 04/11/2021.
//

#ifndef NATIVESDR_NODE_MANAGER_H
#define NATIVESDR_NODE_MANAGER_H

#include <nativesdr/pipeline/block/block.h>
#include <nativesdr/core_export.h>

namespace pipeline {

    typedef std::function<pipeline::block_ptr()> block_factory;

    class node_manager {

    public:

        virtual void registerBlockType(std::string name, pipeline::block_factory factory) = 0;

        virtual const pipeline::block_factory& getFactory(std::string name) = 0;

        virtual void forEachFactory(const std::function<void(const std::string&)>& func) = 0;

    };

}

CORE_EXPORT pipeline::node_manager* newNodeManager();

CORE_EXPORT void deleteNodeManager(pipeline::node_manager* nodeManager);

#endif //NATIVESDR_NODE_MANAGER_H
