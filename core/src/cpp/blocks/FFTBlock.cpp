//
// Created by Elec332 on 25/02/2022.
//

#include <util/default_core_block.h>
#include <nativesdr/dsp/windows.h>
#include <nativesdr/util/chart_helper.h>
#include <nativesdr/dsp/fft.h>

class FFTBlock : public pipeline::threaded_block {

public:

    FFTBlock() : pipeline::threaded_block("FFT Block", ImColor(255, 0, 0)) {
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
                skip = (int) std::ceil((diff / sr) * drawSamples);
                if (skip % 2 != 0) {
                    skip++;
                }
                bandwidth = (drawSamples - skip) * iPerD;
            }
            auto hBw = std::abs(bandwidth / 2);
            ImVec2 start = ImGui::GetWindowPos();
            ImVec2 end = start + ImGui::GetWindowSize();
            double yStart = -100;
            ImVec2 ppu = ImGui::DrawChartFrame(start, end, yStart, 0, utils::ui::getDbScale, f - hBw, f + hBw, utils::ui::getFreqScale);
            ImGui::DrawChartLineFilled(start, end, &drawBuf[skip / 2], drawSamples - skip, ppu, yStart, utils::ui::BLUE, utils::ui::BLUE_F);
        };
        drawFuncRef = &drawFunc;
        addInput("IQ in", utils::complexStreamType(), stream);
        addOutput("Renderer", utils::uiType(), drawFuncRef, true);
        addInput("Frequency in", utils::frequencyType(), freq);

        iq = dsp::malloc<utils::complex>(pipeline::BUFFER_COUNT);
        fft = dsp::malloc<utils::complex>(pipeline::BUFFER_COUNT);
        psd = dsp::malloc<float>(pipeline::BUFFER_COUNT);
        drawBuf = dsp::malloc<float>(pipeline::BUFFER_COUNT);
        window = dsp::malloc<float>(pipeline::BUFFER_COUNT);
    }

    ~FFTBlock() {
        dsp::free(window);
        dsp::free(iq);
        dsp::free(fft);
        dsp::free(drawBuf);
        dsp::free(psd);
    }

    void loop() override {
        if (stream) {
            stream->read([&](const utils::complex* dat, int samples) {
                if (samples == 0) {
                    return;
                }
                if (lastSamples != samples) {
                    resetRoot(samples);
                }
                int factor = 2;
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
                    drawBuf[i] = (drawBuf[i] * (1 - attack)) + (max * attack);
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

    bool hasMenu() override {
        return true;
    }

    void drawMenu() override {
        ImGui::SliderFloat("FFT Attack", &attack, 0, 1);
        if (ImGui::Combo("##filter", &filter, "Square\0Hamming\0Hann\0")) {
            resetRoot(lastSamples);
        }
    }

    void resetRoot(int newSamples) {
        dsp::WindowFunction windowFunc = getWindow();
        for (int i = 0; i < newSamples; i++) {
            window[i] = i % 2 ? windowFunc(i, newSamples) : -windowFunc(i, newSamples);
        }
        if (lastSamples != newSamples) {
            plan = dsp::create_plan(newSamples, iq, fft, true);
            lastSamples = newSamples;
        }
    }

    [[nodiscard]] dsp::WindowFunction getWindow() const {
        switch (filter) {
            case 0:
                return dsp::squareWindow;
            case 1:
                return dsp::hammingWindow;
            case 2:
                return dsp::hannWindow;
            default:
                return dsp::squareWindow;
        }
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

    float attack = 0.2;
    int filter = 0;

};

pipeline::block_ptr createFFTBlock() {
    return std::make_shared<FFTBlock>();
}