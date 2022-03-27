//
// Created by Elec332 on 16/10/2021.
//

#ifndef NATIVESDR_BLOCK_H
#define NATIVESDR_BLOCK_H

#include <nativesdr/NativeSDRGraphics.h>
#include <nativesdr/util/object_type.h>
#include <nativesdr/core_export.h>
#include <list>
#include <utility>
#include <memory>
#include <iostream>
#include <nlohmann/json.hpp>

#define MAX_BLOCK_PINS 31

namespace pipeline {

    class block_connection_base_instance;

    typedef std::shared_ptr<block_connection_base_instance> block_connection_base;

    typedef std::function<void(int flags)> connection_callback;

    class block_connection_base_instance {

        CORE_EXPORT static pipeline::block_connection_base createBlockConnectionImpl(const std::string& name, const utils::object_type_base* type, void*& object, const pipeline::connection_callback& callback, bool multi, uint8_t id, const std::type_info& type_info);

    public:

        template<class T>
        static block_connection_base createBlockConnection(const std::string& name, const utils::object_type<T>* type, T*& object, const pipeline::connection_callback& callback, bool multi, uint8_t id) {
            return createBlockConnectionImpl(name, type, (void*&) object, callback, multi, id, typeid(T));
        }

        [[nodiscard]] virtual std::string getName() const = 0;

        [[nodiscard]] virtual const utils::object_type_base* getType() const = 0;

        [[nodiscard]] virtual uint8_t getId() const = 0;

        [[nodiscard]] virtual bool canConnectMultiple() const = 0;

        virtual void onValueChanged(int flags) const = 0;

    };

    class block {

    public:

        block(std::string name, ImColor color) : name_(std::move(name)) {
            color_.Value = color.Value;
        }

        typedef std::list<std::shared_ptr<pipeline::block_connection_base_instance>> connection_list;

        [[nodiscard]] std::string getName() const {
            return name_;
        };

        virtual void postConstruction() {
        }

        virtual void start() = 0;

        virtual void stop() = 0;

        virtual bool hasMenu() {
            return false;
        }

        virtual void drawMenu() {
        }

        virtual void drawDialogs() {
        }

        [[nodiscard]] const connection_list& getInputs() const {
            return inputs;
        };

        [[nodiscard]] const connection_list& getOutputs() const {
            return outputs;
        };

        [[nodiscard]] ImColor getColor() const {
            return color_;
        };

        virtual void drawMiddle() = 0;

        virtual void toJson(nlohmann::json& json) const {
        }

        virtual void readJson(const nlohmann::json& json) {
        }

    protected:

        template<class T>
        void addInput(std::string name, const utils::object_type<T>* type, T*& object, const pipeline::connection_callback& callback, uint8_t id) {
            inputs.push_back(pipeline::block_connection_base_instance::createBlockConnection(name, type, object, callback, false, id));
        }

        template<class T>
        connection_callback addOutput(std::string name, const utils::object_type<T>* type, T*& object, bool multi, uint8_t id) {
            block_connection_base conn = pipeline::block_connection_base_instance::createBlockConnection(name, type, object, nullptr, multi, id);
            outputs.push_back(conn);
            return [conn](int flags) {
                conn->onValueChanged(flags);
            };
        }

        template<class T>
        void addInput(std::string name, const utils::object_type<T>* type, T*& object, const pipeline::connection_callback& callback) {
            addInput(name, type, object, callback, pinCounter++);
        }

        template<class T>
        connection_callback addOutput(std::string name, const utils::object_type<T>* type, T*& object, bool multi) {
            return addOutput(name, type, object, multi, pinCounter++);
        }

        template<class T>
        void addInput(std::string name, const utils::object_type<T>* type, T*& object) {
            addInput(name, type, object, nullptr);
        }

    private:

        uint8_t pinCounter = 1;
        const std::string name_;
        ImColor color_;
        connection_list inputs, outputs;

    };

    typedef std::shared_ptr<block> block_ptr;

    class source_block : public pipeline::block {

    public:

        source_block(std::string name, ImColor color) : pipeline::block(std::move(name), color) {
        }

        virtual void setFrequency(uint64_t frequency) = 0;

        virtual uint64_t getFrequency() = 0;

        virtual void setSampleRate(uint32_t sampleRate) = 0;

        virtual uint32_t getSampleRate() = 0;

        virtual uint32_t getBandwidth() = 0;

        virtual void setGain(int gain) = 0;

        virtual int getGain() = 0;

    };

    typedef std::shared_ptr<source_block> source_block_ptr;

}

#endif //NATIVESDR_BLOCK_H
