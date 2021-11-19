//
// Created by Elec332 on 04/11/2021.
//

#include "impl/pipeline/wrapped_block.h"

#include <utility>

class wbii : public wrapped_block_instance {

public:

    wbii(pipeline::block_factory factory, size_t id, std::string type) : block(factory()), ID(id),
                                                                         type(std::move(type)) {
    }

    [[nodiscard]] pipeline::block* getBlock() const override {
        return block.get();
    }

    [[nodiscard]] nlohmann::json toJson() const override {
        nlohmann::json ret;
        ret["type"] = type;
        ret["id"] = ID;
        ret["x"] = x;
        ret["y"] = y;
        return ret;
    }

    [[nodiscard]] ax::NodeEditor::NodeId getId() const override {
        return ID;
    }

    [[nodiscard]] const std::string& getType() const override {
        return type;
    }

    [[nodiscard]] pipeline::block_connection_base
    getPin(ax::NodeEditor::PinId pin, const pipeline::block::connection_list& pins) const {
        auto p = (size_t) pin;
        auto sid = p % 32;
        if (ID != (p - sid)) {
            return nullptr;
        }
        for (const auto& c: pins) {
            if (c->getId() == sid) {
                return c;
            }
        }
        return nullptr;
    }

    [[nodiscard]] pipeline::block_connection_base getInputPin(ax::NodeEditor::PinId pin) const override {
        return getPin(pin, block->getInputs());
    }

    [[nodiscard]] pipeline::block_connection_base getOutputPin(ax::NodeEditor::PinId pin) const override {
        return getPin(pin, block->getOutputs());
    }

private:

    const size_t ID;
    const std::string type;
    const pipeline::block_ptr block;

};

wrapped_block fromFactory(pipeline::block_factory factory, size_t id, const std::string& type) {
    return std::make_shared<wbii>(factory, id, type);
}

wrapped_block fromJson(const nlohmann::json& json, pipeline::node_manager* nodeManager) {
    std::string type = json["type"].get<std::string>();
    pipeline::block_factory factory = nodeManager->getFactory(type);
    if (factory == nullptr) {
        return nullptr;
    }
    wrapped_block ret = std::make_shared<wbii>(factory, json["id"].get<size_t>(), type);
    ret->x = json["x"].get<float>();
    ret->y = json["y"].get<float>();
    return ret;
}
