//
// Created by Elec332 on 20/11/2021.
//

#ifndef NATIVESDR_WINDOWS_H
#define NATIVESDR_WINDOWS_H

#include <nativesdr/util/types.h>

namespace dsp {

    const float _HAMM_A = 25.0 / 46;

    typedef float (* WindowFunction)(size_t sample, size_t totalSamples);

    inline float cosFirstOrderWindow(size_t sample, size_t totalSamples, float a0) {
        return a0 - ((1 - a0) * cos((2 * PI_DSP * sample) / totalSamples));
    }

    inline float squareWindow(size_t sample, size_t totalSamples) {
        return 1;
    }

    inline float hammingWindow(size_t sample, size_t totalSamples) {
        return cosFirstOrderWindow(sample, totalSamples, _HAMM_A);
    }

    inline float hannWindow(size_t sample, size_t totalSamples) {
        return cosFirstOrderWindow(sample, totalSamples, 0.5);
    }

}

#endif //NATIVESDR_WINDOWS_H
