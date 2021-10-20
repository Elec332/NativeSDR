//
// Created by Elec332 on 10/07/2021.
//

#include <module/SDRModule.h>
#include <iostream>
#include <testbase.h>

class Test : public ModuleInstance {

    void test() override {
        std::cout << getTest() << std::endl;
        std::cout << "-------MOD2---------" << std::endl;
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
