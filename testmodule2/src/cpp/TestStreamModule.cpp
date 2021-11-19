//
// Created by Elec332 on 13/11/2021.
//

#include <TestModule.h>
#include <volk/volk.h>
#include <dsp/fft.h>
#include <dsp/iq_converter.h>
#include <dsp/malloc.h>
#include "../../../core/src/headers/util/wav_reader.h"

class FFTTestBlock : public pipeline::threaded_block {

    int factor = 16;
    float PI = 3.14159265358979323846;

public:

    FFTTestBlock() : pipeline::threaded_block("FFT Block", ImColor(255, 0, 0)) {
        drawFunc = [&](size_t random) {
            std::string str = "##" + std::to_string(random);
            ImPlot::SetNextAxisToFit(ImAxis_X1);
            ImPlot::BeginPlot(str.c_str(), ImVec2(-1, 0), ImPlotFlags_NoInputs);
            ImPlot::SetupAxisLimits(ImAxis_Y1, -100, 0);
            if (stream != nullptr && stream->auxData != nullptr) {
                ImPlot::SetupAxisFormat(ImAxis_X1, formatTag, this);
            }
            ImPlot::PlotLine(str.c_str(), drawBuf, drawSamples);
            ImPlot::EndPlot();
        };
        drawFuncRef = &drawFunc;
        addInput("IQ in", utils::complexStreamType(), stream, [](int flags) {
            std::cout << "Change " << flags << std::endl;
        });
        addOutput("Renderer", utils::uiType(), drawFuncRef, true);

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

    static void formatTag(double value, char* buf, int len, void* data) {
        auto thiz = (FFTTestBlock*) data;
        auto sampleData = (utils::sampleData*) thiz->stream->auxData;
        double vpi = (double) sampleData->bandwidth / thiz->drawSamples;
        value -= (double) thiz->drawSamples / 2;
        double val = sampleData->centerFreq + (value * vpi);
        formatFrequency(val, buf, len);
    }

    static void formatFrequency(double freq, char* buf, int bufLen) {
        if (bufLen < 8) {
            buf[0] = 'X';
            return;
        }
        std::string postFix;
        double test = std::abs(freq);
        if (test > 1000000000) {
            postFix = "GHz";
            freq /= 1000000000;
        } else if (test > 1000000) {
            postFix = "MHz";
            freq /= 1000000;
        } else if (test > 1000) {
            postFix = "KHz";
            freq /= 1000;
        } else {
            postFix = "Hz";
        }
        postFix = "%.2f " + postFix;
        snprintf(buf, bufLen, postFix.c_str(), freq);
    }

    void loop() override {
        if (stream) {
            stream->read([&](utils::complex* dat, int samples) {
                volk_32fc_32f_multiply_32fc((lv_32fc_t*) iq, (lv_32fc_t*) dat, window, samples);
                if (lastSamples != samples) {
                    for (int i = 0; i < samples; i++) {
                        window[i] = (i % 2) ? 1 : -1;
//                      double alpha = 0.16;
//                      double a0 = (1.0f - alpha) / 2.0f;
//                      double a2 = alpha / 2.0f;
//                      double ret = a0 - (0.5f * cos(2.0f * PI * (i / (double) samples))) + (a2 * cos(4.0f * PI * (i / (double) samples)));
//                      window[i] = float((i % 2) ? ret : -ret) * 2;
                    }
                    plan = dsp::create_plan(samples, iq, fft, true);
                    lastSamples = samples;
                }
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

        std::this_thread::sleep_for(std::chrono::microseconds ((base * samples) / data->sampleData.SamplesPerSec));
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