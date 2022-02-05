//
// Created by Elec332 on 04/11/2021.
//

#ifndef NATIVESDR_EDITOR_UI_H
#define NATIVESDR_EDITOR_UI_H

#include <nativesdr/pipeline/block/schematic.h>

namespace editor_ui {

    void init(const std::string& rootDir);

    void draw(pipeline::schematic* nodes);

    void deinit();

}

#endif //NATIVESDR_EDITOR_UI_H
