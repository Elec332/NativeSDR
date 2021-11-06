//
// Created by Elec332 on 04/11/2021.
//

#ifndef NATIVESDR_MAIN_WINDOW_H
#define NATIVESDR_MAIN_WINDOW_H

#include <ui/editor_ui.h>
#include <ui/sdr_ui.h>
#include <pipeline/block/schematic.h>

namespace main_window {

    void init();

    void deinit();

    void start(pipeline::schematic* nodes);

}

#endif //NATIVESDR_MAIN_WINDOW_H
