//
// Created by Elec332 on 04/11/2021.
//

#ifndef NATIVESDR_NODE_MANAGER_H
#define NATIVESDR_NODE_MANAGER_H

#include <pipeline/block/block.h>

namespace pipeline {

    typedef pipeline::block_ptr(* block_factory)();

    class node_manager {

    public:

        virtual void registerBlockType(std::string name, pipeline::block_factory factory) = 0;

        virtual pipeline::block_factory getFactory(std::string name) = 0;

        virtual void forEachFactory(const std::function<void(const std::string&)>& func) = 0;

    };

}

pipeline::node_manager* newNodeManager();

void deleteNodeManager(pipeline::node_manager* nodeManager);

#endif //NATIVESDR_NODE_MANAGER_H
