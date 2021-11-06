//
// Created by Elec332 on 04/11/2021.
//

#include <dsp/iq_converter.h>
#include <volk/volk.h>
#include <iostream>
#include <dsp/fft.h>
#include "ui/sdr_ui.h"
#include "util/wav_reader.h"

int factor = 16;
bool filter = true;

class testclass {

    void update(FILE* file, file_data* data) const {
        fread(buf, 1, samples * 2 * sizeof(int16_t), file);
        dsp::IQConverter converter = dsp::getConverter(data->sampleData.bitsPerSample,
                                                       data->sampleData.bitsPerSample != 8,
                                                       data->sampleData.blockAlign);
        converter(buf, iq, samples);
//        auto* mean = (float*) volk_malloc(sizeof(float), volk_get_alignment());
//        auto* stddev = (float*) volk_malloc(sizeof(float), volk_get_alignment());
//
//        volk_32f_stddev_and_mean_32f_x2(stddev, mean, (float*) iq, samples * 2);
//        for (int i = 0; i < samples * 2; ++i) {
//            ((float*) iq)[i] -= *mean;
//        }
//
//        volk_free(mean);
//        volk_free(stddev);

        if (filter) volk_32fc_32f_multiply_32fc((lv_32fc_t*) iq2, (lv_32fc_t*) iq, window, samples);

        plan->execute();
        volk_32fc_s32f_power_spectrum_32f(psd, (lv_32fc_t*) iq_out, (float) samples, samples);

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
    }

    float PI = 3.14159265358979323846;

public:

    utils::complex* giq2() const {
        if (!filter)return iq;
        return (utils::complex*) volk_malloc(samples * sizeof(utils::complex), volk_get_alignment());
    }

    explicit testclass(const int samplez) :
            samples(samplez),
            buf((int16_t*) volk_malloc(samples * sizeof(int16_t) * 2, volk_get_alignment())),
            iq((utils::complex*) volk_malloc(samples * sizeof(utils::complex), volk_get_alignment())),
            iq2(giq2()),
            iq_out((utils::complex*) volk_malloc(samples * sizeof(utils::complex), volk_get_alignment())),
            psd((float*) volk_malloc(samples * sizeof(float), volk_get_alignment())),
            window((float*) volk_malloc(samples * sizeof(float), volk_get_alignment())),
            drawBuf((float*) volk_malloc((samples * sizeof(float)) / factor, volk_get_alignment())),
            plan(dsp::create_plan(samples, iq2, iq_out, true)) {
        for (int i = 0; i < samples; i++) {
//                window[i] = (i % 2) ? 1 : -1;
            double alpha = 0.16;
            double a0 = (1.0f - alpha) / 2.0f;
            double a2 = alpha / 2.0f;
            double ret = a0 - (0.5f * cos(2.0f * PI * (i / (double) samples))) + (a2 * cos(4.0f * PI * (i / (double) samples)));
            window[i] = float((i % 2) ? ret : -ret) * 2;
        }
    }

    ~testclass() {
        volk_free(buf);
        volk_free(iq);
        volk_free(iq2);
        volk_free(iq_out);
        volk_free(psd);
    }

    void draw(FILE* file, file_data* data) {
        if (counter % 5 == 0) {
            update(file, data);
        }
        counter++;
        ImPlot::BeginPlot("", ImVec2(-1, 0), ImPlotFlags_NoInputs);
        ImPlot::SetupAxisLimits(ImAxis_Y1, -100, 0);
        ImPlot::PlotLine("", drawBuf, samples / factor);
        ImPlot::EndPlot();
    }

    int counter = 0;
    int samples;
    int16_t* buf;
    utils::complex* iq;
    utils::complex* iq2;
    utils::complex* iq_out;
    float* psd;
    float* window;
    float* drawBuf;
    dsp::fft_plan_ptr plan;

};

testclass* tc;
FILE* file;
std::shared_ptr<file_data> data;

void sdr_ui::init() {
    int samples = 1024 * 16;
    tc = new testclass(samples);

    //std::string fileName = "D:/Downloads/ecars_net_7255_HDSDR_20180225_174354Z_7255kHz_RF.wav";
    //std::string fileName = "D:/Downloads/15-29-07_92783258Hz.wav";
    std::string fileName = "D:/Downloads/15-39-37_91303258Hz.wav";
    fopen_s(&file, fileName.c_str(), "rb");
    data = readWAV(file);

    std::cout << "Center freq: " << data->centerFreq << std::endl;
    std::cout << "Length: " << data->length << std::endl;

    std::cout << "A Format: " << data->sampleData.AudioFormat << std::endl;
    std::cout << "Channels: " << data->sampleData.NumOfChan << std::endl;
    std::cout << "Samples Sec: " << data->sampleData.SamplesPerSec << std::endl;
    std::cout << "bpSample: " << data->sampleData.bitsPerSample << std::endl;
    std::cout << "allign: " << data->sampleData.blockAlign << std::endl;
    std::cout << "byperSample: " << data->sampleData.bytesPerSec << std::endl;
}

void sdr_ui::draw() {
    tc->draw(file, data.get());
}

void sdr_ui::deinit() {
    fclose(file);
    data.reset();
    delete tc;
}

