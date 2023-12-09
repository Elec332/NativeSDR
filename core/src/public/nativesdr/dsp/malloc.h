//
// Created by Elec332 on 13/11/2021.
//

#ifndef NATIVESDR_MALLOC_H
#define NATIVESDR_MALLOC_H

#include <nativesdr/core_export.h>
#include <cstddef>

namespace dsp {

    CORE_EXPORT void* malloc(size_t length);

    template<class T>
    T* malloc(size_t length) {
        return (T*) malloc(length * sizeof(T));
    }

    CORE_EXPORT void free(void* ptr);

    CORE_EXPORT size_t getAlignment();

}

#endif //NATIVESDR_MALLOC_H
