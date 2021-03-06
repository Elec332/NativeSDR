//
// Created by Elec332 on 22/10/2021.
//

#include <nativesdr/NativeSDRGraphics.h>
#include <impl/util/object_types_internal.h>

#include <utility>
#include <nativesdr/util/object_type.h>

#include "subinit.h"

template<class T>
class stream_type : public simple_type<pipeline::datastream<T>> {

public:

    stream_type(std::string name, void(* drawer)(bool)) noexcept : simple_type<pipeline::datastream<T>>(std::move(name), drawer) {
    }

    void setConnectionCount(void* ref, size_t count) const override {
        ((pipeline::datastream<T>*) ref)->setReaders(count);
    }

};

void drawComplexStream(bool connected) {
    ax::Widgets::Icon(ImVec2(24, 24), ax::Drawing::IconType::Flow, connected, ImColor(255, 255, 255), ImColor(32, 32, 32, (int) (ImGui::GetStyle().Alpha * 255)));
}

void drawDataStream(bool connected) {
    ax::Widgets::Icon(ImVec2(24, 24), ax::Drawing::IconType::Flow, connected, ImColor(255, 0, 0), ImColor(32, 32, 32, (int) (ImGui::GetStyle().Alpha * 255)));
}

void drawSampleDataType(bool connected) {
    ax::Widgets::Icon(ImVec2(24, 24), ax::Drawing::IconType::Circle, connected, ImColor(0, 0, 255), ImColor(32, 32, 32, (int) (ImGui::GetStyle().Alpha * 255)));
}

void drawStringType(bool connected) {
    ax::Widgets::Icon(ImVec2(24, 24), ax::Drawing::IconType::Circle, connected, ImColor(255, 0, 0), ImColor(32, 32, 32, (int) (ImGui::GetStyle().Alpha * 255)));
}

void drawUIType(bool connected) {
    ax::Widgets::Icon(ImVec2(24, 24), ax::Drawing::IconType::Grid, connected, ImColor(255, 0, 0), ImColor(32, 32, 32, (int) (ImGui::GetStyle().Alpha * 255)));
}

void drawFrequencyType(bool connected) {
    ax::Widgets::Icon(ImVec2(24, 24), ax::Drawing::IconType::RoundSquare, connected, ImColor(255, 255, 0), ImColor(32, 32, 32, (int) (ImGui::GetStyle().Alpha * 255)));
}

void drawNumberType(bool connected) {
    ax::Widgets::Icon(ImVec2(24, 24), ax::Drawing::IconType::Square, connected, ImColor(128, 128, 128), ImColor(32, 32, 32, (int) (ImGui::GetStyle().Alpha * 255)));
}

void drawAudioStream(bool connected) {
    ax::Widgets::Icon(ImVec2(24, 24), ax::Drawing::IconType::Flow, connected, ImColor(0, 255, 255), ImColor(32, 32, 32, (int) (ImGui::GetStyle().Alpha * 255)));
}

void init_object_types() {
}

stream_type<utils::complex> complex_stream("Complex Stream", drawComplexStream);
stream_type<uint8_t> data_stream("Data Stream", drawComplexStream);
simple_type<utils::sampleData> sample_data("SDR Data", drawComplexStream);
simple_type<std::string> string_data("String", drawStringType);
simple_type<utils::drawFunc> ui("UI", drawUIType);
simple_type<uint64_t> freq("Frequency", drawFrequencyType);
simple_type<int> number("Number", drawNumberType);
stream_type<utils::audio> audio_stream("Audio Stream", drawComplexStream);

const utils::object_type<pipeline::datastream<utils::complex>>* utils::complexStreamType() {
    return &complex_stream;
}

const utils::object_type<pipeline::datastream<uint8_t>>* utils::dataStreamType() {
    return &data_stream;
}

const utils::object_type<utils::sampleData>* utils::sampleDataType() {
    return &sample_data;
}

const utils::object_type<std::string>* utils::stringType() {
    return &string_data;
}

const utils::object_type<utils::drawFunc>* utils::uiType() {
    return &ui;
}

const utils::object_type<uint64_t>* utils::frequencyType() {
    return &freq;
}

const utils::object_type<int>* utils::numberType() {
    return &number;
}

const utils::object_type<pipeline::datastream<utils::audio>>* utils::audioStreamType() {
    return &audio_stream;
}
