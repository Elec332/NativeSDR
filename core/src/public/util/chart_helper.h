//
// Created by Elec332 on 20/11/2021.
//

#ifndef NATIVESDR_CHART_HELPER_H
#define NATIVESDR_CHART_HELPER_H

#include <nativesdr_core_export.h>
#include <NativeSDRGraphics.h>

namespace utils::ui {

    static ImU32 BLUE = IM_COL32(0, 0, 255, 255);
    static ImU32 BLUE_F = IM_COL32(0, 0, 255, 64);

    NATIVESDR_CORE_EXPORT std::string getDbScale(double num);

    NATIVESDR_CORE_EXPORT std::string getFreqScale(double num);
}

#endif //NATIVESDR_CHART_HELPER_H
