//
// Created by Elec332 on 25/02/2022.
//
#include <nativesdr/file-source_block.h>
#include <nativesdr/pipeline/block/block_base_impl.h>
#include <nativesdr/dsp/iq_converter.h>
#include <nativesdr/dsp/malloc.h>
#include <filesystem>

#include "../../../core/src/headers/util/wav_reader.h"

class FileSourceBlock : public pipeline::threaded_block {

public:

    FileSourceBlock() : pipeline::threaded_block("File Block", ImColor(255, 0, 0)) {
        sampleData = dsp::malloc<utils::sampleData>(1);
        memset(sampleData, 0, sizeof(utils::sampleData));
        stream = pipeline::createStream<utils::complex>();
        stream->auxData = sampleData;
        buf = dsp::malloc<int16_t>(pipeline::BUFFER_COUNT * 2);

        addOutput("IQ out", utils::complexStreamType(), stream, true);
    }

    ~FileSourceBlock() {
        pipeline::deleteStream(stream);
        dsp::free(sampleData);
        dsp::free(buf);
        if (file) {
            fclose(file);
        }
    }

    void openFile() {
        if (file) {
            fclose(file);
        }
        if (fileName.empty() || !std::filesystem::exists(fileName)) {
            return;
        }
        file = fopen(fileName.c_str(), "rb");
        data = readWAV(file);
        startPos = ftell(file);
        if (data->sampleData.NumOfChan != 2) {
            error = true;
            fclose(file);
            data = nullptr;
            return;
        }
        converter = dsp::getConverter(data->sampleData.bitsPerSample, data->sampleData.bitsPerSample != 8, data->sampleData.blockAlign);
        bitDepth = data->sampleData.bitsPerSample;
        sampleData->bandwidth = data->sampleData.SamplesPerSec;
        sampleData->sampleRate = data->sampleData.SamplesPerSec;
        sampleData->centerFreq = data->centerFreq;
    }

    bool hasMenu() override {
        return true;
    }

    void drawMenu() override {
        if (ImGui::Button("Select file")) {
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".wav", fileDir);
        }
        ImGui::Text("File: %s", std::filesystem::path(fileName).filename().string().c_str());
        ImGui::TextUnformatted("");
        ImGui::Text("SampleRate: %u sps", sampleData->sampleRate);
        ImGui::Text("Depth: %hu bits", bitDepth);

        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse, ImVec2(500, 300))) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                fileDir = ImGuiFileDialog::Instance()->GetCurrentPath();
                fileName = ImGuiFileDialog::Instance()->GetFilePathName();
                openFile();
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }

    void drawDialogs() override {
        if (ImGui::BeginPopupModal("Wav Error", &error, ImGuiWindowFlags_NoScrollbar)) {
            ImGui::Text("Invalid or corrupted WAV-file!:\n %s", fileName.c_str());
            ImGui::EndPopup();
        }
        if (error) {
            ImGui::OpenPopup("Wav Error");
        }
    }

    void loop() override {
        if (!data) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return;
        }
        int samples = 1024 * 128;
        int elementSize = sizeof(int16_t) * 2;
        stream->write([&](utils::complex* dat) {
            auto len = fread(buf, elementSize, samples, file);
            if (len < samples) {
                fseek(file, startPos, SEEK_SET);
                fread(&buf[len], elementSize, samples - len, file);
            }
            converter(buf, dat, samples);
            return samples;
        });
//        double base = 1000 * 1000 * 1000;
//        std::this_thread::sleep_for(std::chrono::nanoseconds((int) ((base * samples) / data->sampleData.SamplesPerSec)));
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
        if (data) {
            std::string str = "File: " + fileName;
            ImGui::TextUnformatted(str.c_str());
        }
    }

    void readJson(const nlohmann::json& json) override {
        fileName = json["file"].get<std::string>();
        fileDir = json["dir"].get<std::string>();
        bool restart = !hasStopped();
        if (restart) {
            stop();
        }
        openFile();
        if (restart) {
            start();
        }
    }

    void toJson(nlohmann::json& json) const override {
        json["file"] = fileName;
        json["dir"] = fileDir;
    }

private:

    bool error = false;
    std::string fileName, fileDir = ".";
    uint16_t bitDepth = 0;

    pipeline::datastream<utils::complex>* stream;
    utils::sampleData* sampleData;
    FILE* file = nullptr; //Shut up clang...
    long startPos = 0;
    std::shared_ptr<file_data> data;
    dsp::IQConverter converter = nullptr;

    int16_t* buf;

};

pipeline::block_ptr createFileSource() {
    return std::make_shared<FileSourceBlock>();
}
