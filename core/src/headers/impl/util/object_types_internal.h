//
// Created by Elec332 on 04/11/2021.
//

#ifndef NATIVESDR_OBJECT_TYPES_INTERNAL_H
#define NATIVESDR_OBJECT_TYPES_INTERNAL_H

#include <util/object_type.h>

template<class T>
class abstract_type : public utils::object_type<T> {

public:

    explicit abstract_type(std::string name) noexcept: name(std::move(name)) {
    }

    [[nodiscard]] std::string getName() const override {
        return name;
    }

    bool equals(const utils::object_type_base* other) const override {
        return this == other;
    }

private:

    const std::string name;

};

template<class T>
class simple_type : public abstract_type<T> {

public:

    simple_type(std::string name, void(* drawer)(bool)) noexcept: abstract_type<T>(std::move(name)), drawer(drawer) {
    }

    void drawIcon(bool connected) const override {
        drawer(connected);
    }

private:

    void (* drawer)(bool);

};

#endif //NATIVESDR_OBJECT_TYPES_INTERNAL_H
