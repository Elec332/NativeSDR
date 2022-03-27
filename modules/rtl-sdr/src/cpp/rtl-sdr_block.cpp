//
// Created by Elec332 on 26/11/2021.
//

#include <nativesdr/rtl-sdr_block.h>
#include <nativesdr/pipeline/block/block_base_impl.h>
#include <rtl-sdr.h>
#include <nativesdr/dsp/malloc.h>
#include <nativesdr/dsp/iq_converter.h>

class RTLSDRBlock : public pipeline::threaded_source_block {

public:

    RTLSDRBlock() : pipeline::threaded_source_block("RTLSDRBlock", ImColor(255, 0, 0)) {
        sampleData = dsp::malloc<utils::sampleData>(1);
        stream = pipeline::createStream<utils::complex>();
        stream->auxData = sampleData;
        buf = dsp::malloc<uint8_t>(pipeline::BUFFER_COUNT);
        converter = dsp::getConverter(8, false);

        addInput("Frequency in", utils::frequencyType(), freq, [&](int) {
            if (freq && *freq != centerFreq) {
                setFrequency(*freq);
            }
        });
        cb = addOutput("IQ out", utils::complexStreamType(), stream, true);
    }

    ~RTLSDRBlock() {
        pipeline::deleteStream(stream);
        dsp::free(sampleData);
        dsp::free(buf);
    }

    void setFrequency(uint64_t newFreq) override {
        if (device) {
            rtlsdr_set_center_freq(device, newFreq);
//            centerFreq = rtlsdr_get_center_freq64(device);
            centerFreq = rtlsdr_get_center_freq(device);
            if (freq) {
                *freq = centerFreq;
            }
            sampleData->centerFreq = centerFreq;
        }
    }

    uint64_t getFrequency() override {
        return centerFreq;
    }

    void setSampleRate(uint32_t sampleRate_) override {
        rtlsdr_set_sample_rate(device, sampleRate_);
        sampleRate = rtlsdr_get_sample_rate(device);
        sampleData->sampleRate = sampleRate;
    }

    uint32_t getSampleRate() override {
        return sampleRate;
    }

    uint32_t getBandwidth() {
        return 0;
    }

    void setGain(int gain) override {
    }

    int getGain() override {
        return 0;
    }

    void loop() override {
        if (device) {
            stream->write([&](utils::complex* dat) {
                int read = 0;
                if (rtlsdr_read_sync(device, buf, pipeline::BUFFER_COUNT, &read) == 0) {
                    read /= 2;
                    converter(buf, dat, read);
                    return read;
                }
                return 0;
            });
        }
    }

    void start() override {
        if (rtlsdr_get_device_count() > 0) {
            rtlsdr_open(&device, 0);
            rtlsdr_set_tuner_gain_mode(device, 0);
            rtlsdr_set_agc_mode(device, 0);
//            rtlsdr_set_direct_sampling(device, 0);
//            rtlsdr_set_offset_tuning(device, 0);
//            rtlsdr_
//            rtlsdr_set_tuner_if_gain(device, );
            if (freq) {
                setFrequency(*freq);
            } else {
                setFrequency(100000000);
            }
            setSampleRate(2400000);
            rtlsdr_set_tuner_bandwidth(device, sampleData->sampleRate);
            sampleData->bandwidth = sampleData->sampleRate;
//            rtlsdr_set_and_get_tuner_bandwidth(device, sampleData->sampleRate, &sampleData->bandwidth, 0);
            rtlsdr_reset_buffer(device);
            cb(1);
        }
        stream->start();
        pipeline::threaded_source_block::start();
    }

    void stop() override {
        stream->stop();
        pipeline::threaded_source_block::stop();
        if (device) {
            rtlsdr_close(device);
            device = nullptr;
        }
    }

    void drawMiddle() override {
        if (!hasStopped() && device) {
            std::string str = "File: ";
            str += rtlsdr_get_device_name(0);
            ImGui::TextUnformatted(str.c_str());
        }
    }

private:

    pipeline::connection_callback cb;
    pipeline::datastream<utils::complex>* stream;
    utils::sampleData* sampleData;
    dsp::IQConverter converter;
    uint8_t* buf;

    uint64_t* freq = nullptr;

    rtlsdr_dev_t* device = nullptr;
    uint64_t centerFreq = 0;
    uint32_t sampleRate = 0;

};

pipeline::source_block_ptr createRTLSDRSource() {
    return std::make_shared<RTLSDRBlock>();
}