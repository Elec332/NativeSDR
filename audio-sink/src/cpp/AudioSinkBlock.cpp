//
// Created by Elec332 on 09/12/2021.
//

#include <AudioModule.h>
#include <portaudio.h>
#include <dsp/malloc.h>
#include <set>
#include <utility>
#include <dsp/dsp.h>

#define DEFAULT_SAMPLE_RATE 48000
static const int defaultSampleRates[] = {
        44100, DEFAULT_SAMPLE_RATE, 88200, 96000, 176400, 192000
};
static const int blockSize = 512;
static const int buffer_size = pipeline::BUFFER_COUNT * 3;

class AudioOutput {

public:

    AudioOutput(PaStreamParameters ao, std::string apiType, std::string name) : outputParameters(ao), apiType(std::move(apiType)), name(std::move(name)) {
        std::string sr;
        for (const auto& i: defaultSampleRates) {
            if (Pa_IsFormatSupported(nullptr, &outputParameters, i) == paFormatIsSupported) {
                sr += std::to_string(i);
                sr += '\0';
                sampleRates_.emplace_back(i);
            }
        }
        sampleRates = sr;
    }

    [[nodiscard]] bool isStereo() const {
        return outputParameters.channelCount > 1;
    }

    [[nodiscard]] std::string getSampleRates() const {
        return sampleRates;
    }

    [[nodiscard]] int getSampleRate(int index) const {
        return sampleRates_[index];
    }

    void setIndex(int& index, int& sampleRate) const {
        int k48 = -1;
        for (int i = 0; i < sampleRates_.size(); ++i) {
            if (sampleRates_[i] == DEFAULT_SAMPLE_RATE) {
                k48 = i;
            }
            if (sampleRates_[i] == sampleRate) {
                index = i;
                return;
            }
        }
        if (k48 >= 0) {
            index = k48;
        } else {
            index = 0;
        }
        sampleRate = sampleRates_[index];
    }

    [[nodiscard]] bool isValid() const {
        return !sampleRates_.empty();
    }

    const PaStreamParameters outputParameters;
    const std::string apiType;
    const std::string name;

private:
    std::string sampleRates;
    std::vector<int> sampleRates_;

};

class AudioOutputBlock : public pipeline::threaded_block {

public:

    AudioOutputBlock() : pipeline::threaded_block("Audio Sink", ImColor(255, 0, 0)) {
        checkDevices();
        buffer = dsp::malloc<utils::audio>(buffer_size);

        addInput("Audio in", utils::audioStreamType(), stream);
        ocb = addOutput("Audio Sample Rate", utils::numberType(), srRef, true);
    }

    ~AudioOutputBlock() {
        dsp::free(buffer);
    }

    void start() override {
        AudioOutput output = outputs[apiTypes[apiIndex]][outputIndex];
        Pa_OpenStream(&paStream, nullptr, &output.outputParameters, sampleRate, blockSize, paNoFlag, output.isStereo() ? stereo : mono, this);
        Pa_StartStream(paStream);
        pipeline::threaded_block::start();
    }

    void stop() override {
        Pa_AbortStream(paStream);
        Pa_CloseStream(paStream);
        lengthWait.notify_all();
        pipeline::threaded_block::stop();
        indexR = 0;
        indexW = 0;
        bufferLength = 0;
    }

    void loop() override {
        if (stream) {
            stream->read([&](const utils::audio* dat, int samples) {
                {
                    std::unique_lock<std::mutex> raii(indexMutex);
                    lengthWait.wait(raii, [&] {
                        return bufferLength + samples < buffer_size || hasStopped();
                    });
                    if (hasStopped()) {
                        return;
                    }
                    auto buf = &buffer[indexW];
                    auto diff = std::min(buffer_size - indexW, (unsigned long) samples);
                    memcpy(buf, dat, diff * sizeof(utils::audio));
                    indexW += diff;
                    if (diff < samples) {
                        memcpy(buffer, &dat[diff], (samples - diff) * sizeof(utils::audio));
                        indexW = samples - diff;
                    }
                    bufferLength += samples;
                }
                lengthWait.notify_all();
            });
        } else {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void drawMiddle() override {
    }

    bool hasMenu() override {
        return true;
    }

    void drawMenu() override {
        bool restart = false;
        if (ImGui::Combo("##api_sel", &apiIndex, apiTypeString.c_str())) {
            setDevice();
            restart = true;
        }
        const auto& apiType = apiTypes[apiIndex];
        int oldSR = sampleRate;
        if (ImGui::Combo("##dev_sel", &outputIndex, hostTypeStrings[apiIndex].c_str())) {
            outputName = outputs[apiType][outputIndex].name;
            outputs[apiType][outputIndex].setIndex(srIndex, sampleRate);
            restart = true;
        }
        if (ImGui::Combo("##sr", &srIndex, outputs[apiType][outputIndex].getSampleRates().c_str())) {
            sampleRate = outputs[apiType][outputIndex].getSampleRate(srIndex);
            restart = true;
        }
        if (sampleRate != oldSR) {
            ocb(1);
        }
        if (restart) {
            restartBlock();
        }
    }

    void toJson(nlohmann::json& json) const override {
        json["apiName"] = apiTypes[apiIndex];
        json["outputName"] = outputName;
        json["sampleRate"] = sampleRate;
    }

    void readJson(const nlohmann::json& json) override {
        std::string apiName = json["apiName"].get<std::string>();
        outputName = json["outputName"].get<std::string>();
        int oldSR = sampleRate;
        sampleRate = json["sampleRate"].get<int>();
        if (sampleRate != oldSR) {
            ocb(1);
        }

        apiIndex = 0;
        for (int i = 0; i < apiTypes.size(); ++i) {
            if (apiTypes[i] == apiName) {
                apiIndex = i;
                break;
            }
        }
        setDevice();
    }

private:

    void restartBlock() {
        if (!hasStopped()) {
            stop();
            start();
        }
    }

    void setDevice() {
        bool s = false;
        std::string apiType = apiTypes[apiIndex];
        for (int i = 0; i < outputs[apiType].size(); ++i) {
            if (outputName == outputs[apiType][i].name) {
                outputIndex = i;
                s = true;
                break;
            }
        }
        if (!s) {
            outputIndex = 0;
            outputName = outputs[apiType][outputIndex].name;
        }
        int oldSR = sampleRate;
        outputs[apiType][outputIndex].setIndex(srIndex, sampleRate);
        if (sampleRate != oldSR) {
            ocb(1);
        }
    }

    void checkDevices() {
        apiTypeString = "";
        apiTypes.clear();
        hostTypeStrings.clear();
        outputs.clear();

        const PaDeviceInfo* deviceInfo;
        const PaHostApiInfo* hostDeviceInfo;
        int devices = Pa_GetDeviceCount();
        std::set<PaHostApiTypeId> types;
        for (int i = 0; i < devices; ++i) {
            deviceInfo = Pa_GetDeviceInfo(i);
            hostDeviceInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
            PaHostApiTypeId apiType = hostDeviceInfo->type;
            PaStreamParameters outputParameters;
            outputParameters.device = i;
            outputParameters.sampleFormat = paFloat32;
            outputParameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
            outputParameters.channelCount = std::min<int>(deviceInfo->maxOutputChannels, 2);
            outputParameters.hostApiSpecificStreamInfo = nullptr;
            AudioOutput ao(outputParameters, hostDeviceInfo->name, deviceInfo->name);
            if (deviceInfo->maxOutputChannels <= 0 || apiType == paMME || apiType == paWDMKS || !ao.isValid()) {
                continue;
            }
            size_t size = types.size();
            types.insert(apiType);
            std::string apiName_ = hostDeviceInfo->name;
            if (size != types.size()) {
                apiTypeString += hostDeviceInfo->name;
                apiTypeString += '\0';
                apiTypes.emplace_back(apiName_);
            }
            int ai = 0;
            for (; ai < apiTypes.size(); ++ai) {
                if (apiTypes[ai] == apiName_) {
                    break;
                }
            }
            outputs[apiName_].emplace_back(ao);
            while (hostTypeStrings.size() <= ai) {
                hostTypeStrings.emplace_back();
            }
            hostTypeStrings[ai] += deviceInfo->name;
            hostTypeStrings[ai] += '\0';
        }
    }

    static int stereo(const void* input, void* output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
        return ((AudioOutputBlock*) userData)->audio(output, frameCount, stereoCopy);
    }

    static int mono(const void* input, void* output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
        return ((AudioOutputBlock*) userData)->audio(output, frameCount, monoCopy);
    }

    int audio(void* output, unsigned long frameCount, void(* cpyFunc)(float*, utils::audio*, const unsigned long, const unsigned long)) {
        {
            std::unique_lock<std::mutex> raii(indexMutex);
            lengthWait.wait(raii, [&] {
                return bufferLength > frameCount;
            });
            auto buf = &buffer[indexR];
            auto diff = std::min(buffer_size - indexR, frameCount);
            auto out = (float*) output;

            cpyFunc(out, buf, 0, diff);
            indexR += diff;
            if (diff < frameCount) {
                cpyFunc(out, buffer, diff, frameCount - diff);
                indexR = frameCount - diff;
            }
            bufferLength -= frameCount;
        }
        lengthWait.notify_all();
        return 0;
    }

    static void stereoCopy(float* out, utils::audio* in, const unsigned long offset, const unsigned long frameCount) {
        out = &out[offset * 2];
        memcpy(out, in, frameCount * sizeof(utils::audio));
    }

    static void monoCopy(float* out, utils::audio* in, const unsigned long offset, const unsigned long frameCount) {
        out = &out[offset];
        for (int i = 0; i < frameCount; ++i) {
            out[i] = (in[i].left + in[i].right) * 0.5f;
        }
    }

    utils::audio* buffer;
    unsigned long indexR = 0, indexW = 0;
    unsigned long bufferLength = 0;
    std::mutex indexMutex;
    std::condition_variable lengthWait{};
    pipeline::datastream<utils::audio>* stream = nullptr;

    PaStream* paStream = nullptr;

    int apiIndex = 0;
    std::string apiTypeString;
    std::vector<std::string> apiTypes;

    int outputIndex = 0;
    std::string outputName;
    std::vector<std::string> hostTypeStrings;
    std::map<std::string, std::vector<AudioOutput>> outputs;

    int srIndex = 0;
    int sampleRate = DEFAULT_SAMPLE_RATE;
    int* srRef = &sampleRate;
    pipeline::connection_callback ocb;

};

pipeline::block_ptr createAudioSinkBlock() {
    return std::make_shared<AudioOutputBlock>();
}
