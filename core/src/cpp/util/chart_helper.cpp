//
// Created by Elec332 on 20/11/2021.
//

#include <util/chart_helper.h>
#include <iostream>
#include <sstream>
#include <cmath>

std::string utils::ui::getDbScale(double num) {
    return std::to_string((int) num);
}

std::string utils::ui::getFreqScale(double num) {
    if (num == 0) {
        return "0";
    }
    std::string unit;
    auto aNum = std::abs(num);
    if (aNum >= 1000000000.0) {
        num /= 1000000000.0;
        unit = "GHz";
    } else if (aNum >= 1000000) {
        num /= 1000000;
        unit = "MHz";
    } else if (aNum >= 1000) {
        num /= 1000;
        unit = "KHz";
    } else {
        unit = "Hz";
    }
    aNum = std::abs(num);

    std::ostringstream oss;
    char str[15];

    {//TODO: My eyes
        auto f1 = (uint64_t) std::floor(aNum * 100000);
        int precision = 5;
        if (f1 % 100000 == 0) {
            precision = 0;
        } else if (f1 % 10000 == 0) {
            precision = 1;
        } else if (f1 % 1000 == 0) {
            precision = 2;
        } else if (f1 % 100 == 0) {
            precision = 3;
        } else if (f1 % 10 == 0) {
            precision = 4;
        }
        std::string fmt = "%." + std::to_string(precision) + "f";
        snprintf(str, 15, fmt.c_str(), num);
    }

    oss << str << " " << unit;
    return oss.str();
}
