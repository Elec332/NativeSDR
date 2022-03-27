//
// Created by Elec332 on 26/11/2021.
//

#include <nativesdr/hackrf_block.h>
#include <libhackrf/hackrf.h>
#include <nativesdr/dsp/malloc.h>
#include <nativesdr/dsp/iq_converter.h>

static uint32_t K = 1000;
static uint32_t M = K * K;

static std::vector<uint32_t> sampleRates = {8 * M, 10 * M, 12500 * K, 16 * M, 20 * M};
static std::vector<std::string> sampleRateStrings = std::accumulate(sampleRates.begin(), sampleRates.end(), std::vector<std::string>(), [](std::vector<std::string> a, uint32_t sri) {
    double sr = sri;
    if (sr >= M) {
        a.emplace_back(std::to_string(sr / M) + " MSPS");
    } else if (sr >= K) {
        a.emplace_back(std::to_string(sr / K) + " KSPS");
    } else {
        a.emplace_back(std::to_string(sr) + " SPS");
    }
    return a;
});

class HackRFSourceBlock : public pipeline::source_block {

public:

    HackRFSourceBlock() : pipeline::source_block("HackRF Source Block", ImColor(255, 0, 0)) {
        sampleData = dsp::malloc<utils::sampleData>(1);
        stream = pipeline::createStream<utils::complex>();
        stream->auxData = sampleData;
        buf = dsp::malloc<uint8_t>(pipeline::BUFFER_COUNT);
        converter = dsp::getConverter(8, true);

        addInput("Frequency in", utils::frequencyType(), freq, [&](int) {
            if (freq && *freq != currentFreq) {
                setFrequency(*freq);
            }
        });
        cb = addOutput("IQ out", utils::complexStreamType(), stream, true);
    }

    ~HackRFSourceBlock() {
        pipeline::deleteStream(stream);
        dsp::free(sampleData);
        dsp::free(buf);
    }

    void postConstruction() override {
        for (int i = 0; i < sampleRates.size(); ++i) {
            if (sampleRates[i] == sampleRate) {
                sampleRateIdx = i;
                break;
            }
        }
        sampleRate = sampleRates[sampleRateIdx];
    }

    void setFrequency(uint64_t newFreq) override {
        if (device) {
            hackrf_set_freq(device, newFreq);
        }
        currentFreq = newFreq;
        if (freq) {
            *freq = currentFreq;
        }
        sampleData->centerFreq = currentFreq;
    }

    uint64_t getFrequency() override {
        return currentFreq;
    }

    void setSampleRate(uint32_t sampleRate_) override {
        uint32_t bw = hackrf_compute_baseband_filter_bw(sampleRate_);
        if (device) {
            hackrf_set_sample_rate(device, sampleRate_);
            hackrf_set_baseband_filter_bandwidth(device, bw);
        }
        sampleRate = sampleRate_;
        bandwidth = bw;
        sampleData->sampleRate = sampleRate;
        sampleData->bandwidth = bandwidth;
    }

    uint32_t getSampleRate() override {
        return sampleRate;
    }

    uint32_t getBandwidth() override {
        return bandwidth;
    }

    void setGain(int gain_) override {
        if (device) {
            hackrf_set_lna_gain(device, gain_ * 8);
        }
        gain = gain_;
    }

    int getGain() override {
        return gain * 8;
    }

    void setVGAGain(int gain_) {
        if (device) {
            hackrf_set_vga_gain(device, gain_ * 2);
        }
        gainVGA = gain_;
    }

    int getVGAGain() {
        return gainVGA * 2;
    }

    void setAmpEnabled(bool amp) {
        if (device) {
            hackrf_set_amp_enable(device, amp);
        }
        ampEnable = amp;
    }

    void setPowerEnabled(bool amp) {
        if (device) {
            hackrf_set_antenna_enable(device, amp);
        }
        biasTee = amp;
    }

    void start() override {
        int err = hackrf_open(&device);
        if (err != HACKRF_SUCCESS) {
            return;
        }
        setSampleRate(sampleRate);
        setGain(gain);
        setVGAGain(gainVGA);
        setFrequency(currentFreq);
        setAmpEnabled(ampEnable);
//        hackrf_set_antenna_enable()
        hackrf_start_rx(device, receiveSamples, this);
        cb(1);
        stream->start();
        stopped = false;
    }

    static int receiveSamples(hackrf_transfer* transfer) {
        auto source = (HackRFSourceBlock*) transfer->rx_ctx;
        if (source->stopped) {
            return 0;
        }
        source->stream->write([&](utils::complex* dat) {
            int read = transfer->valid_length;
            read /= 2;
            source->converter(transfer->buffer, dat, read);
            return read;
        });
        return 0;
    }

    void stop() override {
        stopped = true;
        stream->stop();
        if (device) {
            hackrf_stop_rx(device);
            hackrf_close(device);
            device = nullptr;
        }
    }

    void drawMiddle() override {
    }

    bool hasMenu() override {
        return true;
    }

    void drawMenu() override {
        if (ImGui::SliderInt("Gain", &gain, 0, 6)) {
            setGain(gain);
        }
        if (ImGui::SliderInt("VGA Gain", &gainVGA, 0, 31)) {
            setVGAGain(gainVGA);
        }
        if (ImGui::Checkbox("AMP", &ampEnable)) {
            setAmpEnabled(ampEnable);
        }
        if (ImGui::Checkbox("Bias-tee", &biasTee)) {
            setPowerEnabled(biasTee);
        }
        ImGui::Text("SampleRate:");
        ImGui::SameLine();
        if (ImGui::Combo("##SampleRateCombo", &sampleRateIdx, sampleRateStrings)) {
            sampleRate = sampleRates[sampleRateIdx];
            setSampleRate(sampleRate);
        }
        ImGui::NewLine();
        ImGui::Text("Frequency: %.3fHMz", (float) getFrequency() / 1000000.0f);
        ImGui::Text("Samplerate: %d MSPS", getSampleRate() / M);
        ImGui::Text("Bandwidth: %d MHz", getBandwidth() / M);
        ImGui::Text("LNA Gain: %d dB", getGain());
        ImGui::Text("VGA Gain: %d dB", getVGAGain());

    }

private:

    pipeline::connection_callback cb;
    pipeline::datastream<utils::complex>* stream;
    utils::sampleData* sampleData;
    dsp::IQConverter converter;
    uint8_t* buf;

    uint64_t* freq = nullptr;

    bool stopped = false;
    hackrf_device* device = nullptr;

    int sampleRateIdx = 0;

    int gain = 0, gainVGA = 0;
    uint64_t currentFreq = 0;
    uint32_t sampleRate = 800000;
    uint32_t bandwidth = 800000;
    bool ampEnable = false;
    bool biasTee = false;

};

pipeline::source_block_ptr createHackRFSource() {
    return std::make_shared<HackRFSourceBlock>();
}