//
// Created by Elec332 on 26/11/2021.
//

#include <nativesdr/module/SDRModule.h>
#include <nativesdr/hackrf_block.h>
#include <libhackrf/hackrf.h>

class HackRFModule : public ModuleInstance {

    void init(pipeline::node_manager* nodeManager, const SDRCoreContext* context) override {
        nodeManager->registerSourceBlockType("HackRF Source", createHackRFSource);
    }

};


bool initModule() {
    hackrf_init();
    return true;
}

const char* getModuleName() {
    return "HackRF Module";
}

void onShutdownModule() {
    hackrf_exit();
}

ModuleInstance* createModuleContainer() {
    return new HackRFModule();
}

void destroyModuleContainer(ModuleInstance* instance) {
    delete (HackRFModule*) instance;
}