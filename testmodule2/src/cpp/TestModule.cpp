//
// Created by Elec332 on 10/07/2021.
//

#include <module/SDRModule.h>
#include <iostream>
#include <testbase.h>
#include <NativeSDRGraphics.h>
#include "drawtest.h"

void drawmain() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowSize(io.DisplaySize);
    //std::cout << io.DisplaySize.x << "  " << io.DisplaySize.y << std::endl;
    ImGui::Begin("Content", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings |
                 ImGuiWindowFlags_NoBringToFrontOnFocus);
    draw();

    ImGui::End();
}

class Test : public ModuleInstance {

    void test() override {
        std::cout << getTest() << std::endl;
        std::cout << "-------MOD2---------" << std::endl;
        init();
        NativeGraphics::startMainWindow(drawmain);
        deinit();
    }

};


void initModule() {
}

void onShutdownModule() {
}

ModuleInstance* createModuleContainer() {
    return new Test();
}

void destroyModuleContainer(ModuleInstance* instance) {
    delete (Test*) instance;
}
