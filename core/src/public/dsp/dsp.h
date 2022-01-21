//
// Created by Elec332 on 09/12/2021.
//

#ifndef NATIVESDR_DSP_H
#define NATIVESDR_DSP_H

#include <memory>
#include <util/types.h>
#include <pipeline/datastream.h>
#include <nativesdr_core_export.h>

namespace dsp {

    class dsp_plan {

    public:

        virtual void execute() = 0;

    };

    class dsp_plan_io {

    public:

        virtual void execute(const utils::complex* in, utils::complex* out) = 0;

    };

    class dsp_plan_ios {

    public:

        virtual void execute(const utils::complex* in, pipeline::datastream<utils::complex>* out) = 0;

    };

    typedef std::shared_ptr<dsp_plan> dsp_plan_ptr;
    typedef std::shared_ptr<dsp_plan_io> dsp_plan_io_ptr;
    typedef std::shared_ptr<dsp_plan_ios> dsp_plan_ios_ptr;

    NATIVESDR_CORE_EXPORT dsp_plan_ios_ptr resamplingWindow(size_t sampleRate, size_t bandwidth, size_t inSize, size_t newSampleRate, size_t newBandwidth);

}

#endif //NATIVESDR_DSP_H
