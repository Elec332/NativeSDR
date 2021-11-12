//
// Created by Elec332 on 22/10/2021.
//

#include <NativeSDRGraphics.h>
#include <impl/util/object_types_internal.h>

#include <utility>
#include "subinit.h"

template<class T>
class stream_type : public simple_type<pipeline::datastream<T>> {

public:

    stream_type(std::string name, void(* drawer)(bool)) noexcept: simple_type(std::move(name), drawer) {
    }

    void setConnectionCount(void* ref, size_t count) const override {
        ((pipeline::datastream<T>*) ref)->setReaders(count);
    }

};

void drawComplexStream(bool connected) {
    ax::Widgets::Icon(ImVec2(24, 24), ax::Drawing::IconType::Flow, connected, ImColor(255, 255, 255),
                      ImColor(32, 32, 32, (int) (ImGui::GetStyle().Alpha * 255)));
}

void drawDataStream(bool connected) {
    ax::Widgets::Icon(ImVec2(24, 24), ax::Drawing::IconType::Flow, connected, ImColor(255, 0, 0),
                      ImColor(32, 32, 32, (int) (ImGui::GetStyle().Alpha * 255)));
}

void drawSampleDataType(bool connected) {
    ax::Widgets::Icon(ImVec2(24, 24), ax::Drawing::IconType::Circle, connected, ImColor(0, 0, 255),
                      ImColor(32, 32, 32, (int) (ImGui::GetStyle().Alpha * 255)));
}

void drawStringType(bool connected) {
    ax::Widgets::Icon(ImVec2(24, 24), ax::Drawing::IconType::Circle, connected, ImColor(255, 0, 0),
                      ImColor(32, 32, 32, (int) (ImGui::GetStyle().Alpha * 255)));
}

void init_object_types() {
}

stream_type<utils::complex> complex_stream("Complex Stream", drawComplexStream);
stream_type<uint8_t> data_stream("Data Stream", drawComplexStream);
simple_type<utils::sampleData> sample_data("SDR Data", drawComplexStream);
simple_type<std::string> string_data("String", drawStringType);

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
