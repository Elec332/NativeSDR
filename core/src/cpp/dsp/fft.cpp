//
// Created by Elec332 on 23/10/2021.
//

#include <nativesdr/dsp/fft.h>
#include <fftw3.h>

class fft_impl : public dsp::fft_plan {

public:

    fft_impl(int length, utils::complex* in, utils::complex* out, bool forward) {
        plan = fftwf_plan_dft_1d(length, (fftwf_complex*) in, (fftwf_complex*) out, (forward ? FFTW_FORWARD : FFTW_BACKWARD), FFTW_ESTIMATE);
    }

    ~fft_impl() {
        fftwf_destroy_plan(plan);
    }

    void execute() override {
        fftwf_execute(plan);
    }

private:

    fftwf_plan plan;

};

dsp::fft_plan_ptr dsp::create_plan(int length, utils::complex* in, utils::complex* out, bool forward) {
    return std::make_shared<fft_impl>(length, in, out, forward);
}
