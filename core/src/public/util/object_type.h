//
// Created by Elec332 on 16/10/2021.
//

#ifndef NATIVESDR_OBJECT_TYPE_H
#define NATIVESDR_OBJECT_TYPE_H

#include <nativesdr_core_export.h>
#include <string>
#include <util/types.h>
#include <pipeline/datastream.h>

namespace utils {

    class object_type_base {

    public:

        [[nodiscard]] virtual std::string getName() const = 0;

        virtual void drawIcon(bool connected) const = 0;

    };

    template<class T>
    class object_type : public object_type_base {
    };

    NATIVESDR_CORE_EXPORT const utils::object_type<datastream<utils::complex>>* complexStreamType();

    NATIVESDR_CORE_EXPORT const utils::object_type<datastream<uint8_t>>* dataStreamType();

    NATIVESDR_CORE_EXPORT const utils::object_type<utils::sampleData>* sampleDataType();

    NATIVESDR_CORE_EXPORT const utils::object_type<std::string>* stringType();

}

#endif //NATIVESDR_OBJECT_TYPE_H
