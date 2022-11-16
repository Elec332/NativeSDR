//
// Created by Elec332 on 04/11/2021.
//

#ifndef NATIVESDR_MAIN_WINDOW_H
#define NATIVESDR_MAIN_WINDOW_H

#include <ui/editor_ui.h>
#include <ui/sdr_ui.h>
#include <nativesdr/pipeline/block/schematic.h>
#include <nativesdr/core_actions.h>

#define FREQUENCY_NUMBERS 12

class SDRMainWindow {

public:

    virtual void init(const std::string& rootDir) = 0;

    virtual void deInit() = 0;

    virtual void start(pipeline::schematic** nodes, SDRCoreActions* actions) = 0;

    virtual void drawWindow(pipeline::schematic** nm, SDRCoreActions* actions) = 0;

    virtual NativeGraphics* getBackend() = 0;

    virtual pipeline::block_ptr createFrequencyBlock() = 0;

};

std::shared_ptr<SDRMainWindow> createMainWindow();

#endif //NATIVESDR_MAIN_WINDOW_H
