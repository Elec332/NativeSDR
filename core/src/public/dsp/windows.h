//
// Created by Elec332 on 20/11/2021.
//

#ifndef NATIVESDR_WINDOWS_H
#define NATIVESDR_WINDOWS_H

namespace dsp {

    typedef float (* WindowFunction)(size_t sample, size_t totalSamples);

    inline float squareWindow(size_t sample, size_t totalSamples) {
        return (sample % 2) ? 1 : -1;
    }

}

#endif //NATIVESDR_WINDOWS_H
