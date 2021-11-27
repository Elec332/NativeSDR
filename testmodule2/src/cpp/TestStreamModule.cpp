//
// Created by Elec332 on 13/11/2021.
//

#include <TestModule.h>
#include <volk/volk.h>
#include <dsp/fft.h>
#include <dsp/iq_converter.h>
#include <dsp/malloc.h>
#include <dsp/windows.h>
#include <util/chart_helper.h>
#include "../../../core/src/headers/util/wav_reader.h"

class FFTTestBlock : public pipeline::threaded_block {

    int factor = 16;
    float PI = 3.14159265358979323846;

public:

    FFTTestBlock() : pipeline::threaded_block("FFT Block", ImColor(255, 0, 0)) {
        drawFunc = [&](size_t random) {
            double f = 0;
            if (freq) {
                f = (double) *freq;
            }
            double bandwidth = 100000;
            double sr = bandwidth;
            if (stream && stream->auxData) {
                f = (double) ((utils::sampleData*) stream->auxData)->centerFreq;
                bandwidth = ((utils::sampleData*) stream->auxData)->bandwidth;
                sr = ((utils::sampleData*) stream->auxData)->sampleRate;
            }
            int skip = 0;
            if (sr > bandwidth) {
                auto diff = sr - bandwidth;
                auto iPerD = sr / drawSamples;
                skip = (int) std::ceil((diff / bandwidth) * drawSamples);
                if (skip % 2 != 0) {
                    skip++;
                }
                bandwidth = (drawSamples - skip) * iPerD;
            }
            auto hBw = std::abs(bandwidth / 2);
            ImVec2 start = ImGui::GetWindowPos();
            ImVec2 end = start + ImGui::GetWindowSize();
            ImVec2 ppu = ImGui::DrawChartFrame(start, end, -100, 0, utils::ui::getDbScale, f - hBw, f + hBw, utils::ui::getFreqScale);

            ImGui::DrawChartLineFilled(start, end, drawBuf + (skip / 2), drawSamples - skip, ppu, -100, utils::ui::BLUE, utils::ui::BLUE_F, true);
        };
        drawFuncRef = &drawFunc;
        addInput("IQ in", utils::complexStreamType(), stream);
        addOutput("Renderer", utils::uiType(), drawFuncRef, true);
        addInput("Frequency in", utils::frequencyType(), freq);

        int samples = 1024 * 128;
        iq = dsp::malloc<utils::complex>(samples);
        fft = dsp::malloc<utils::complex>(samples);
        psd = dsp::malloc<float>(samples);
        drawBuf = dsp::malloc<float>(samples);
        window = dsp::malloc<float>(samples);
    }

    ~FFTTestBlock() {
        dsp::free(window);
        dsp::free(iq);
        dsp::free(fft);
        dsp::free(drawBuf);
        dsp::free(psd);
    }

    void loop() override {
        if (stream) {
            stream->read([&](utils::complex* dat, int samples) {
                dsp::WindowFunction windowFunc = dsp::squareWindow;
                if (lastSamples != samples) {
                    for (int i = 0; i < samples; i++) {
                        window[i] = windowFunc(i, samples);
                    }
                    plan = dsp::create_plan(samples, iq, fft, true);
                    lastSamples = samples;
                }
                volk_32fc_32f_multiply_32fc((lv_32fc_t*) iq, (lv_32fc_t*) dat, window, samples);
                plan->execute();
                volk_32fc_s32f_power_spectrum_32f(psd, (lv_32fc_t*) fft, (float) samples, samples);
                int offset = 0;
                for (int i = 0; i < samples / factor; ++i) {
                    float max = -INFINITY;
                    for (int j = 0; j < factor; ++j) {
                        if (psd[offset + j] > max) {
                            max = psd[offset + j];
                        }
                    }
                    drawBuf[i] = max;
                    offset += factor;
                }
                drawSamples = samples / factor;
            });
        } else {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void drawMiddle() override {
    }

private:

    pipeline::datastream<utils::complex>* stream = nullptr;
    uint64_t* freq = nullptr;
    utils::drawFunc drawFunc;
    utils::drawFunc* drawFuncRef;

    dsp::fft_plan_ptr plan;
    float* window;
    utils::complex* iq;
    utils::complex* fft;
    float* psd;
    float* drawBuf;
    int lastSamples = 0;
    int drawSamples = 0;

};

class FileStreamTestBlock : public pipeline::threaded_block {

    //std::string fileName = "D:/Downloads/15-29-07_92783258Hz.wav";
    std::string fileName = "D:/Downloads/15-39-37_91303258Hz.wav";

public:

    FileStreamTestBlock() : pipeline::threaded_block("File Block", ImColor(255, 0, 0)) {
        sampleData = dsp::malloc<utils::sampleData>(1);
        stream = pipeline::createStream<utils::complex>();
        stream->auxData = sampleData;
        fopen_s(&file, fileName.c_str(), "rb");
        data = readWAV(file);
        buf = dsp::malloc<int16_t>(1024 * 128);
        converter = dsp::getConverter(data->sampleData.bitsPerSample, data->sampleData.bitsPerSample != 8, data->sampleData.blockAlign);

        std::cout << "Center freq: " << data->centerFreq << std::endl;
        std::cout << "Length: " << data->length << std::endl;

        std::cout << "A Format: " << data->sampleData.AudioFormat << std::endl;
        std::cout << "Channels: " << data->sampleData.NumOfChan << std::endl;
        std::cout << "Samples Sec: " << data->sampleData.SamplesPerSec << std::endl;
        std::cout << "bpSample: " << data->sampleData.bitsPerSample << std::endl;
        std::cout << "allign: " << data->sampleData.blockAlign << std::endl;
        std::cout << "byperSample: " << data->sampleData.bytesPerSec << std::endl;
        sampleData->offset = 0;
        sampleData->bandwidth = data->sampleData.SamplesPerSec;
        sampleData->sampleRate = data->sampleData.SamplesPerSec;
        sampleData->centerFreq = data->centerFreq;

        addOutput("IQ out", utils::complexStreamType(), stream, true);
    }

    ~FileStreamTestBlock() {
        pipeline::deleteStream(stream);
        dsp::free(sampleData);
        fclose(file);
        data.reset();
        dsp::free(buf);
    }

    void loop() override {
        int samples = 1024 * 8;
        stream->write([&](utils::complex* dat) {
            fread(buf, 1, samples * 2 * sizeof(int16_t), file);
            converter(buf, dat, samples);
            return samples;
        });
        int base = 1000 * 1000;

        std::this_thread::sleep_for(std::chrono::microseconds((base * samples) / data->sampleData.SamplesPerSec));
    }

    void start() override {
        stream->start();
        pipeline::threaded_block::start();
    }

    void stop() override {
        stream->stop();
        pipeline::threaded_block::stop();
    }

    void drawMiddle() override {
        std::string str = "File: " + fileName;
        ImGui::TextUnformatted(str.c_str());
    }

private:

    pipeline::datastream<utils::complex>* stream;
    utils::sampleData* sampleData;
    FILE* file = nullptr; //Shut up clang...
    std::shared_ptr<file_data> data;
    dsp::IQConverter converter;

    int16_t* buf;

};

pipeline::block_ptr createStreamFileBlock() {
    return std::make_shared<FileStreamTestBlock>();
}

pipeline::block_ptr createFFTBlock() {
    return std::make_shared<FFTTestBlock>();
}