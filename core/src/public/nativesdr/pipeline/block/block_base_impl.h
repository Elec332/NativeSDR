//
// Created by Elec332 on 13/11/2021.
//

#ifndef NATIVESDR_BLOCK_BASE_IMPL_H
#define NATIVESDR_BLOCK_BASE_IMPL_H

#include <nativesdr/pipeline/block/block.h>

#include <utility>
#include <thread>

namespace pipeline {

    class threaded_block : public block {

    public:

        threaded_block(std::string name, ImColor color) : pipeline::block(std::move(name), color) {
        }

        void start() override {
            stopped = false;
            thread = std::thread([&]() {
                while (true) {
                    if (hasStopped()) {
                        return;
                    }
                    loop();
                }
            });
        }

        void stop() override {
            stopped = true;
            if (std::this_thread::get_id() != thread.get_id()) {
                thread.join(); //Cannot join itself
            }
        }

        [[nodiscard]] bool hasStopped() const {
            return stopped;
        }

    private:

        virtual void loop() = 0;

        std::thread thread;
        bool stopped = true;

    };

    class threaded_source_block : public source_block {

    public:

        threaded_source_block(std::string name, ImColor color) : pipeline::source_block(std::move(name), color) {
        }

        void start() override {
            stopped = false;
            thread = std::thread([&]() {
                while (true) {
                    if (hasStopped()) {
                        return;
                    }
                    loop();
                }
            });
        }

        void stop() override {
            stopped = true;
            if (std::this_thread::get_id() != thread.get_id()) {
                thread.join(); //Cannot join itself
            }
        }

        [[nodiscard]] bool hasStopped() const {
            return stopped;
        }

    private:

        virtual void loop() = 0;

        std::thread thread;
        bool stopped = true;

    };

}

#endif //NATIVESDR_BLOCK_BASE_IMPL_H
