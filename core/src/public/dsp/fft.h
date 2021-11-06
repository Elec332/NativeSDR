//
// Created by Elec332 on 23/10/2021.
//

#ifndef NATIVESDR_FFT_H
#define NATIVESDR_FFT_H

#include <util/types.h>
#include <memory>
#include <nativesdr_core_export.h>

namespace dsp {

    //Made for compatibility with multiple implementations, e.g. GPU acceleration
    class fft_plan {

    public:

        virtual void execute() = 0;

    };

    typedef std::shared_ptr<fft_plan> fft_plan_ptr;

    NATIVESDR_CORE_EXPORT dsp::fft_plan_ptr create_plan(int length, utils::complex* in, utils::complex* out, bool forward);

}

NATIVESDR_CORE_EXPORT utils::complex* malloc_complex(int length);

#endif //NATIVESDR_FFT_H
