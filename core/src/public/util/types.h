//
// Created by Elec332 on 22/10/2021.
//

#ifndef NATIVESDR_TYPES_H
#define NATIVESDR_TYPES_H

#include <cstdint>

namespace utils {

    typedef struct complex {

        float re;
        float im;

    } complex;

    typedef struct sampleData {

        uint32_t centerFreq;
        uint32_t sampleRate;
        uint32_t offset;
        uint32_t bandwidth;

    } sampleData;

}

#endif //NATIVESDR_TYPES_H
