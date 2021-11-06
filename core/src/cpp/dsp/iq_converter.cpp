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
                default:
                    return nullptr;
            }
        }
    }

}
