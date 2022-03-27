//
// Created by Elec332 on 30/10/2021.
//

#ifndef NATIVESDR_IQ_CONVERTER_H
#define NATIVESDR_IQ_CONVERTER_H

#include <nativesdr/core_export.h>
#include <nativesdr/util/types.h>
#include <cstddef>

namespace dsp {

    typedef void(* IQConverter)(void* in, utils::complex* out, size_t complexSamples);

    CORE_EXPORT IQConverter getConverter(int bitsPerSample, bool isSigned, int align);

    CORE_EXPORT IQConverter getConverter(int bitsPerSample, bool isSigned);

}

#endif //NATIVESDR_IQ_CONVERTER_H
