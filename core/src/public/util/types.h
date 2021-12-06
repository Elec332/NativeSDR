//
// Created by Elec332 on 22/10/2021.
//

#ifndef NATIVESDR_TYPES_H
#define NATIVESDR_TYPES_H

#include <cstdint>

#define PI_DSP 3.14159265358979323846

namespace utils {

    typedef struct complex {

        float re;
        float im;

    } complex;

    typedef struct sampleData {

        uint64_t centerFreq;
        uint32_t sampleRate;
        uint32_t bandwidth;

    } sampleData;

}

#endif //NATIVESDR_TYPES_H
