//
// Created by Elec332 on 22/10/2021.
//

#ifndef NATIVESDR_TYPES_H
#define NATIVESDR_TYPES_H

#include <cstdint>

#define PI_DSP 3.14159265358979323846

namespace utils {

    typedef struct complex {

        complex operator*(const float b) const {
            return complex{re * b, im * b};
        }

        complex operator/(const float b) const {
            return complex{re / b, im / b};
        }

        complex operator*(const complex& b) const {
            return complex{(re * b.re) - (im * b.im), (im * b.re) + (re * b.im)};
        }

        complex operator+(const complex& b) const {
            return complex{re + b.re, im + b.im};
        }

        complex operator-(const complex& b) const {
            return complex{re - b.re, im - b.im};
        }

        float re;
        float im;

    } complex;

    typedef struct audio {

        float left;
        float right;

    } audio;

    typedef struct sampleData {

        uint64_t centerFreq;
        uint32_t sampleRate;
        uint32_t bandwidth;

    } sampleData;

}

#endif //NATIVESDR_TYPES_H
