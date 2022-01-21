//
// Created by Elec332 on 30/10/2021.
//

#include <dsp/iq_converter.h>
#include <volk/volk.h>

namespace dsp {

    void convert8(void* in, utils::complex* out, size_t complexSamples) {
        volk_8i_s32f_convert_32f((float*) out, (int8_t*) in, 128.0f, complexSamples * 2);
    }

    void convert16(void* in, utils::complex* out, size_t complexSamples) {
        volk_16i_s32f_convert_32f((float*) out, (int16_t*) in, 32768.0f, complexSamples * 2);
    }

    void convert32(void* in, utils::complex* out, size_t complexSamples) {
        volk_32i_s32f_convert_32f((float*) out, (int32_t*) in, 2147483648.0f, complexSamples * 2);
    }

    //Todo: Was too lazy to make this a LUT, do so later
    //Todo: Used 127.4 for RTL-SDR (Rafael), see https://cgit.osmocom.org/gr-osmosdr/commit/?id=0edfcfcba0ddf0fd5002dab70331d75a376bcf3f
    //      Check if also applicable to other SDR's
    void convert8u(void* in, utils::complex* out, size_t complexSamples) {
        auto ini = (uint8_t*) in;
        for (int i = 0; i < complexSamples; ++i) {
            out[i].im = (ini[i * 2] - 127.4f) * (1.0f / 128.0f);
            out[i].re = (ini[i * 2 + 1] - 127.4f) * (1.0f / 128.0f);
        }
    }

    IQConverter getConverter(int bitsPerSample, bool isSigned, int align) {
        if ((bitsPerSample / 8) * 2 != align) {
            return nullptr;
        }
        return getConverter(bitsPerSample, isSigned);
    }

    IQConverter getConverter(int bitsPerSample, bool isSigned) {
        if (bitsPerSample % 8 != 0) {
            return nullptr;
        }
        if (isSigned) {
            switch (bitsPerSample) {
                case 8:
                    return convert8;
                case 16:
                    return convert16;
                case 32:
                    return convert32;
                default:
                    return nullptr;
            }
        } else {
            switch (bitsPerSample) {
                case 8:
                    return convert8u;
                default:
                    return nullptr;
            }
        }
    }

}
