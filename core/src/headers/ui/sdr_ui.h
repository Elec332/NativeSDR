//
// Created by Elec332 on 04/11/2021.
//

#ifndef NATIVESDR_SDR_UI_H
#define NATIVESDR_SDR_UI_H

#include <nativesdr/NativeSDRGraphics.h>
#include <nativesdr/pipeline/block/block.h>
#include <nativesdr/pipeline/block/schematic.h>

class sdr_ui {

public:

    void init();

    void draw(pipeline::schematic* nodes, NativeGraphics& context);

    void deinit();

    static pipeline::block_ptr createUIBlock();

private:

    bool showMenu = true;
    float size = 0.2;

};

#endif //NATIVESDR_SDR_UI_H
