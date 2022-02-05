//
// Created by Elec332 on 04/11/2021.
//

#ifndef NATIVESDR_SDR_UI_H
#define NATIVESDR_SDR_UI_H

#include <nativesdr/NativeSDRGraphics.h>
#include <nativesdr/pipeline/block/block.h>
#include <nativesdr/pipeline/block/schematic.h>

namespace sdr_ui {

    void init();

    void draw(pipeline::schematic* nodes);

    void deinit();

    pipeline::block_ptr createUIBlock();

}

#endif //NATIVESDR_SDR_UI_H
