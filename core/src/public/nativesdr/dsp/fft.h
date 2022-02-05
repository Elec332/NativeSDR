//
// Created by Elec332 on 23/10/2021.
//

#ifndef NATIVESDR_FFT_H
#define NATIVESDR_FFT_H

#include <nativesdr/util/types.h>
#include <memory>
#include <nativesdr/dsp/dsp.h>

namespace dsp {

    //Made for compatibility with multiple implementations, e.g. GPU acceleration
    class fft_plan : public dsp_plan {
    };

    typedef std::shared_ptr<fft_plan> fft_plan_ptr;

    CORE_EXPORT dsp::fft_plan_ptr create_plan(int length, utils::complex* in, utils::complex* out, bool forward);

}

#endif //NATIVESDR_FFT_H
