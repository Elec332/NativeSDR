//
// Created by Elec332 on 19/03/2022.
//

#include <util/default_core_block.h>

class DCBlockerBlock : public pipeline::threaded_block {

public:

    DCBlockerBlock() : pipeline::threaded_block("DC Blocker", ImColor(255, 0, 0)) {
        sampleData = dsp::malloc<utils::sampleData>(1);
        streamOut = pipeline::createStream<utils::complex>();
        streamOut->auxData = sampleData;
        ocb = addOutput("IQ out", utils::complexStreamType(), streamOut, true);
        addInput("IQ in", utils::complexStreamType(), streamIn, [&](int) {
            if (streamIn && streamIn->auxData) {
                streamOut->auxData = streamIn->auxData;
            } else {
                streamOut->auxData = sampleData;
            }
            ocb(1);
        });
    }

    ~DCBlockerBlock() {
        pipeline::deleteStream(streamOut);
        dsp::free(sampleData);
    }

    void loop() override {
        if (streamIn) {
            streamIn->read([&](const utils::complex* data, int len) {
                streamOut->write([&](utils::complex* stream) {
                    for (int i = 0; i < len; i++) {
                        stream[i] = data[i] - correction;
                        correction = correction + (stream[i] * 0.00001);
                    }
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
    pipeline::datastream<utils::complex>* streamOut;

    utils::sampleData* sampleData;
    pipeline::connection_callback ocb;

    utils::complex correction = {0, 0};

};

pipeline::block_ptr createDCBlockerBlock() {
    return std::make_shared<DCBlockerBlock>();
}