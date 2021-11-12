//
// Created by Elec332 on 16/10/2021.
//

#ifndef NATIVESDR_OBJECT_TYPE_H
#define NATIVESDR_OBJECT_TYPE_H

#include <nativesdr_core_export.h>
#include <string>
#include <util/types.h>
#include <pipeline/datastream.h>
#include <iostream>

namespace utils {

    class object_type_base {

    public:

        [[nodiscard]] virtual std::string getName() const = 0;

        virtual void drawIcon(bool connected) const = 0;

        virtual bool equals(const utils::object_type_base* other) const = 0;

        virtual void setConnectionCount(void* ref, size_t count) const {
        }

    };

    template<class T>
    class object_type : public object_type_base {
    };

    NATIVESDR_CORE_EXPORT const utils::object_type<pipeline::datastream<utils::complex>>* complexStreamType();

    NATIVESDR_CORE_EXPORT const utils::object_type<pipeline::datastream<uint8_t>>* dataStreamType();

    NATIVESDR_CORE_EXPORT const utils::object_type<utils::sampleData>* sampleDataType();

    NATIVESDR_CORE_EXPORT const utils::object_type<std::string>* stringType();

}

#endif //NATIVESDR_OBJECT_TYPE_H
