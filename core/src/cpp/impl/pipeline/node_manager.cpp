//
// Created by Elec332 on 04/11/2021.
//
#include <nativesdr/pipeline/block/node_manager.h>
#include <map>

class node_manager_impl : public pipeline::node_manager {

public:

    void registerSourceBlockType(std::string name, pipeline::source_block_factory factory) override {
        std::cout << "Registered source!" << std::endl;
        registerBlockType(name, (pipeline::block_factory) factory);
    }

    void registerBlockType(std::string name, pipeline::block_factory factory) override {
        factories[name] = factory;
    }

    const pipeline::block_factory& getFactory(std::string name) override {
        return factories[name];
    }

    void forEachFactory(const std::function<void(const std::string&)>& func) override {
        for (auto& f : factories) {
            func(f.first);
        }
    }

private:

    std::map<std::string, pipeline::block_factory> factories;

};

pipeline::node_manager* newNodeManager() {
    return new node_manager_impl();
}

void deleteNodeManager(pipeline::node_manager* ptr) {
    if (ptr) {
        delete (node_manager_impl*) ptr;
    }
}
