//
// Created by Elec332 on 04/11/2021.
//

#ifndef NATIVESDR_EDITOR_UI_H
#define NATIVESDR_EDITOR_UI_H

#include <nativesdr/pipeline/block/schematic.h>

class editor_ui {

public:

    void init(const std::string& rootDir);

    void draw(pipeline::schematic* nodes);

    void deinit();

private:


    ImTextureID background = nullptr;
    bool showPinId = false;

};

#endif //NATIVESDR_EDITOR_UI_H
