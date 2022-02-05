//
// Created by Elec332 on 13/01/2022.
//

#ifndef NATIVESDR_TIMER_H
#define NATIVESDR_TIMER_H

#include <chrono>

class Timer {

public:

    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    void stop() {
        end_time = std::chrono::high_resolution_clock::now();
    }

    void stop(int div) {
        stop();
        double nd = 1.0 / div;
        history *= 1 - nd;
        history += get_timing() * nd;
    }

    [[nodiscard]] double get_timing() const {
        return std::chrono::nanoseconds(end_time - start_time).count() * 1e-6;
    }

    [[nodiscard]] double getHistory() const {
        return history;
    }

private:

    double history = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time, end_time;

};

#endif //NATIVESDR_TIMER_H
