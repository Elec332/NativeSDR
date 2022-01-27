//
// Created by Elec332 on 09/12/2021.
//
#include <dsp/dsp.h>
#include <dsp/malloc.h>
#include <volk/volk.h>
#include <iostream>
#include <cassert>
#include <numeric>
#include <cstring>
#include <dsp/windows.h>

namespace dsp {

    int getTapLen(double cutoff, size_t sampleRate) {
        auto fc = std::min(cutoff / (double) sampleRate, 0.5);
        int c = (int) (4.0f / fc);
        if (c % 2 == 0) {
            c++;
        }
        return c;
    }

    void makeTaps(float* taps, int tapLen, double cutoff, size_t sampleRate, float gain = 1) {
        assert(tapLen % 2 != 0);
        assert(sampleRate * 0.5 >= cutoff);

        auto fc = std::min(cutoff / (double) sampleRate, 0.5);
        auto lm1 = (tapLen - 1) / 2;
        auto tim = fc * 2 * PI_DSP;

        float total = 0;
        for (int i = 0; i < tapLen; ++i) {
            auto n = (float) (i - lm1);
            float val;
            if (n == 0) {
                val = (float) (tim / PI_DSP) * dsp::hammingWindow(i, tapLen - 1);
            } else {
                val = (float) (sin(tim * n) / (PI_DSP * n)) * dsp::hammingWindow(i, tapLen - 1);
            }
            taps[i] = val;
            total += val;
        }
        gain /= total;
        volk_32f_s32f_multiply_32f(taps, taps, gain, tapLen);
    }

    class ResamplingWindow : public dsp_plan_ios {

    public:

        ResamplingWindow(size_t sampleRate, size_t bandwidth, size_t inSize, size_t newSampleRate, size_t req_bw) : inSize(inSize) {
            auto cutoff = (double) std::min(bandwidth, std::min(req_bw, newSampleRate)) / 2.0;
            auto gcd = std::gcd(sampleRate, newSampleRate);
            interpolation = (int) (newSampleRate / gcd);
            decimation = (int) (sampleRate / gcd);

            outLen = inSize;
            backlog = dsp::malloc<utils::complex>(outLen);

            int tapLen = getTapLen(cutoff, sampleRate * interpolation);
            if (tapLen > inSize) {
                throw std::runtime_error("Too many taps");
            }
            auto taps = dsp::malloc<float>(tapLen);

            makeTaps(taps, tapLen, cutoff, sampleRate * interpolation, (float) interpolation);

            filterCount = interpolation;
            polyTaps = dsp::malloc<float*>(filterCount);
            polyTapLen = std::ceil(tapLen / (double) filterCount);
            for (int i = 0; i < filterCount; ++i) {
                polyTaps[i] = dsp::malloc<float>(polyTapLen);
                memset(polyTaps[i], 0, polyTapLen * sizeof(float));
            }

            for (int i = 0; i < tapLen; i++) {
                polyTaps[i % filterCount][(polyTapLen - 1) - (i / filterCount)] = taps[i];
            }

            fullBuf = dsp::malloc<utils::complex>(inSize * 2);
            offset = decimation;
            dsp::free(taps);
        }

        ~ResamplingWindow() {
            dsp::free(backlog);
            dsp::free(fullBuf);
            for (int i = 0; i < filterCount; ++i) {
                dsp::free(polyTaps[i]);
            }
            dsp::free(polyTaps);
        }

        void execute(const utils::complex* in, pipeline::datastream<utils::complex>* out) override {
            memcpy(&fullBuf[polyTapLen], in, inSize * sizeof(utils::complex));
            int i = offset;
            while (i < inSize) {
                if (blIdx >= outLen) {
                    out->write([&](utils::complex* stream) {
                        memcpy(stream, backlog, outLen * sizeof(utils::complex));
                        return (int) outLen;
                    });
                    blIdx = 0;
                }
                volk_32fc_32f_dot_prod_32fc((lv_32fc_t*) &backlog[blIdx++], (lv_32fc_t*) &fullBuf[i], polyTaps[tapCounter], polyTapLen);

                tapCounter += decimation;
                i += tapCounter / interpolation;
                tapCounter %= interpolation;
            }
            offset = (int) (i - inSize);
            memmove(fullBuf, &fullBuf[inSize], polyTapLen * sizeof(utils::complex));
        }

    private:

        size_t inSize;
        int interpolation;
        int decimation;

        utils::complex* fullBuf;
        int remIdx = 0;
        int offset = 0;

        float** polyTaps;
        int filterCount;
        int polyTapLen;

        size_t outLen;
        utils::complex* backlog;
        int blIdx = 0;
        int tapCounter = 0;

    };

    dsp_plan_ios_ptr
    resamplingWindow(size_t sampleRate, size_t bandwidth, size_t inSize, size_t newSampleRate, size_t newBandwidth) {
        return std::make_shared<ResamplingWindow>(sampleRate, bandwidth, inSize, newSampleRate, newBandwidth);
    }

}