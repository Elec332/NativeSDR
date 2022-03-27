//
// Created by Elec332 on 25/02/2022.
//

#include <util/default_core_block.h>

class AMDemodulationBlock : public pipeline::threaded_block {

public:

    AMDemodulationBlock() : pipeline::threaded_block("AM Block", ImColor(255, 0, 0)) {
        streamOut = pipeline::createStream<utils::audio>();
        middleBuf = dsp::malloc<float>(pipeline::BUFFER_COUNT);
        addOutput("Audio out", utils::audioStreamType(), streamOut, true);
        addInput("IQ in", utils::complexStreamType(), streamIn);
    }

    ~AMDemodulationBlock() {
        pipeline::deleteStream(streamOut);
        dsp::free(middleBuf);
    }

    void loop() override {
        if (streamIn) {
            streamIn->read([&](const utils::complex* data, int len) {
                volk_32fc_magnitude_32f(middleBuf, (lv_32fc_t*) data, len);
                streamOut->write([&](utils::audio* stream) {
                    volk_32f_x2_interleave_32fc((lv_32fc_t*) stream, middleBuf, middleBuf, len);
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

    float* middleBuf;
    pipeline::datastream<utils::complex>* streamIn = nullptr;
    pipeline::datastream<utils::audio>* streamOut;

};

pipeline::block_ptr createAMDemodulationBlock() {
    return std::make_shared<AMDemodulationBlock>();
}