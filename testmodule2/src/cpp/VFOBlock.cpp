//
// Created by Elec332 on 03/12/2021.
//

#include <TestModule.h>
#include <dsp/malloc.h>
#include <volk/volk.h>

class VFOBlock : public pipeline::threaded_block {

    const lv_32fc_t zeroPhase = lv_cmake(1.0f, 0.0f); // start at 1 (0 rad phase)

public:

    VFOBlock() : pipeline::threaded_block("VFO Block", ImColor(255, 0, 0)) {
        sampleData = dsp::malloc<utils::sampleData>(1);
        streamOut = pipeline::createStream<utils::complex>();
        streamOut->auxData = sampleData;

        addInput("IQ in", utils::complexStreamType(), streamIn, [&](int) {
            if (streamIn && streamIn->auxData) {
                auto sd = (utils::sampleData*) streamIn->auxData;
                sampleData->bandwidth = sd->bandwidth;
                sampleData->sampleRate = sd->sampleRate;
                sampleData->centerFreq = sd->centerFreq;
                setValue();
            }
        });
        addInput("Frequency Offset", utils::numberType(), offsetIn, [&](int) {
            setValue();
        });
        ocb = addOutput("IQ out", utils::complexStreamType(), streamOut, true);

        phase = zeroPhase;
    }

    ~VFOBlock() {
        pipeline::deleteStream(streamOut);
        dsp::free(sampleData);
    }

    void setValue() {
        if (offsetIn && *offsetIn != 0) {
            auto div = (double) *offsetIn / sampleData->sampleRate;
            phase_increment = lv_cmake(std::cos(div * 2 * PI_DSP), std::sin(div * 2 * PI_DSP));
        } else {
            phase_increment = zeroPhase;
        }
        ocb(1);
    }

    void loop() override {
        if (streamIn) {
            streamIn->read([&](const utils::complex* data, int len) {
                streamOut->write([&](utils::complex* dat) {
                    volk_32fc_s32fc_x2_rotator_32fc((lv_32fc_t*) dat, (lv_32fc_t*) data, phase_increment, &phase, len);
                    return len;
                });
            });
        }
    }

    void start() override {
        streamOut->start();
        pipeline::threaded_block::start();
    }

    void stop() override {
        streamOut->stop();
        pipeline::threaded_block::stop();
    }

    void drawMiddle() override {
    }

private:

    pipeline::datastream<utils::complex>* streamIn = nullptr;
    int* offsetIn = nullptr;

    pipeline::datastream<utils::complex>* streamOut;
    pipeline::connection_callback ocb;
    utils::sampleData* sampleData;

    lv_32fc_t phase;
    lv_32fc_t phase_increment;

};

pipeline::block_ptr createOffsetBlock() {
    return std::make_shared<VFOBlock>();
}