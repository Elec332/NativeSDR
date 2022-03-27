//
// Created by Elec332 on 04/11/2021.
//

#ifndef NATIVESDR_MAIN_WINDOW_H
#define NATIVESDR_MAIN_WINDOW_H

#include <ui/editor_ui.h>
#include <ui/sdr_ui.h>
#include <nativesdr/pipeline/block/schematic.h>

#define FREQUENCY_NUMBERS 12

class main_window {

public:

    void init(const std::string& rootDir);

    void deInit();

    void start(pipeline::schematic** nodes);

    pipeline::block_ptr createFrequencyBlock();

    void drawWindow(pipeline::schematic** nm);

    NativeGraphics* getBackend();

    pipeline::connection_callback callback = nullptr;

private:

    void runCallBack();

    void checkFrequency();

    std::shared_ptr<SubContext> getSubContext();

    void drawFreqChooser();

    void drawTopRow(pipeline::schematic* nm);

    bool showEditor = false;
    uint64_t oldFrequency = 0;
//uint64_t frequency = 99109674;
    uint64_t frequency = 95300000;
    uint64_t* freqRef = &frequency;
    uint8_t drawer[FREQUENCY_NUMBERS];
    ImFont* big;

    NativeGraphics context;
    editor_ui editor;
    sdr_ui sdr;

    std::shared_ptr<SubContext> ctx;

    pipeline::block_ptr mainFB;

};

#endif //NATIVESDR_MAIN_WINDOW_H
