//
// Created by Elec332 on 26/11/2021.
//

#include <nativesdr/module/SDRModule.h>
#include <nativesdr/rtl-sdr_block.h>

class RTLSDRModule : public ModuleInstance {

    void init(pipeline::node_manager* nodeManager, const SDRCoreContext* context) override {
        nodeManager->registerSourceBlockType("RTL-SDR Source", createRTLSDRSource);
    }

};


bool initModule() {
    return true;
}

const char* getModuleName() {
    return "RTL-SDR Module";
}

void onShutdownModule() {
}

ModuleInstance* createModuleContainer() {
    return new RTLSDRModule();
}

void destroyModuleContainer(ModuleInstance* instance) {
    delete (RTLSDRModule*) instance;
}