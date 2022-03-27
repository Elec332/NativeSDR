//
// Created by Elec332 on 03/03/2022.
//

#include <nativesdr/uhd_block.h>
#include <nativesdr/dsp/malloc.h>
#include <nativesdr/dsp/iq_converter.h>
#include <uhd/convert.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <nativesdr/pipeline/block/block_base_impl.h>

static int MBOARD = 0;
static int CHANNEL = 0;

static uint32_t K = 1000;
static uint32_t M = K * K;

static std::vector<uint32_t> sampleRates = {192 * K, 256 * K, 800 * K, 1 * M, 1024 * K, 2 * M, 2400 * K, 3 * M, 3200 * K, 4 * M, 5 * M, 8 * M, 10 * M, 12 * M, 12500 * K, 15 * M, 16 * M, 20 * M,
                                            25 * M, 30 * M, 35 * M, 40 * M, 45 * M, 50 * M, 55 * M, 56 * M
};
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

class UHDSourceBlock : public pipeline::threaded_source_block {
public:

    explicit UHDSourceBlock(USRPDeviceCache* cache) : pipeline::threaded_source_block("UHD Source Block", ImColor(255, 0, 0)), cache(cache) {
        cache->refresh();
        sampleData = dsp::malloc<utils::sampleData>(1);
        stream = pipeline::createStream<utils::complex>();
        stream->auxData = sampleData;
        buf = dsp::malloc<uint8_t>(pipeline::BUFFER_COUNT);

        addInput("Frequency in", utils::frequencyType(), freq, [&](int) {
            if (freq && *freq != currentFreq) {
                setFrequency(*freq);
            }
        });
        cb = addOutput("IQ out", utils::complexStreamType(), stream, true);
    }

    ~UHDSourceBlock() {
        pipeline::deleteStream(stream);
        dsp::free(sampleData);
        dsp::free(buf);
    }

    void postConstruction() override {
        deviceIdx = -1;
        uhd_devices devs = cache->getDevices();
        for (int l = 0; l < devs.size(); ++l) {
            if (devs[l] == deviceSpec) {
                deviceIdx = l;
                break;
            }
        }
        if (deviceIdx <= 0) {
            deviceIdx = 0;
            setDevice(devs[deviceIdx], true);
            return;
        }
        setDevice(devs[deviceIdx], false);

        setMiscSettings(true);
    }

    void setDevice(const std::string& spec, bool fully) {
        bool restart = false;
        if (!hasStopped()) {
            stop();
            restart = true;
        }
        device = uhd::usrp::multi_usrp::make(spec);
        deviceSpec = spec;
        if (fully) {
            subDeviceSpec = device->get_rx_subdev_spec(MBOARD);
            setMiscSettings(true);
        }
        if (restart) {
            start();
        }
        auto bw = device->get_rx_bandwidth_range(CHANNEL);
        std::cout << bw.start() << " " << bw.stop() << " " << bw.step() << std::endl;
        auto bw2 = device->get_rx_freq_range(CHANNEL);
        std::cout << bw2.start() << " " << bw2.stop() << " " << bw2.step() << std::endl;
        std::cout << device->get_tree()->access<std::string>("/mboards/" + std::to_string(MBOARD) + "/name").get() << std::endl;
    }

    void setMiscSettings(bool changeDevice) {
        if (changeDevice) {
            dBoards = device->get_tree()->list("/mboards/" + std::to_string(MBOARD) + "/dboards/");
            std::string sDev[2];
            int idx = 0;
            std::istringstream f(subDeviceSpec.to_string());
            while (std::getline(f, sDev[idx], ':') && idx < 2) {
                idx++;
            }
            for (int i = 0; i < dBoards.size(); ++i) {
                if (dBoards[i] == sDev[0]) {
                    dBoardIdx = i;
                    break;
                }
            }
            radios = device->get_tree()->list("/mboards/" + std::to_string(MBOARD) + "/dboards/" + dBoards[dBoardIdx] + "/rx_frontends/");
            radioNames.clear();
            for (int i = 0; i < radios.size(); ++i) {
                radioNames.emplace_back(radios[i] + " (" + device->get_tree()->access<std::string>("/mboards/" + std::to_string(MBOARD) + "/dboards/" + dBoards[dBoardIdx] + "/rx_frontends/" + radios[i] + "/name").get() + ")");
                if (radios[i] == sDev[1]) {
                    radioIdx = i;
                }
            }
            device->set_rx_subdev_spec(uhd::usrp::subdev_spec_t(dBoards[dBoardIdx] + ":" + radios[radioIdx]), MBOARD);
            subDeviceSpec = device->get_rx_subdev_spec(MBOARD);

            clockSources = device->get_clock_sources(MBOARD);
            for (int i = 0; i < clockSources.size(); ++i) {
                if (clockSources[i] == clockSource) {
                    clockSourceIdx = i;
                    break;
                }
            }
            clockSource = clockSources[clockSourceIdx];

            antennas = device->get_rx_antennas(CHANNEL);
            for (int i = 0; i < antennas.size(); ++i) {
                if (antennas[i] == antenna) {
                    antennaIdx = i;
                    break;
                }
            }
            antenna = antennas[antennaIdx];


            for (int i = 0; i < sampleRates.size(); ++i) {
                if (sampleRates[i] == sampleRate) {
                    sampleRateIdx = i;
                    break;
                }
            }
            sampleRate = sampleRates[sampleRateIdx];


            gainRange = device->get_rx_gain_range(CHANNEL);
        }
        device->set_rx_antenna(antenna, CHANNEL);
        antenna = device->get_rx_antenna(CHANNEL);

        device->set_clock_source(clockSource, MBOARD);
        clockSource = device->get_clock_source(MBOARD);
    }

    void setFrequency(uint64_t newFreq) override {
        uhd::tune_request_t req((double) newFreq);
        device->set_rx_freq(req, CHANNEL);
        currentFreq = (uint64_t) device->get_rx_freq(CHANNEL);
        if (freq) {
            *freq = currentFreq;
        }
        sampleData->centerFreq = currentFreq;
        cb(1);
    }

    uint64_t getFrequency() override {
        return currentFreq;
    }

    void setSampleRate(uint32_t sampleRate_) override {
        device->set_rx_rate(sampleRate_, CHANNEL);
        device->set_rx_bandwidth(sampleRate_, CHANNEL);
        sampleRate = (uint32_t) device->get_rx_rate(CHANNEL);
        bandwidth = (uint32_t) device->get_rx_bandwidth(CHANNEL);
        sampleData->sampleRate = sampleRate;
        sampleData->bandwidth = bandwidth;
        cb(1);
    }

    uint32_t getSampleRate() override {
        return sampleRate;
    }

    uint32_t getBandwidth() override {
        return bandwidth;
    }

    void setGain(int gain_) override {
        device->set_rx_gain(gain_, CHANNEL);
        gain = (int) device->get_rx_gain(CHANNEL);
    }

    int getGain() override {
        return gain;
    }

    void start() override {
        device->set_rx_spp(pipeline::BUFFER_COUNT, CHANNEL);
        setSampleRate(sampleRate);
        setGain(gain);
        setFrequency(currentFreq);

        std::vector<std::string> sensor_names = device->get_tx_sensor_names(CHANNEL);
        if (std::find(sensor_names.begin(), sensor_names.end(), "lo_locked") != sensor_names.end()) {
            uhd::sensor_value_t lo_locked = device->get_tx_sensor("lo_locked", 0);
            std::cout << boost::format("Checking TX: %s ...") % lo_locked.to_pp_string()
                      << std::endl;
            UHD_ASSERT_THROW(lo_locked.to_bool());
        }

        uhd::stream_args_t stream_args("fc32", "sc16");
//        stream_args.args["spp"] = "1500";

        rx_stream = device->get_rx_stream(stream_args);
        uhd::stream_cmd_t cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
        cmd.num_samps = 2040;
        cmd.stream_now = true;
        cmd.time_spec = device->get_time_now() + uhd::time_spec_t(0.05);

        rx_stream->issue_stream_cmd(cmd);

        std::cout << "Samps: " << rx_stream->get_max_num_samps() << "  " << uhd::convert::get_bytes_per_item("fc32") << std::endl;

        stream->start();
        pipeline::threaded_source_block::start();
    }

    void loop() override {
        if (rx_stream) {
            stream->write([&](utils::complex* dat) {
                int totalSamps = 0;
                while (totalSamps + 2040 < pipeline::BUFFER_COUNT) {
                    size_t num_rx_samps;
                    {
                        std::lock_guard<std::mutex> raii(streamLock);
                        if (hasStopped() || !rx_stream) {
                            return 0;
                        }
//                    std::cout << "Recv: "  << std::endl;
                        num_rx_samps = rx_stream->recv(&dat[totalSamps], 2040, metaData, 3);
                    }
//                std::cout << "Samples: " << num_rx_samps << std::endl;
                    if (metaData.error_code == uhd::rx_metadata_t::ERROR_CODE_NONE) {
//                    std::cout << "NOERR: " << metaData.end_of_burst << " " << metaData.start_of_burst << " " << metaData.more_fragments << " " << metaData.fragment_offset << std::endl;
//                        return (int) (num_rx_samps);
                        totalSamps += (int) num_rx_samps;
                    } else {
                        std::cout << "Error code: " << metaData.error_code << " " << metaData.out_of_sequence << std::endl;

                    }
//                if (metaData.error_code == uhd::rx_metadata_t::ERROR_CODE_OVERFLOW) {
//                    std::cout << "Overflow error code: " << metaData.error_code << std::endl;
//                }
                }
//                std::cout << "Samps: " << totalSamps << std::endl;
//                stop();
                return totalSamps;
            });
        }
    }

    void stop() override {
        if (!hasStopped()) {
            {
                std::lock_guard<std::mutex> raii(streamLock);
                rx_stream->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
                rx_stream = nullptr;
            }
            stream->stop();
        }
        pipeline::threaded_source_block::stop();
    }

    void drawMiddle() override {
        ImGui::TextUnformatted(deviceSpec.c_str());
    }

    bool hasMenu() override {
        return true;
    }

    void drawMenu() override {
        ImGui::Text("Device:");
        ImGui::SameLine();
        uhd_devices devs = cache->getDevices();
        if (ImGui::Combo("##DeviceCombo", &deviceIdx, getDeviceName, &devs, (int) devs.size())) {
            setDevice(devs[deviceIdx], true);
        }
        ImGui::Text("Clock Source:");
        ImGui::SameLine();
        if (ImGui::Combo("##ClockSourceCombo", &clockSourceIdx, clockSources)) {
            clockSource = clockSources[clockSourceIdx];
            setMiscSettings(false);
        }
        ImGui::Text("Daughterboard:");
        ImGui::SameLine();
        if (ImGui::Combo("##DaughterBoardCombo", &dBoardIdx, dBoards)) {
            subDeviceSpec = uhd::usrp::subdev_spec_t(dBoards[dBoardIdx] + ":" + radios[radioIdx]);
            setMiscSettings(true);
        }
        ImGui::Text("Subdevice:");
        ImGui::SameLine();
        if (ImGui::Combo("##SubdeviceCombo", &radioIdx, radioNames)) {
            subDeviceSpec = uhd::usrp::subdev_spec_t(dBoards[dBoardIdx] + ":" + radios[radioIdx]);
            setMiscSettings(true);
        }
        ImGui::Text("Antenna:");
        ImGui::SameLine();
        if (ImGui::Combo("##AntennaCombo", &antennaIdx, antennas)) {
            antenna = antennas[antennaIdx];
            setMiscSettings(false);
        }
        ImGui::NewLine();
        if (ImGui::SliderInt("Gain", &gain, (int) gainRange.start(), (int) gainRange.stop())) {
            setGain(gain);
        }
        ImGui::Text("SampleRate:");
        ImGui::SameLine();
        if (ImGui::Combo("##SampleRateCombo", &sampleRateIdx, sampleRateStrings)) {
            sampleRate = sampleRates[sampleRateIdx];
            setSampleRate(sampleRate);
        }
    }

    static bool getDeviceName(void* data, int idx, const char** name) {
        auto dev = *((uhd_devices*) data);
        uhd::device_addr_t dat = dev[idx];
        *name = (dat.get("product") + "(" + dat.get("serial") + ")").c_str();
        return true;
    }

    void readJson(const nlohmann::json& json) override {
        deviceSpec = json["device"].get<std::string>();
        clockSource = json["clock"].get<std::string>();
        subDeviceSpec = json["subDev"].get<std::string>();
        antenna = json["antenna"].get<std::string>();

        sampleRate = json["rate"].get<uint32_t>();
        gain = json["gain"].get<int>();
    }

    void toJson(nlohmann::json& json) const override {
        json["device"] = deviceSpec;
        json["clock"] = clockSource;
        json["subDev"] = subDeviceSpec.to_string();
        json["antenna"] = antenna;

        json["rate"] = sampleRate;
        json["gain"] = gain;
    }

private:

    pipeline::connection_callback cb;
    pipeline::datastream<utils::complex>* stream;
    utils::sampleData* sampleData;
    uint8_t* buf;

    uint64_t* freq = nullptr;

    int deviceIdx = 0;
    USRPDeviceCache* cache;

    uhd::usrp::multi_usrp::sptr device = nullptr;
    uhd::rx_streamer::sptr rx_stream = nullptr;
    std::mutex streamLock;
    uhd::rx_metadata_t metaData;

    std::string deviceSpec, clockSource, antenna;
    uhd::usrp::subdev_spec_t subDeviceSpec = uhd::usrp::subdev_spec_t();
    std::vector<std::string> dBoards, radios, radioNames, clockSources, antennas;
    int dBoardIdx = 0, radioIdx = 0, clockSourceIdx = 0, antennaIdx = 0, sampleRateIdx = (int) sampleRates.size() / 2;

    uhd::gain_range_t gainRange;
    int gain = 0;
    uint32_t sampleRate = 1600000;
    uint32_t bandwidth = 1600000;
    uint64_t currentFreq = 0;

};

pipeline::source_block_ptr createUHDSource(USRPDeviceCache* cache) {
    return std::make_shared<UHDSourceBlock>(cache);
}