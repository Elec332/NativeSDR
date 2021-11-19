//
// Created by Elec332 on 13/11/2021.
//

#ifndef NATIVESDR_MALLOC_H
#define NATIVESDR_MALLOC_H

#include <nativesdr_core_export.h>

namespace dsp {

    template<class T>
    T* malloc(size_t length) {
        return (T*) malloc(length * sizeof(T));
    }

    NATIVESDR_CORE_EXPORT void* malloc(size_t length);

    NATIVESDR_CORE_EXPORT void free(void* ptr);

}

#endif //NATIVESDR_MALLOC_H
