//
// Created by Elec332 on 26/11/2021.
//

#include <nativesdr/module/SDRModule.h>
#include <nativesdr/rtl-sdr_block.h>

class RTLSDRModule : public ModuleInstance {

    [[nodiscard]] std::string getName() const override {
        return "RTL-SDR Source";
    }

    void init(pipeline::node_manager* nodeManager) override {
        nodeManager->registerBlockType("RTL-SDR Source", createRTLSDRSource);
    }

};


void initModule() {
}

void onShutdownModule() {
}

ModuleInstance* createModuleContainer() {
    return new RTLSDRModule();
}

void destroyModuleContainer(ModuleInstance* instance) {
    delete (RTLSDRModule*) instance;
}