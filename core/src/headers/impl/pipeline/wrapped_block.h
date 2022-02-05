//
// Created by Elec332 on 04/11/2021.
//

#ifndef NATIVESDR_WRAPPED_BLOCK_H
#define NATIVESDR_WRAPPED_BLOCK_H

#include <nlohmann/json.hpp>
#include <nativesdr/pipeline/block/schematic.h>

class wrapped_block_instance;

typedef std::shared_ptr<wrapped_block_instance> wrapped_block;

class wrapped_block_instance : public pipeline::block_data_instance {

public:

    [[nodiscard]] virtual nlohmann::json toJson() const = 0;

    [[nodiscard]] virtual pipeline::block_connection_base getInputPin(ax::NodeEditor::PinId pin) const = 0;

    [[nodiscard]] virtual pipeline::block_connection_base getOutputPin(ax::NodeEditor::PinId pin) const = 0;

    float x = 0, y = 0;

};

wrapped_block fromFactory(const pipeline::block_factory& factory, size_t id, const std::string& type);

wrapped_block fromJson(const nlohmann::json& json, pipeline::node_manager* nodeManager);

#endif //NATIVESDR_WRAPPED_BLOCK_H
