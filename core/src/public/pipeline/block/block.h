//
// Created by Elec332 on 16/10/2021.
//

#ifndef NATIVESDR_BLOCK_H
#define NATIVESDR_BLOCK_H

#include <NativeSDRGraphics.h>
#include <util/object_type.h>
#include <list>
#include <utility>
#include <memory>

#define MAX_BLOCK_PINS 31

namespace pipeline {

    class block_connection_base_instance {

    public:

        [[nodiscard]] virtual std::string getName() const = 0;

        [[nodiscard]] virtual const utils::object_type_base* getType() const = 0;

        [[nodiscard]] virtual uint8_t getId() const = 0;

        [[nodiscard]] virtual bool canConnectMultiple() const = 0;

    };

    typedef std::shared_ptr<block_connection_base_instance> block_connection_base;

    template<class T>
    class block_connection;

    class block;

    typedef std::shared_ptr<block> block_ptr;

    class block {

    public:

        block(std::string name, ImColor color) : name_(std::move(name)) {
            color_.Value = color.Value;
        }

        typedef std::list<std::shared_ptr<pipeline::block_connection_base_instance>> connection_list;

        [[nodiscard]] std::string getName() const {
            return name_;
        };

        virtual void start() = 0;

        virtual void stop() = 0;

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

    protected:

        template<class T>
        void addInput(std::string name, const utils::object_type<T>* type, T*& object, bool multi) {
            addInput(name, type, object, multi, pinCounter++);
        }

        template<class T>
        void addOutput(std::string name, const utils::object_type<T>* type, T*& object, bool multi) {
            addOutput(name, type, object, multi, pinCounter++);
        }

        template<class T>
        void addInput(std::string name, const utils::object_type<T>* type, T*& object, bool multi, uint8_t id) {
            inputs.push_front(std::make_shared<pipeline::block_connection<T>>(name, type, object, multi, id));
        }

        template<class T>
        void addOutput(std::string name, const utils::object_type<T>* type, T*& object, bool multi, uint8_t id) {
            outputs.push_front(std::make_shared<pipeline::block_connection<T>>(name, type, object, multi, id));
        }

    private:

        uint8_t pinCounter = 1;
        const std::string name_;
        ImColor color_;
        connection_list inputs, outputs;

    };

    template<class T>
    class block_connection : public block_connection_base_instance {

    public:

        block_connection(std::string name, const utils::object_type<T>* type, T*& object, bool multi, uint8_t id) :
                name(std::move(name)),
                obj(object),
                type(type),
                multi(multi),
                id(id) {
            if (id < 1 || id > MAX_BLOCK_PINS) {
                throw std::exception("Max pins exceeded");
            }
        }

        [[nodiscard]] const utils::object_type<T>* getType() const override {
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

        void setObject(T* object) const {
            obj = object;
        }

    private:

        const uint8_t id;
        T*& obj;
        const utils::object_type<T>* type;
        const std::string name;
        const bool multi;

    };

}

#endif //NATIVESDR_BLOCK_H
