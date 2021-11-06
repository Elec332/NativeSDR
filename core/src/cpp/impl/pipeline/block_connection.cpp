//
// Created by Elec332 on 06/11/2021.
//

#include <impl/pipeline/block_connection.h>
#include <utility>
#include <iostream>

class block_connection_impl : public block_connection {

public:

    block_connection_impl(std::string name, const utils::object_type_base* type, void*& object, pipeline::connection_callback callback, bool multi, uint8_t id) :
            name(std::move(name)), obj(&object), type(type), multi(multi), id(id), callback(std::move(callback)) {
        if (id < 1 || id > MAX_BLOCK_PINS) {
            throw std::exception("Max pins exceeded");
        }
    }

    [[nodiscard]] const utils::object_type_base* getType() const override {
        return type;
    }

    [[nodiscard]] std::string getName() const override {
        return name;
    }

    [[nodiscard]] uint8_t getId() const override {
        return id;
    }

    [[nodiscard]] bool canConnectMultiple() const override {
        return multi;
    }

    void setObject(const block_connection* object) override {
        if (!linkHandler) {
            if (object) {
                *obj = *(((block_connection_impl*) object)->obj);
            } else {
                *obj = nullptr;
            }
            if (callback) {
                callback();
            }
        }
    }

    void initOutput(schematic_link_handler* lh, ax::NodeEditor::PinId pin_) override {
        pin = pin_;
        linkHandler = lh;
    }

    void onValueChanged() const override {
        if (linkHandler) {
            linkHandler->onLinkValueChanged(pin, this);
        }
    }

private:

    const uint8_t id;
    void** obj;
    const pipeline::connection_callback callback;
    const utils::object_type_base* type;
    const std::string name;
    const bool multi;

    ax::NodeEditor::PinId pin = 0;
    schematic_link_handler* linkHandler = nullptr;

};

pipeline::block_connection_base pipeline::block_connection_base_instance::createBlockConnectionImpl(const std::string& name, const utils::object_type_base* type, void*& object, const pipeline::connection_callback& callback, bool multi, uint8_t id) {
    return std::make_shared<block_connection_impl>(name, type, object, callback, multi, id);
}