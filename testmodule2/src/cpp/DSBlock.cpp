//
// Created by Elec332 on 09/12/2021.
//

#include <TestModule.h>
#include <dsp/malloc.h>
#include "dsp/dsp.h"

class ResampleBlock : public pipeline::threaded_block {

public:

    ResampleBlock() : pipeline::threaded_block("Resample Block", ImColor(255, 0, 0)) {
        sampleData = dsp::malloc<utils::sampleData>(1);
        streamOut = pipeline::createStream<utils::complex>();
        streamOut->auxData = sampleData;
        ocb = addOutput("IQ out", utils::complexStreamType(), streamOut, true);

        addInput("IQ in", utils::complexStreamType(), streamIn, [&](int) {
            if (streamIn && streamIn->auxData) {
                auto sd = (utils::sampleData*) streamIn->auxData;
                if (!sampleRate) {
                    sampleData->bandwidth = sd->bandwidth;
                    sampleData->sampleRate = sd->sampleRate;
                }
                sampleData->centerFreq = sd->centerFreq;
                ocb(1);
            } else if (!streamIn) {
                resLen = 0;
            }
        });
        addInput("Samplerate Out", utils::numberType(), sampleRate, [&](int) {
            if (sampleRate) {
                sampleData->sampleRate = *sampleRate;
                setBandwidth();
                ocb(1);
            } else if (streamIn && streamIn->auxData) {
                auto sd = (utils::sampleData*) streamIn->auxData;
                sampleData->sampleRate = sd->sampleRate;
                setBandwidth();
                ocb(1);
            }
            if (streamIn) {
                resetSampler();
            }
        });
    }

    ~ResampleBlock() {
        pipeline::deleteStream(streamOut);
        dsp::free(sampleData);
    }

    void setBandwidth() {
        sampleData->bandwidth = sampleData->sampleRate;
    }

    void loop() override {
        if (streamIn && streamIn->auxData) {
            streamIn->read([&](const utils::complex* data, int len) {
                if (sampleData->sampleRate == ((utils::sampleData*) streamIn->auxData)->sampleRate) {
                    streamOut->write([data, len](utils::complex* rDat) {
                        memcpy(rDat, data, len * sizeof(utils::complex));
                        return len;
                    });
                    return;
                }
                if (resLen != len) {
                    resLen = len;
                    resetSampler();
                }
                resampler->execute(data, streamOut);
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
        resLen = 0;
        resampler = nullptr;
    }

    void drawMiddle() override {
    }

private:

    void resetSampler() {
        if (resLen == 0) {
            return;
        }
        auto sd = (utils::sampleData*) streamIn->auxData;
        resampler = dsp::resamplingWindow(sd->sampleRate, sd->bandwidth, resLen, sampleData->sampleRate, sampleData->bandwidth);
    }

    dsp::dsp_plan_ios_ptr resampler;
    int resLen = 0;
    int* sampleRate = nullptr;

    pipeline::datastream<utils::complex>* streamIn = nullptr;

    pipeline::datastream<utils::complex>* streamOut;
    utils::sampleData* sampleData;
    pipeline::connection_callback ocb;

};

pipeline::block_ptr createResampleBlock() {
    return std::make_shared<ResampleBlock>();
}